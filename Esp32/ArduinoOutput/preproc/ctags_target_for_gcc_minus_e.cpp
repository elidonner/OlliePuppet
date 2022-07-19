# 1 "/Users/ethandonlon/Documents/School/Fifth Year/FWM/OlliePuppet/Esp32/tests/talk/talk.ino"
/*
 * This is an example to read analog data at high frequency using the I2S peripheral
 * Reads in filtered audio signal from aux chord
 * The readings from the device will be 12bit (0-4096)
 * Then automates a servo to make puppet seem like it is talking
 */
# 8 "/Users/ethandonlon/Documents/School/Fifth Year/FWM/OlliePuppet/Esp32/tests/talk/talk.ino" 2
# 9 "/Users/ethandonlon/Documents/School/Fifth Year/FWM/OlliePuppet/Esp32/tests/talk/talk.ino" 2




/**** SERVOSTUFF *****/

espServo mouth(25, 0, 1.15, 1.40);

uint16_t adc_reading;

const i2s_port_t I2S_PORT = I2S_NUM_0;


const int DMA_SIZE = 1024; //max I2S is 1024
const int BLOCK_SIZE = DMA_SIZE / 2; // larger block size -> finer FFT time resolution bands,

//  SPH0645 requires 1 to 4Mhz clock. 64 bits per sample means 16Khz to 64Khz sample rate
// but I2S samples are 32 bits so sampling freq must be 32Khz to 128Khz.
const int samplingFrequency = 40000;

int32_t samples[DMA_SIZE];

void i2sInit()
{
   i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_ADC_BUILT_IN),
    .sample_rate = 40000, // The format of the signal using ADC_BUILT_IN
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, // is fixed at 12bit, stereo, MSB
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = (1<<1) /*|< Accept a Level 1 interrupt vector (lowest priority)*/,
    .dma_buf_count = 4,
    .dma_buf_len = 8,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
   };
   i2s_driver_install(I2S_NUM_0, &i2s_config, 0, 
# 46 "/Users/ethandonlon/Documents/School/Fifth Year/FWM/OlliePuppet/Esp32/tests/talk/talk.ino" 3 4
                                                __null
# 46 "/Users/ethandonlon/Documents/School/Fifth Year/FWM/OlliePuppet/Esp32/tests/talk/talk.ino"
                                                    );
   i2s_set_adc_mode(ADC_UNIT_1, ADC1_CHANNEL_4 /*pin 32*/);
   i2s_adc_enable(I2S_NUM_0);
}

void setup() {
  Serial.begin(115200);
  delay(100);

  // Initialize the I2S peripheral
  i2sInit();
}


void loop() {
  size_t num_bytes_read;
  int32_t val;
  static float maxsample, avgsample, smoothed;
  static uint32_t oldus;


  i2s_read(I2S_PORT, (char *)samples, DMA_SIZE * sizeof(samples[0]), &num_bytes_read, ( TickType_t ) 0xffffffffUL); // no timeout
  for (uint16_t i = 0; i < BLOCK_SIZE; i++) {
    val = ((~samples[i * 2]) + 1) >> 14; // SPH0645 sends 18bits of 2's complement values 
    avgsample = (avgsample * 20000 + val) / 20001;
    if (abs(val - (int)avgsample) > maxsample)
      maxsample = abs(val - (int)avgsample); // SPH0645 assumes 2 devices (L & R) so we throw out one
    else maxsample = (maxsample * 2000 + abs(val - (int)avgsample)) / 2001;
  }

  if (micros() - oldus > 1000000 / 60) {
    oldus = micros();

    smoothed = (smoothed*2 + maxsample)/3; // low pass to make it less jittery
    Serial.printf("smoothed: %d \t", (int)smoothed);
    // samples range from 50 to 2000
    if (smoothed > 250.0) // noise threshold
    {
      int servo_value = (((int)smoothed)<(250)?(250):(((int)smoothed)>(10000)?(10000):((int)smoothed)));
      servo_value = map(servo_value, 250, 10000, 0, 100);
      Serial.printf("servo_value: %d \n", servo_value);
      mouth.sendServo(servo_value);
      // sendServo(constrain(CLOSED + ((smoothed - 2500.0) / 20000.0), CLOSED, OPEN)); // send 1.0 to 2.0 ms
    }
    else
    {
      mouth.sendServo(0); // close mouth 
    }
  }
}
