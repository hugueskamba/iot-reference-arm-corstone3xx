/* Copyright 2021-2023 Arm Limited and/or its affiliates
 * <open-source-office@arm.com>
 * SPDX-License-Identifier: MIT
 */

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

#include "ml_interface.h"
#include "AsrClassifier.hpp"
#include "AsrResult.hpp"
#include "AudioUtils.hpp"
#include "BufAttributes.hpp"
#include "Labels.hpp"
#include "OutputDecode.hpp"
#include "TensorFlowLiteMicro.hpp"
#include "UseCaseCommonUtils.hpp"
#include "Wav2LetterMfcc.hpp"
#include "Wav2LetterModel.hpp"
#include "Wav2LetterPostprocess.hpp"
#include "Wav2LetterPreprocess.hpp"
#include CMSIS_device_header
#include "device_mps3.h" /* FPGA level definitions and functions. */
#include "dsp_interfaces.h"
#include "ethos-u55.h"     /* Mem map and configuration definitions of the Ethos U55 */
#include "ethosu_driver.h" /* Arm Ethos-U55 driver header */
#include "model_config.h"
#include "timer_mps3.h"     /* Timer functions. */
#include "timing_adapter.h" /* Driver header of the timing adapter */
#include "log_macros.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <stdbool.h>
#include <string>
#include <utility>
#include <vector>

extern "C" {
/* Include header that defines log levels. */
#include "logging_levels.h"
/* Configure name and log level. */
#ifndef LIBRARY_LOG_NAME
    #define LIBRARY_LOG_NAME     "ML_Interface"
#endif
#ifndef LIBRARY_LOG_LEVEL
    #define LIBRARY_LOG_LEVEL    LOG_INFO
#endif
#include "logging_stack.h"
}

#include "audio_config.h"
#include "app_config.h"
extern "C" {
#include "events.h"
}

extern EventGroupHandle_t xAppEvents;
extern QueueHandle_t xMlMqttQueue;

// Processing state
static SemaphoreHandle_t xMlMutex = NULL;

// Define tensor arena and declare functions required to access the model
namespace arm {
namespace app {
uint8_t tensorArena[ACTIVATION_BUF_SZ] ACTIVATION_BUF_ATTRIBUTE;
namespace asr {
extern uint8_t *GetModelPointer();
extern size_t GetModelLen();
} /* namespace asr */
} /* namespace app */
} /* namespace arm */

