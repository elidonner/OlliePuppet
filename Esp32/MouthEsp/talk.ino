/*
 * This is an example to read analog data at high frequency using the I2S peripheral
 * Reads in filtered audio signal from aux chord
 * The readings from the device will be 12bit (0-4096)
 * Then automates a servo to make puppet seem like it is talking
 */
#include <driver/i2s.h>
#include <espServo.h>

#define I2S_SAMPLE_RATE 40000
#define ADC_INPUT ADC1_CHANNEL_4 // pin 32

/**** Comm stuff *****/
#define FROM_ESP 15

/**** SERVOSTUFF *****/
#define SERVOPIN 25
espServo mouth(25, 0, 1.23, 1.45);

uint16_t adc_reading;

const i2s_port_t I2S_PORT = I2S_NUM_0;

const int DMA_SIZE = 1024;           // max I2S is 1024
const int BLOCK_SIZE = DMA_SIZE / 2; // larger block size -> finer FFT time resolution bands,

//  SPH0645 requires 1 to 4Mhz clock. 64 bits per sample means 16Khz to 64Khz sample rate
// but I2S samples are 32 bits so sampling freq must be 32Khz to 128Khz.
const int samplingFrequency = I2S_SAMPLE_RATE;

int32_t samples[DMA_SIZE];

void i2sInit()
{
  i2s_config_t i2s_config = {
      .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_ADC_BUILT_IN),
      .sample_rate = I2S_SAMPLE_RATE,               // The format of the signal using ADC_BUILT_IN
      .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, // is fixed at 12bit, stereo, MSB
      .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
      .communication_format = I2S_COMM_FORMAT_STAND_I2S,
      .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
      .dma_buf_count = 4,
      .dma_buf_len = 8,
      .use_apll = false,
      .tx_desc_auto_clear = false,
      .fixed_mclk = 0};
  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_adc_mode(ADC_UNIT_1, ADC_INPUT);
  i2s_adc_enable(I2S_NUM_0);
}

void setup()
{
  Serial.begin(115200);
  delay(100);

  // Initialize the I2S peripheral
  i2sInit();
}

void loop()
{
  size_t num_bytes_read;
  int32_t val;
  static float maxsample, avgsample, smoothed;
  static uint32_t oldus;

  if (digitalRead(FROM_ESP) == HIGH)
  {

    i2s_read(I2S_PORT, (char *)samples, DMA_SIZE * sizeof(samples[0]), &num_bytes_read, portMAX_DELAY); // no timeout
    for (uint16_t i = 0; i < BLOCK_SIZE; i++)
    {
      val = ((~samples[i * 2]) + 1) >> 14; // SPH0645 sends 18bits of 2's complement values
      avgsample = (avgsample * 20000 + val) / 20001;
      if (abs(val - (int)avgsample) > maxsample)
        maxsample = abs(val - (int)avgsample); // SPH0645 assumes 2 devices (L & R) so we throw out one
      else
        maxsample = (maxsample * 2000 + abs(val - (int)avgsample)) / 2001;
    }

    if (micros() - oldus > 1000000 / LEDC_FREQ_HZ)
    {
      oldus = micros();

      smoothed = (smoothed * 2 + maxsample) / 3; // low pass to make it less jittery
      // Serial.printf("smoothed: %d \t", (int)smoothed);
      // samples range from 50 to 2000
      if (smoothed > 250.0) // noise threshold
      {
        int servo_value = constrain((int)smoothed, 250, 7000);
        servo_value = map(servo_value, 250, 7000, 0, 100);
        // Serial.printf("servo_value: %d \n", servo_value);
        mouth.sendServo(servo_value);
        // sendServo(constrain(CLOSED + ((smoothed - 2500.0) / 20000.0), CLOSED, OPEN)); // send 1.0 to 2.0 ms
      }
      else
      {
        mouth.sendServo(0); // close mouth
      }
    }
    
  }
  else
  {
    mouth.sendPulse(0.0); // shut off the mouth
  }
}