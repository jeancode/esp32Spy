#include <WiFi.h>
#include <WiFiUdp.h>
#include "driver/i2s.h"

// Configuración de red
const char *ssid = "NombreDeMired";          // Reemplaza con tu SSID
const char *password = "Password";               // Reemplaza con tu contraseña
const char *udpAddress = "192.168.0.193";    // IP del receptor (tu computadora)
const int udpPort = 12345;                   // Puerto UDP

WiFiUDP udp;

#define I2S_NUM         I2S_NUM_0
#define SAMPLE_RATE     48000
#define BITS_PER_SAMPLE I2S_BITS_PER_SAMPLE_32BIT
#define GAIN            2  // Factor de ganancia (ajusta según sea necesario)

void setupI2S() {

    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = BITS_PER_SAMPLE,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 64,
        .use_apll = false
    };

    i2s_pin_config_t pin_config = {
        .bck_io_num = 14,      // Pin SCK
        .ws_io_num = 15,       // Pin WS
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = 32      // Pin SD
    };

    i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM, &pin_config);
}

void setup() {

    Serial.begin(115200);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Conectado a WiFi");
    udp.begin(udpPort);
    
    setupI2S();
}

void loop() {

    int32_t samples[128];  // Cambia a 32 bits
    size_t bytes_read;

    i2s_read(I2S_NUM, samples, sizeof(samples), &bytes_read, portMAX_DELAY);
    
    if (bytes_read > 0) {
        // Amplifica las muestras
        for (size_t i = 0; i < bytes_read / sizeof(int32_t); i++) {
            // Amplifica y limita a 32 bits
            samples[i] = constrain(samples[i] * GAIN, INT32_MIN, INT32_MAX);
        }
        udp.beginPacket(udpAddress, udpPort);
        udp.write((uint8_t *)samples, bytes_read); // Enviar datos amplificados
        udp.endPacket();
    }
}
