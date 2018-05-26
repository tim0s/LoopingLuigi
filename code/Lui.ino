#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>



// PINS FOR THE ROTARY ENCODER
#define PIN_A 12
#define PIN_B 11
#define PIN_SW 10

// OLED display TWI address
#define OLED_ADDR   0x3C

// reset pin not used on 4-pin OLED module
Adafruit_SSD1306 display(-1);  // -1 = no reset pin

// 128 x 64 pixel display
#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

volatile int sw_state = 1;
volatile int switch_position = 0;
volatile int switch_pressed = 0;
volatile int switch_released = 0;


void pciSetup(byte pin)
{
    *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
    PCIFR  |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
    PCICR  |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group
}
 
// Use one Routine to handle each group
 
ISR (PCINT0_vect) // handle pin change interrupt for D8 to D13 here
 { 
   noInterrupts();
   int sw_state_new = digitalRead(PIN_SW);
   if (sw_state_new != sw_state) {
     static unsigned long last_interrupt_time = 0;
     unsigned long interrupt_time = millis();
     if ((interrupt_time - last_interrupt_time) < 50) {
       interrupts();
       return;
     } 
     last_interrupt_time = interrupt_time;
     if (sw_state_new == 1) {
       switch_released += 1;
     }
     else {
       switch_pressed += 1;
     }
     sw_state = sw_state_new;
   }
   else {
     if (digitalRead(PIN_A) == HIGH) {
       // low to high on A
       if (digitalRead(PIN_B) == HIGH) switch_position -= 1;
       else switch_position += 1;
     }
     else {
       // high to low on A
       if (digitalRead(PIN_B) == HIGH) switch_position += 1;
       else switch_position += -1;
     }
   }
   interrupts();
 }
 
ISR (PCINT1_vect) // handle pin change interrupt for A0 to A5 here
 {
     // do nothing
 }  
 
ISR (PCINT2_vect) // handle pin change interrupt for D0 to D7 here
 {
     // do nothing
 }  
 
void setup() {  

  Serial.begin(9600);
    
  pinMode(PIN_A, INPUT_PULLUP);
  pinMode(PIN_B, INPUT_PULLUP);
  pinMode(PIN_SW, INPUT_PULLUP);
   
  // enable interrupt for pins
  pciSetup(PIN_A);
  pciSetup(PIN_SW);


  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();
  display.display();

  // display a pixel in each corner of the screen
  display.drawPixel(0, 0, WHITE);
  display.drawPixel(127, 0, WHITE);
  display.drawPixel(0, 63, WHITE);
  display.drawPixel(127, 63, WHITE);

  // display a line of text
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(27,30);
  display.print("Hello, world!");

  // update display with all of the above graphics
  display.display();

}
 
 
void loop() {
  delay(400);
  Serial.print("Switch Position: "); Serial.print(switch_position); Serial.print("\n");
  Serial.print("Switch Presses: "); Serial.print(switch_pressed); Serial.print("\n");
  Serial.print("Switch Released: "); Serial.print(switch_released); Serial.print("\n");
  Serial.print("\n");
}


void draw_menu() {

}