namespace {

#define ML_EVENT_START (1 << 0)
#define ML_EVENT_STOP  (1 << 1)

typedef struct {
    char *result;
} ml_mqtt_msg_t;

// Import
using namespace arm::app;

extern "C" {
void ml_task_inference_start(void)
{
    if(xAppEvents == NULL)
    {
        LogError( ( "xAppEvents is not initialised\r\n" ) );
        return;
    }

    LogInfo( ( "Signal task inference start\r\n" ) );
    ( void )xEventGroupClearBits( xAppEvents, (EventBits_t)ML_EVENT_STOP );
    ( void )xEventGroupSetBits( xAppEvents, (EventBits_t)ML_EVENT_START );

}

void ml_task_inference_stop(void)
{
    if(xAppEvents == NULL)
    {
        LogError( ( "xAppEvents is not initialised\r\n" ) );
        return;
    }

    LogInfo( ( "Signal task inference stop\r\n" ) );
    ( void )xEventGroupClearBits( xAppEvents, (EventBits_t)ML_EVENT_START );
    ( void )xEventGroupSetBits( xAppEvents, (EventBits_t)ML_EVENT_STOP );
}

void vStartMlTask( void *pvParameters )
{
    if (
        xTaskCreate(
            ml_task,
            "ML_TASK",
            appCONFIG_ML_TASK_STACK_SIZE,
            pvParameters,
            appCONFIG_ML_TASK_PRIORITY,
            NULL
        ) != pdPASS
    ) {
        LogError( ( "Failed to create ML Task\r\n" ) );
    }
}

void vStartMlMqttTask( void )
{
    if (
        xTaskCreate(
            ml_mqtt_task,
            "ML_MQTT",
            appCONFIG_ML_MQTT_TASK_STACK_SIZE,
            NULL,
            appCONFIG_ML_MQTT_TASK_PRIORITY,
            NULL
        ) != pdPASS
    ) {
        LogError( ( "Failed to create ML Mqtt Task\r\n" ) );
    }
}

void mqtt_send_message(const char *message);
} // extern "C" {


static bool ml_lock(void)
{
    if (xMlMutex == NULL) {
        return false;
    }

    BaseType_t wasSemaphoreTaken = xSemaphoreTake( xMlMutex, portMAX_DELAY );
    if (wasSemaphoreTaken != pdTRUE) {
        LogError( ( "xSemaphoreTake xMlMutex failed %ld\r\n", wasSemaphoreTaken ) );
        return false;
    }

    return true;
}

static bool ml_unlock(void)
{

    if (xMlMutex == NULL) {
        return false;
    }

    BaseType_t wasSemaphoreGiven = xSemaphoreGive( xMlMutex );
    if (wasSemaphoreGiven != pdTRUE) {
        LogError( ( "xSemaphoreGive xMlMutex failed %ld\r\n", wasSemaphoreGiven) );
        return false;
    }

    return true;
}

static void set_ml_processing_state(const char *inference_result)
{
    if (!ml_lock()) {
        return;
    }

    size_t msg_len = strlen(inference_result) + 1;
    char *msg_result = reinterpret_cast<char *>(malloc(msg_len));
    if (msg_result) {
        if(xMlMqttQueue == NULL)
        {
            LogError( ( "xMlMqttQueue is not initialised\r\n" ) );
            free(reinterpret_cast<void *>(msg_result));
            return;
        }

        memcpy(msg_result, inference_result, msg_len);
        const ml_mqtt_msg_t msg = {msg_result};
        if (xQueueSendToBack(xMlMqttQueue, (void *)&msg, (TickType_t)0) != pdTRUE) {
            LogError( ( "Failed to send message to xMlMqttQueue\r\n" ) );
            free(reinterpret_cast<void *>(msg_result));
        }
    } else {
        LogWarn( ( "Failed to send ml processing inference_result (alloc failure)" ) );
    }

    ml_unlock();
}

// Model
arm::app::ApplicationContext caseContext;

/**
 * @brief           Presents inference results using the data presentation
 *                  object.
 * @param[in]       platform    Reference to the hal platform object.
 * @param[in]       results     Vector of classification results to be displayed.
 * @return          true if successful, false otherwise.
 **/
static bool PresentInferenceResult(const std::vector<arm::app::asr::AsrResult> &results);

extern "C" {
int ml_get_frame_length(void)
{
    return caseContext.Get<int>("frameLength"); // 640
}

int ml_get_frame_stride(void)
{
    return caseContext.Get<int>("frameStride"); // 320
}
}

void ProcessAudio(ApplicationContext &ctx, DSPML *dspMLConnection)
{
    /* Get model reference. */
    auto &model = ctx.Get<Model &>("model");
    if (!model.IsInited()) {
        LogError( ( "Model is not initialised! Terminating processing.\n" ) );
        return;
    }

    /* Get score threshold to be applied for the classifier (post-inference). */
    auto scoreThreshold = ctx.Get<float>("scoreThreshold");

    /* Get tensors. Dimensions of the tensor should have been verified by
     * the callee. */
    TfLiteTensor *inputTensor = model.GetInputTensor(0);
    TfLiteTensor *outputTensor = model.GetOutputTensor(0);
    TfLiteIntArray *inputShape = model.GetInputShape(0);

    /* Populate MFCC related parameters. */
    auto mfccFrameLen = ctx.Get<uint32_t>("frameLength");
    auto mfccFrameStride = ctx.Get<uint32_t>("frameStride");

    /* Populate ASR inference context and inner lengths for input. */
    auto inputCtxLen = ctx.Get<uint32_t>("ctxLen");

    /* Get pre/post-processing objects. */
    AsrPreProcess preProcess = AsrPreProcess(inputTensor,
                                             Wav2LetterModel::ms_numMfccFeatures,
                                             inputShape->data[Wav2LetterModel::ms_inputRowsIdx],
                                             mfccFrameLen,
                                             mfccFrameStride);

    std::vector<ClassificationResult> singleInfResult;
    const uint32_t outputCtxLen = AsrPostProcess::GetOutputContextLen(model, inputCtxLen);
    AsrPostProcess postProcess = AsrPostProcess(outputTensor,
                                                ctx.Get<AsrClassifier &>("classifier"),
                                                ctx.Get<std::vector<std::string> &>("labels"),
                                                singleInfResult,
                                                outputCtxLen,
                                                Wav2LetterModel::ms_blankTokenIdx,
                                                Wav2LetterModel::ms_outputRowsIdx);

    const uint32_t inputRows = inputTensor->dims->data[arm::app::Wav2LetterModel::ms_inputRowsIdx];
    /* Audio data stride corresponds to inputInnerLen feature vectors. */
    const uint32_t audioParamsWinLen = inputRows * mfccFrameStride;

    auto inferenceWindow = std::vector<int16_t>(audioParamsWinLen, 0);
    size_t inferenceWindowLen = audioParamsWinLen;

    // Start processing audio data as it arrive
    uint32_t inferenceIndex = 0;
    // We do inference on 2 audio segments before reporting a result
    // We do not have the concept of audio clip in a streaming application
    // so we need to decide when a sentenced is finished to start a recognition.
    // It was arbitrarily chosen to be 2 inferences.
    // In a real app, a voice activity detector would probably be used
    // to detect a long silence between 2 sentences.
    const uint32_t maxNbInference = 2;
    std::vector<arm::app::asr::AsrResult> results;

    while (true) {
        while (true) {
            // Adding 10 ticks delay to block this task for some time allowing
            // other task to be scheduled to run on the CPU.
            vTaskDelay(10);
            EventBits_t flags = xEventGroupWaitBits(
                xAppEvents, (EventBits_t)ML_EVENT_STOP, pdTRUE, pdFAIL, 0
            );

            if (flags & ML_EVENT_STOP) {
                LogInfo( ( "Stopping audio processing\r\n" ) );
                break;
            }

            // Wait for the DSP task signal to start the recognition
            dspMLConnection->waitForDSPData();

            int16_t *p = inferenceWindow.data();
            dspMLConnection->copyFromMLBufferInto(p);

            // This timestamp is corresponding to the time when
            // inference is starting and not to the time of the
            // beginning of the audio segment used for this inference.
            float currentTimeStamp = get_audio_timestamp();
            LogInfo( ( "Inference %i/%i\n", inferenceIndex + 1, maxNbInference ) );

            /* Run the pre-processing, inference and post-processing. */
            if (!preProcess.DoPreProcess(inferenceWindow.data(), inferenceWindowLen)) {
                LogError( ( "Pre-processing failed." ) );
            }

#ifdef AUDIO_VSI
            LogInfo( ( "Start running inference on audio input from the Virtual Streaming Interface\r\n" ) );
#else
            LogInfo( ( "Start running inference on an audio clip in local memory\r\n" ) );
#endif

            /* Run inference over this audio clip sliding window. */
            if (!model.RunInference()) {
                LogError( ( "Failed to run inference" ) );
                return;
            }

            LogInfo( ( "Doing post processing\n" ) );

            /* Post processing needs to know if we are on the last audio window. */
            // postProcess.m_lastIteration = !audioDataSlider.HasNext();
            if (!postProcess.DoPostProcess()) {
                LogError( ( "Post-processing failed." ) );
            }

            LogInfo( ( "Inference done\n" ) );

            std::vector<ClassificationResult> classificationResult;
            auto &classifier = ctx.Get<AsrClassifier &>("classifier");
            classifier.GetClassificationResults(
                outputTensor,
                classificationResult,
                ctx.Get<std::vector<std::string> &>("labels"),
                1,
                true
            );

            auto result = asr::AsrResult(
                classificationResult,
                currentTimeStamp,
                inferenceIndex,
                scoreThreshold
            );

            results.emplace_back(result);

            inferenceIndex = inferenceIndex + 1;
            if (inferenceIndex == maxNbInference) {

                inferenceIndex = 0;

                ctx.Set<std::vector<arm::app::asr::AsrResult>>("results", results);

                if (!PresentInferenceResult(results)) {
                    return;
                }

                results.clear();
            }

            // Inference loop
        } /* while (true) */

        while (true) {
            EventBits_t flags = xEventGroupWaitBits(
                xAppEvents, (EventBits_t)ML_EVENT_START, pdTRUE, pdFAIL, portMAX_DELAY
            );

            if (flags & ML_EVENT_START) {
                LogError( ( "Restarting audio processing %u\r\n", flags ) );
                break;
            }
        }
    } /* while (true) */
}

static bool PresentInferenceResult(const std::vector<arm::app::asr::AsrResult> &results)
{
    LogInfo( ( "Final results:\n" ) );
    LogInfo( ( "Total number of inferences: %zu\n", results.size() ) );
    /* Results from multiple inferences should be combined before processing. */
    std::vector<arm::app::ClassificationResult> combinedResults;
    for (auto &result : results) {
        combinedResults.insert(combinedResults.end(), result.m_resultVec.begin(), result.m_resultVec.end());
    }

    /* Get each inference result string using the decoder. */
    for (const auto &result : results) {
        std::string infResultStr = audio::asr::DecodeOutput(result.m_resultVec);

        LogInfo( ( "For timestamp: %f (inference #: %" PRIu32 "); label: %s\r\n",
             (double)result.m_timeStamp,
             result.m_inferenceNumber,
             infResultStr.c_str() ) );
    }

    /* Get the decoded result for the combined result. */
    std::string finalResultStr = audio::asr::DecodeOutput(combinedResults);

    LogInfo( ( "Complete recognition: %s\n", finalResultStr.c_str() ) );

    // Send the inference result
    set_ml_processing_state(finalResultStr.c_str());

    return true;
}

} // anonymous namespace

