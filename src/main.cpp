#include <Arduino.h>
#include <STM32FreeRTOS.h>


// for 野火 STM32-MINI
#define LED_0 PC_2
#define LED_1 PC_3
#define LED_ON LOW
#define LED_OFF HIGH
#define BUTTON PA_0

SemaphoreHandle_t sem;

static void LedBlinkTask1(void *pvParameters)
{
    const TickType_t xDelay = 200 / portTICK_PERIOD_MS;
    while (1) {
        digitalWrite(LED_0, LED_ON);
        vTaskDelay(xDelay);
        digitalWrite(LED_0, LED_OFF);
        vTaskDelay(xDelay);
    }
}

static void LedBlinkTask2(void *pvParameters)
{
    const TickType_t xDelay = 1000 / portTICK_PERIOD_MS;
    while (1) {
        digitalWrite(LED_1, LED_ON);
        vTaskDelay(xDelay);
        digitalWrite(LED_1, LED_OFF);
        vTaskDelay(xDelay);
    }
}

static void SerialTask(void *pvParameters)
{
    while (1) {
        xSemaphoreTake(sem, portMAX_DELAY);
        Serial.println("Key pressed");
    }
}

static void ButtonPressISR()
{
    BaseType_t xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;

    xSemaphoreGiveFromISR(sem, &xHigherPriorityTaskWoken);

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void setup()
{
    Serial.begin(115200);
    pinMode(LED_0, OUTPUT);
    pinMode(LED_1, OUTPUT);

    Serial.println("Starting...");

    sem = xSemaphoreCreateBinary();
    attachInterrupt(digitalPinToInterrupt(BUTTON), ButtonPressISR, RISING);

    xTaskCreate(LedBlinkTask1,
                "blink1",
                configMINIMAL_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY + 3,
                NULL);

    xTaskCreate(LedBlinkTask2,
                "blink2",
                configMINIMAL_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY + 2,
                NULL);
    
    xTaskCreate(SerialTask,
                "serial",
                configMINIMAL_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY + 1,
                NULL);

    vTaskStartScheduler();

    Serial.println("Die");
    while (1) {
    };
}

void loop()
{
}