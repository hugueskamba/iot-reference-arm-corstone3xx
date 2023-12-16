/* Copyright 2023 Arm Limited and/or its affiliates
 * <open-source-office@arm.com>
 * SPDX-License-Identifier: MIT
 */

#include "fff.h"

#include "gtest/gtest.h"

extern "C" {
    #include "ota_agent_task.h"
    #include "task.h"
    #include "version/application_version.h"
    #include "logging_stack.h"
    #include "semphr.h"
    #include "queue.h"
}

DEFINE_FFF_GLOBALS

class TestOTAAgentTask : public ::testing::Test {
public:
    TestOTAAgentTask()
    {
        RESET_FAKE(xTaskCreate);
        RESET_FAKE(xTaskCreate);
        RESET_FAKE(GetImageVersionPSA);
        RESET_FAKE(xSemaphoreCreateMutex);
        RESET_FAKE(vSemaphoreDelete);
        RESET_FAKE(LogError);
        RESET_FAKE(LogInfo);
    }
};

class TestOTAAgentTaskCreatedOTATask : public TestOTAAgentTask {
public:
    TestOTAAgentTaskCreatedOTATask() : TestOTAAgentTask()
    {
        vStartOtaTask();

        ota_task = xTaskCreate_fake.arg0_val;
        EXPECT_NE(nullptr, ota_task);
    }

    TaskFunction_t ota_task = nullptr;
    struct QueueDefinition buffer_queue;
    SemaphoreHandle_t buffer_semaphore = &buffer_queue;
};

TEST_F(TestOTAAgentTask, creating_an_ota_task_registers_a_callback)
{
    TaskFunction_t ota_task = nullptr;

    ota_task = xTaskCreate_fake.arg0_val;
    EXPECT_EQ(nullptr, ota_task);

    vStartOtaTask();

    ota_task = xTaskCreate_fake.arg0_val;
    EXPECT_NE(nullptr, ota_task);
}

TEST_F(TestOTAAgentTaskCreatedOTATask, failure_to_obtain_the_psa_image_version_does_not_stop_ota_task)
{
    GetImageVersionPSA_fake.return_val = -1;

    ota_task(nullptr);

    // The request to create the OTA event buffer mutex still goes ahead
    EXPECT_EQ(1, xSemaphoreCreateMutex_fake.call_count);
}