#ifdef USE_ETHOS
extern struct ethosu_driver ethosu_drv; /* Default Ethos-U55 device driver */

/**
 * @brief   Initialises the Arm Ethos-U55 NPU
 * @return  0 if successful, error code otherwise
 **/
static int arm_npu_init(void);

/**
 * @brief   Defines the Ethos-U interrupt handler: just a wrapper around the default
 *          implementation.
 **/
extern "C" {
void ETHOS_U55_Handler(void)
{
    /* Call the default interrupt handler from the NPU driver */
    ethosu_irq_handler(&ethosu_drv);
}
}

/**
 * @brief  Initialises the NPU IRQ
 **/
static void arm_npu_irq_init(void)
{
    const IRQn_Type ethosu_irqnum = (IRQn_Type)EthosU_IRQn;

    NVIC_EnableIRQ(ethosu_irqnum);

    LogDebug( ( "EthosU IRQ#: %u, Handler: 0x%p\n", ethosu_irqnum, ETHOS_U55_Handler ) );
}

static int _arm_npu_timing_adapter_init(void)
{
#if defined(TA0_BASE)
    struct timing_adapter ta_0;
    struct timing_adapter_settings ta_0_settings = {
        .maxr = TA0_MAXR,
        .maxw = TA0_MAXW,
        .maxrw = TA0_MAXRW,
        .rlatency = TA0_RLATENCY,
        .wlatency = TA0_WLATENCY,
        .pulse_on = TA0_PULSE_ON,
        .pulse_off = TA0_PULSE_OFF,
        .bwcap = TA0_BWCAP,
        .perfctrl = TA0_PERFCTRL,
        .perfcnt = TA0_PERFCNT,
        .mode = TA0_MODE,
        .maxpending = 0, /* This is a read-only parameter */
        .histbin = TA0_HISTBIN,
        .histcnt = TA0_HISTCNT
    };

    if (ta_init(&ta_0, TA0_BASE) != 0) {
        LogError( ( "TA0 initialisation failed\n" ) );
        return 1;
    }

    ta_set_all(&ta_0, &ta_0_settings);
#endif /* defined (TA0_BASE) */

#if defined(TA1_BASE)
    struct timing_adapter ta_1;
    struct timing_adapter_settings ta_1_settings = {
        .maxr = TA1_MAXR,
        .maxw = TA1_MAXW,
        .maxrw = TA1_MAXRW,
        .rlatency = TA1_RLATENCY,
        .wlatency = TA1_WLATENCY,
        .pulse_on = TA1_PULSE_ON,
        .pulse_off = TA1_PULSE_OFF,
        .bwcap = TA1_BWCAP,
        .perfctrl = TA1_PERFCTRL,
        .perfcnt = TA1_PERFCNT,
        .mode = TA1_MODE,
        .maxpending = 0, /* This is a read-only parameter */
        .histbin = TA1_HISTBIN,
        .histcnt = TA1_HISTCNT
    };

    if (ta_init(&ta_1, TA1_BASE) != 0) {
        LogError( ( "TA1 initialisation failed\n" ) );
        return 1;
    }

    ta_set_all(&ta_1, &ta_1_settings);
#endif /* defined (TA1_BASE) */

    return 0;
}

