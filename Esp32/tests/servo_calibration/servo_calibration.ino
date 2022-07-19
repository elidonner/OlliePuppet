// We'll track the volume level in this variable.
float servo_pulse = 1.0;


/**** SERVOSTUFF *****/
#define SERVOPIN 25
// 40KHz > freq*resolution > 40MHz
#define LEDC_RES_BITS 12        // cannot be higher than 14 bits
#define LEDC_RES ((1<<LEDC_RES_BITS)-1)
#define LEDC_FREQ_HZ 60
#define CHANNEL0 0

void servoSetup(int pin) {
  ledcSetup(CHANNEL0, LEDC_FREQ_HZ, LEDC_RES_BITS); // channel, freq, bits
  ledcAttachPin(pin, CHANNEL0);
}

void sendServo(float pulse_ms) {  // 1.0ms = 90o left, 2.0ms = 90o right, 1.5ms center
  ledcWrite(CHANNEL0, (pulse_ms / 1000.0)*LEDC_FREQ_HZ * LEDC_RES);
}

void setup() {
  Serial.begin(115200);
  servoSetup(SERVOPIN);
}

// Read in + and - characters to set the volume.
void loop() {
  if (! Serial.available()) return;
  
  // read a character from serial console
  char c = Serial.read();
  
  // increase
  if (c == '+') {
    servo_pulse+=0.01;
  }
  // decrease
  else if (c == '-') {
    servo_pulse-=0.01;
  }
  // else if(c == '<'){
  //   c = Serial.read();
  //   char string_pulse[3];
  //   int i =0;
  //   while(i < '2'){
  //     string_pulse[i] = c;
  //     c = Serial.read();
  //     i++;
  //   }
  //   string_pulse[3] = "\0";
  //   int ans = atoi(string_pulse);
  //   servo_pulse = 1.00+ ans/100.0;

  // }
  // ignore anything else
  else 
    return;
    
  servo_pulse = constrain(servo_pulse,.8,2.0);

  Serial.println(servo_pulse);

  sendServo(servo_pulse);
}