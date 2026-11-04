#include <stdint.h>
#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "soc/soc.h"
#include "soc/gpio_reg.h"

#define LED_GPIO GPIO_NUM_4
#define LED_MASK (1u << LED_GPIO)
#define PERIOD_COUNT 100000

static const char *TAG = "lab1";

static void led_init(void)
{
    // Pin hazirligi: laboratuvarda izin verilen surucu fonksiyonu.
    ESP_ERROR_CHECK(gpio_reset_pin(LED_GPIO));

    // Baslangicta LED sonuk; ardindan cikis surucusunu ac.
    REG_WRITE(GPIO_OUT_W1TC_REG, LED_MASK);
    REG_WRITE(GPIO_ENABLE_W1TS_REG, LED_MASK);
}

static void blink(void)
{
    for (int i = 0; i < 5; i++)
    {
        REG_WRITE(GPIO_OUT_W1TS_REG, LED_MASK);
        vTaskDelay(pdMS_TO_TICKS(500));

        REG_WRITE(GPIO_OUT_W1TC_REG, LED_MASK);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

static void measure_toggle_speed(void)
{
    int64_t start_us = esp_timer_get_time();

    for (int i = 0; i < PERIOD_COUNT; i++)
    {
        REG_WRITE(GPIO_OUT_W1TS_REG, LED_MASK);
        REG_WRITE(GPIO_OUT_W1TC_REG, LED_MASK);
    }

    int64_t elapsed_us = esp_timer_get_time() - start_us;

    if (elapsed_us > 0)
    {
        double frequency_hz =
            (double)PERIOD_COUNT * 1000000.0 / elapsed_us;

        ESP_LOGI(TAG,
                 "%d periyot, %" PRId64 " us, ortalama frekans: %.2f Hz",
                 PERIOD_COUNT, elapsed_us, frequency_hz);
    }
}

void app_main(void)
{
    led_init();

    ESP_LOGI(TAG, "Bolum A: LED 5 kez yanip sonecek.");
    blink();

    ESP_LOGI(TAG, "Bolum B: Hiz olcumu basliyor.");

    while (1)
    {
        measure_toggle_speed();

        // Diger gorevlere calisma suresi birak.
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}