static int arm_npu_init(void)
{
    int err = 0;

    /* If the platform has timing adapter blocks along with Ethos-U55 core
     * block, initialise them here. */
    // cppcheck-suppress knownConditionTrueFalse
    if (0 != (err = _arm_npu_timing_adapter_init())) {
        return err;
    }

    /* Initialise the IRQ */
    arm_npu_irq_init();

    /* Initialise Ethos-U55 device */
    void *const ethosu_base_address = reinterpret_cast<void *const>(SEC_ETHOS_U55_BASE);

    if (0
        != (err = ethosu_init(&ethosu_drv,         /* Ethos-U55 driver device pointer */
                              ethosu_base_address, /* Ethos-U55's base address. */
                              NULL,                /* Pointer to fast mem area - NULL for U55. */
                              0,                   /* Fast mem region size. */
                              0,                   /* Security enable. */
                              0))) {               /* Privilege enable. */
        LogError( ( "failed to initalise Ethos-U55 device\n" ) );
        return err;
    }

    LogInfo( ( "Ethos-U55 device initialised\n" ) );

    /* Get Ethos-U55 version */
    struct ethosu_driver_version driver_version;
    struct ethosu_hw_info hw_info;

    ethosu_get_driver_version(&driver_version);
    ethosu_get_hw_info(&ethosu_drv, &hw_info);

    LogInfo( ( "Ethos-U version info:\n" ) );
    LogInfo( ( "\tArch:       v%" PRIu32 ".%" PRIu32 ".%" PRIu32 "\n",
         hw_info.version.arch_major_rev,
         hw_info.version.arch_minor_rev,
         hw_info.version.arch_patch_rev ) );
    LogInfo( ( "\tDriver:     v%" PRIu8 ".%" PRIu8 ".%" PRIu8 "\n",
         driver_version.major,
         driver_version.minor,
         driver_version.patch ) );
    LogInfo( ( "\tMACs/cc:    %" PRIu32 "\n", (uint32_t)(1 << hw_info.cfg.macs_per_cc) ) );
    LogInfo( ( "\tCmd stream: v%" PRIu32 "\n", hw_info.cfg.cmd_stream_version ) );

    return 0;
}
#endif /* USE_ETHOS */

