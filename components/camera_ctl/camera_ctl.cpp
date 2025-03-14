// ESP
#include <esp_log.h>
#include <esp_check.h>

// ESP-IDF
#include <driver/i2c.h>

#include "camera_ctl.hpp"

esp_err_t CameraCtl::init_camera(void)
{
    camera_config_t config = {
    CAM_PIN_PWDN,        // pin_pwdn
    CAM_PIN_RESET,       // pin_reset
    CAM_PIN_XCLK,        // pin_xclk
    CAM_PIN_SIOD,        // pin_sccb_sda
    CAM_PIN_SIOC,        // pin_sccb_scl
    CAM_PIN_D7,          // pin_d7
    CAM_PIN_D6,          // pin_d6
    CAM_PIN_D5,          // pin_d5
    CAM_PIN_D4,          // pin_d4
    CAM_PIN_D3,          // pin_d3
    CAM_PIN_D2,          // pin_d2
    CAM_PIN_D1,          // pin_d1
    CAM_PIN_D0,          // pin_d0
    CAM_PIN_VSYNC,       // pin_vsync
    CAM_PIN_HREF,        // pin_href
    CAM_PIN_PCLK,        // pin_pclk
    CONFIG_XCLK_FREQ,    // xclk_freq_hz
    LEDC_TIMER_0,        // ledc_timer
    LEDC_CHANNEL_0,      // ledc_channel
    PIXFORMAT_JPEG,//PIXFORMAT_GRAYSCALE, //PIXFORMAT_JPEG,      // pixel_format
    FRAMESIZE_QQVGA,      // frame_size
    12,                   // jpeg_quality
    1,                    // fb_count
    CAMERA_FB_IN_PSRAM,   // fb_location (or CAMERA_FB_IN_DRAM)
    CAMERA_GRAB_WHEN_EMPTY, // grab_mode
    I2C_NUM_0 // sccb_i2c_port
    };

    ESP_RETURN_ON_ERROR(camera_xclk_init(20'000'000), TAG, "xclk_init");
    ESP_RETURN_ON_ERROR(esp_camera_init(&config), TAG, "camera_init");
    return ESP_OK;
}


void CameraCtl::capture_do(std::function<void(const camera_fb_t*)> f)
{
    ESP_LOGI(TAG, "take picture!");
    pic = esp_camera_fb_get();
    ESP_LOGI(TAG, "apply action on picture");
    f(pic);
    ESP_LOGI(TAG, "release framebuffer");
    esp_camera_fb_return(pic);
}


esp_err_t CameraCtl::camera_xclk_init(uint32_t freq_hz) {

    // Configure the LEDC timer
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,  // High-speed mode
        .duty_resolution = LEDC_TIMER_1_BIT, // Minimal duty resolution for clock
        .timer_num = LEDC_TIMER_0,           // Use LEDC_TIMER_0
        .freq_hz = freq_hz,                  // Set the desired frequency
        .clk_cfg = LEDC_AUTO_CLK,            // Automatically select clock source
        .deconfigure = 0,
    };
    ESP_RETURN_ON_ERROR(ledc_timer_config(&ledc_timer), TAG, "ledc_timer");

    // Configure the LEDC channel for XCLK pin
    ledc_channel_config_t ledc_channel = {
        .gpio_num = CAM_PIN_XCLK, // Replace with your XCLK GPIO number
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,               // TODO: default
        .timer_sel = LEDC_TIMER_0,
        .duty = 1, // Minimal duty cycle for clock generation
        .hpoint = 0,
        .sleep_mode = LEDC_SLEEP_MODE_NO_ALIVE_NO_PD, // TODO: default
        .flags = { .output_invert = 1 },              // TODO: default
    };
    ESP_RETURN_ON_ERROR(ledc_channel_config(&ledc_channel), TAG, "ledc_channel");

    return ESP_OK;
}
