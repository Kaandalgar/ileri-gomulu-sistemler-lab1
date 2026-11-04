#include <time.h>
#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>

int main(void)
{
    const unsigned int pin = 17;
    int result = 1;

    struct gpiod_chip *chip = NULL;
    struct gpiod_line_settings *settings = NULL;
    struct gpiod_line_config *config = NULL;
    struct gpiod_line_request *request = NULL;

    chip = gpiod_chip_open("/dev/gpiochip0");
    if (!chip) {
        perror("GPIO denetleyicisi acilamadi");
        goto cleanup;
    }

    settings = gpiod_line_settings_new();
    config = gpiod_line_config_new();

    if (!settings || !config) {
        fprintf(stderr, "Bellek ayrilamadi\n");
        goto cleanup;
    }

    if (gpiod_line_settings_set_direction(
            settings, GPIOD_LINE_DIRECTION_OUTPUT) < 0 ||
        gpiod_line_settings_set_output_value(
            settings, GPIOD_LINE_VALUE_INACTIVE) < 0 ||
        gpiod_line_config_add_line_settings(
            config, &pin, 1, settings) < 0) {
        perror("GPIO ayarlanamadi");
        goto cleanup;
    }

    request = gpiod_chip_request_lines(chip, NULL, config);
    if (!request) {
        perror("GPIO17 kontrolu alinamadi");
        goto cleanup;
    }

    for (int i = 0; i < 5; i++) {
        if (gpiod_line_request_set_value(
                request, pin, GPIOD_LINE_VALUE_ACTIVE) < 0) {
            perror("LED yakilamadi");
            goto cleanup;
        }

        printf("%d: LED yandi\n", i + 1);
        usleep(500000);

        if (gpiod_line_request_set_value(
                request, pin, GPIOD_LINE_VALUE_INACTIVE) < 0) {
            perror("LED sondurulemedi");
            goto cleanup;
        }

        printf("%d: LED sondu\n", i + 1);
        usleep(500000);
    }

    const int period_count = 100000;
    struct timespec start, end;

    printf("Hiz olcumu basliyor...\n");

    if (clock_gettime(CLOCK_MONOTONIC, &start) < 0) {
        perror("Baslangic zamani okunamadi");
        goto cleanup;
    }

    for (int i = 0; i < period_count; i++) {
        if (gpiod_line_request_set_value(
                request, pin, GPIOD_LINE_VALUE_ACTIVE) < 0 ||
            gpiod_line_request_set_value(
                request, pin, GPIOD_LINE_VALUE_INACTIVE) < 0) {
            perror("Hiz olcumunde GPIO hatasi");
            goto cleanup;
        }
    }

    if (clock_gettime(CLOCK_MONOTONIC, &end) < 0) {
        perror("Bitis zamani okunamadi");
        goto cleanup;
    }

    double elapsed_s = (double)(end.tv_sec - start.tv_sec)
                     + (double)(end.tv_nsec - start.tv_nsec) / 1e9;

    if (elapsed_s > 0.0) {
        printf("Periyot sayisi: %d\n", period_count);
        printf("Gecen sure: %.6f saniye\n", elapsed_s);
        printf("Ortalama frekans: %.2f Hz\n",
               period_count / elapsed_s);
    }

    result = 0;

cleanup:
    if (request) {
        gpiod_line_request_set_value(
            request, pin, GPIOD_LINE_VALUE_INACTIVE);
        gpiod_line_request_release(request);
    }

    if (config)
        gpiod_line_config_free(config);
    if (settings)
        gpiod_line_settings_free(settings);
    if (chip)
        gpiod_chip_close(chip);

    return result;
}