extern "C" {

int ml_interface_init(void)
{
    static arm::app::Wav2LetterModel model;    /* Model wrapper object. */
    static arm::app::AsrClassifier classifier; /* Classifier wrapper object. */
    static std::vector<std::string> labels;

#ifdef USE_ETHOS
    // Initialize the ethos U55
    if (arm_npu_init() != 0) {
        LogError( ( "Failed to arm npu\n" ) );
        return -1;
    }
#endif /* USE_ETHOS */

    /* Load the model. */
    if (!model.Init(::arm::app::tensorArena,
                    sizeof(::arm::app::tensorArena),
                    ::arm::app::asr::GetModelPointer(),
                    ::arm::app::asr::GetModelLen())) {
        LogError( ( "Failed to initialise model\n" ) );
        return -1;
    }

    /* Initialise post-processing. */
    GetLabelsVector(labels);

    /* Instantiate application context. */
    caseContext.Set<arm::app::Model &>("model", model);
    caseContext.Set<uint32_t>("frameLength", g_FrameLength);
    caseContext.Set<uint32_t>("frameStride", g_FrameStride);
    caseContext.Set<uint32_t>("ctxLen", g_ctxLen);

    caseContext.Set<float>("scoreThreshold", g_ScoreThreshold); /* Normalised score threshold. */

    caseContext.Set<const std::vector<std::string> &>("labels", labels);
    caseContext.Set<arm::app::AsrClassifier &>("classifier", classifier);

    PrintTensorFlowVersion();
    LogInfo( ( "*** ML interface initialised\r\n" ) );
    return 0;
}

void ml_task(void *pvParameters)
{
    DSPML *dspMLConnection = static_cast<DSPML *>(pvParameters);

    xMlMutex = xSemaphoreCreateMutex();
    if (xMlMutex == NULL) {
        LogError( ( "Failed to create xMlMutex\r\n" ) );
        return;
    }

    while (true) {
        EventBits_t flags = xEventGroupWaitBits(
            xAppEvents, (EventBits_t)ML_EVENT_START, pdTRUE, pdFAIL, portMAX_DELAY
        );

        if (flags & ML_EVENT_START) {
            LogInfo( ( "Initial start of audio processing\r\n" ) );
            break;
        }
        else {
            LogError( ( "Failed to wait for ML_EVENT_START flag: %08X\r\n", flags ) );
            return;
        }
    }

    if (ml_interface_init() < 0) {
        LogError( ( "ml_interface_init failed\r\n" ) );
        return;
    }

    ProcessAudio(caseContext, dspMLConnection);
}

void ml_mqtt_task(void *pvParameters)
{
    (void)pvParameters;

    while (1) {
        ml_mqtt_msg_t msg;
        if (xQueueReceive (xMlMqttQueue, &msg, portMAX_DELAY) == pdTRUE) {
            mqtt_send_message(msg.result);
            free(reinterpret_cast<void *>(msg.result));
        } else {
            LogError( ( "xQueueReceive ML MQTT msg queue failed\r\n" ) );
        }
    }
}

} // extern "C"
