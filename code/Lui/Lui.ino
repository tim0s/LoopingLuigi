#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

struct louie_state {
  volatile int state = 0;
  volatile int menu_option = 0;
  volatile int mode_num = 0;
  volatile char mode[10];
  volatile int motor_speed = 50;
  volatile int motor_direction = 0;
  volatile int loop = 0;
};

#define start_state 100
#define play_state 101
#define speed_state 102
#define direction_state 103
#define max_num_options 4

// PINS FOR THE ROTARY ENCODER
#define PIN_A 12
#define PIN_B 11
#define PIN_SW 10
#define MOTOR_ENABLE_PIN 5
#define H_BRIDGE_A 6
#define H_BRIDGE_B 7

// OLED display TWI address
#define OLED_ADDR   0x3C

// reset pin not used on 4-pin OLED module
Adafruit_SSD1306 display(-1);  // -1 = no reset pin

// 128 x 64 pixel display
#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

volatile int sw_state = 1;
volatile int switch_position_prev = 0;
volatile int switch_position = 0;
volatile int switch_pressed = 0;
volatile int switch_release_prev = 0;
volatile int switch_released = 0;

//volatile int louie_state = 0;
//volatile int menu_option = 0;
//volatile int louie_speed = 50;
//volatile int louie_direction = 0;

volatile struct louie_state game_state;
volatile char options[max_num_options][10];


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

  pinMode(MOTOR_ENABLE_PIN, OUTPUT);
  pinMode(H_BRIDGE_A, OUTPUT);
  pinMode(H_BRIDGE_B, OUTPUT);
  digitalWrite(MOTOR_ENABLE_PIN, LOW);

  randomSeed(analogRead(0));
   
  // enable interrupt for pins
  pciSetup(PIN_A);
  pciSetup(PIN_SW);


  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  //display.clearDisplay();
  //display.display();

  // display a pixel in each corner of the screen
  //display.drawPixel(0, 0, WHITE);
  //display.drawPixel(127, 0, WHITE);
  //display.drawPixel(0, 63, WHITE);
  //display.drawPixel(127, 63, WHITE);

  // display a line of text
  //display.setTextSize(1);
  //display.setTextColor(WHITE);
  //display.setCursor(27,30);
  //display.print("Hello, world!\nHello, world, again!");

  // update display with all of the above graphics
  //display.display();

  for (int i = 0; i < max_num_options; ++i) {
    sprintf(options[i], "         ");
  }
  game_state.state = start_state;
  game_state.menu_option = 0;
  sprintf(game_state.mode, "         ");
  state_s0(0);

}
 
 
void loop() {
  delay(400);

  int switch_rotation = switch_position - switch_position_prev;
  int button_pressed = switch_released - switch_release_prev;
  
  Serial.print("State: "); Serial.print(game_state.state); Serial.print("\n");
  Serial.print("Mode: "); Serial.print(game_state.mode_num); Serial.print("\n");
  Serial.print("Speed: "); Serial.print(game_state.motor_speed); Serial.print("\n");
  Serial.print("Direction: "); Serial.print(game_state.motor_direction); Serial.print("\n");
  Serial.print("Menu start options: "); Serial.print(game_state.menu_option); Serial.print("\n");
  Serial.print("Switch Position (previous)(rotation): ");
  Serial.print(switch_position);
  Serial.print(" ("); Serial.print(switch_position_prev); Serial.print(") ");
  Serial.print(" ("); Serial.print(switch_rotation); Serial.print(") ");
  Serial.print("\n");
  Serial.print("Switch Presses: "); Serial.print(switch_pressed); Serial.print("\n");
  Serial.print("Switch Released: "); Serial.print(switch_released); Serial.print("\n");
  Serial.print("\n");

  switch (game_state.state) {
    case start_state:
      if (button_pressed > 0) {
        game_state.motor_speed = 50;
        game_state.motor_direction = 0;
        if (game_state.menu_option == 0) {
          game_state.state = 0;
          game_state.mode_num = 0;
          sprintf(game_state.mode, "Fixed");
          game_state.menu_option = 0;
          state_o0(0);
        } else if (game_state.menu_option == 1) {
          game_state.state = 1;
          game_state.mode_num = 1;
          sprintf(game_state.mode, "For Pro's");
          game_state.menu_option = 0;
          state_o1(0);
        } else if (game_state.menu_option == 2) {
          game_state.state = 2;
          game_state.mode_num = 2;
          sprintf(game_state.mode, ":ohdeer:");
          game_state.menu_option = 0;
          state_o2(0);
        }
        break;
      }
      if (switch_rotation > 0) {
        state_s0(--game_state.menu_option);
        break;
      }
      if (switch_rotation < 0){
        state_s0(++game_state.menu_option);
        break;
      }
      break;
    case 0:
      if (button_pressed > 0) {
        if (game_state.menu_option == 0) {
          game_state.state = play_state;
          game_state.menu_option = 0;
          state_p0();
          break;
        }
        if (game_state.menu_option == 1) {
          game_state.state = start_state;
          game_state.menu_option = 0;
          state_s0(0);
          break;
        }
        if (game_state.menu_option == 2) {
          game_state.state = speed_state;
          game_state.menu_option = 0;
          state_speed();
          break;
        }
        if (game_state.menu_option == 3) {
          game_state.state = direction_state;
          game_state.menu_option = 0;
          state_direction();
          break;
        }
      }
      if (switch_rotation > 0) {
        state_o0(--game_state.menu_option);
        break;
      }
      if (switch_rotation < 0){
        state_o0(++game_state.menu_option);
        break;
      }
      break;
    case 1:
      if (button_pressed > 0) {
        if (game_state.menu_option == 0) {
          game_state.state = play_state;
          game_state.menu_option = 0;
          state_p0();
          break;
        }
        if (game_state.menu_option == 1) {
          game_state.state = start_state;
          game_state.menu_option = 0;
          state_s0(0);
          break;
        }
      }
      if (switch_rotation > 0) {
        state_o1(--game_state.menu_option);
        break;
      }
      if (switch_rotation < 0){
        state_o1(++game_state.menu_option);
        break;
      }
      break;
    case 2:
      if (button_pressed > 0) {
        if (game_state.menu_option == 0) {
          game_state.state = play_state;
          game_state.menu_option = 0;
          state_p0();
          break;
        }
        if (game_state.menu_option == 1) {
          game_state.state = start_state;
          game_state.menu_option = 0;
          state_s0(0);
          break;
        }
      }
      if (switch_rotation > 0) {
        state_o2(--game_state.menu_option);
        break;
      }
      if (switch_rotation < 0){
        state_o2(++game_state.menu_option);
        break;
      }
      break;
    case play_state:
      game_state.loop++;
      if (button_pressed > 0) {
          game_state.state = game_state.mode;
          game_state.menu_option = 0;
          stop_playing();
          break;
      }
      state_p0();
      break;
    case speed_state:
      if (button_pressed > 0) {
        game_state.state = 0;
        game_state.menu_option = 0;
        state_o0(0);
        break;
      }
      game_state.motor_speed -= switch_rotation;
      if (game_state.motor_speed < 0) game_state.motor_speed = 0;
      if (game_state.motor_speed > 100) game_state.motor_speed = 100;
      state_speed();
      break;
    case direction_state:
      if (button_pressed > 0) {
        game_state.state = 0;
        game_state.menu_option = 0;
        state_o0(0);
        break;
      }
      if (switch_rotation != 0) {
        if (game_state.motor_direction == 0) {
          game_state.motor_direction = 1;
        } else {
          game_state.motor_direction = 0;
        }
      }
      state_direction();
      break;
    default:
      break;
  }

  switch_position_prev = switch_position;
  switch_release_prev = switch_released;
}

void stop_playing() {
  set_motor_speed_and_direction(0, 0);
  game_state.loop = 0;
  switch(game_state.mode_num) {
    case 0:
      game_state.state = 0;
      state_o0(0);
      break;
    case 1:
      game_state.state = 1;
      state_o1(0);
      break;
    case 2:
      game_state.state = 2;
      state_o2(0);
      break;
    default:
      game_state.state = start_state;
      state_s0(0);
      break;
  }
}

void state_s0(int start_option) {
  
  for (int i = 0; i < max_num_options; ++i) {
    sprintf(options[i], "         ");
  }
  sprintf(options[0], "Fixed");
  sprintf(options[1], "For Pro's");
  sprintf(options[2], ":ohdeer:");

  if (start_option < 0) {
    game_state.menu_option = 0;
  } else if (start_option >= 3) {
    game_state.menu_option = 2;
  }
  
  draw_menu("MENU", options, 3, game_state.menu_option);
}

void state_o0(int start_option) {

  for (int i = 0; i < max_num_options; ++i) {
    sprintf(options[i], "         ");
  }
  sprintf(options[0], "Play");
  sprintf(options[1], "Back");
  sprintf(options[2], "Speed %3d", game_state.motor_speed);
  if (game_state.motor_direction == 0) {
    sprintf(options[3], "Rotat  CW");
  } else {
    sprintf(options[3], "Rotat CCW");
  }

  if (start_option < 0) {
    game_state.menu_option = 0;
  } else if (start_option >= 4) {
    game_state.menu_option = 3;
  }
  
  draw_menu("Fixed", options, 4, game_state.menu_option);
}

void state_o1(int start_option) {

  for (int i = 0; i < max_num_options; ++i) {
    sprintf(options[i], "         ");
  }
  sprintf(options[0], "Play");
  sprintf(options[1], "Back");

  if (start_option < 0) {
    game_state.menu_option = 0;
  } else if (start_option >= 2) {
    game_state.menu_option = 1;
  }
  
  draw_menu("For Pro's", options, 2, game_state.menu_option);
}

void state_o2(int start_option) {

  for (int i = 0; i < max_num_options; ++i) {
    sprintf(options[i], "         ");
  }
  sprintf(options[0], "Play");
  sprintf(options[1], "Back");

  if (start_option < 0) {
    game_state.menu_option = 0;
  } else if (start_option >= 2) {
    game_state.menu_option = 1;
  }
  
  draw_menu(":ohdeer:", options, 2, game_state.menu_option);
}

void state_p0() {

  int change_direction = 0;

  if (game_state.loop % 5 == 0) {
    if (game_state.mode_num == 1) {
      game_state.motor_speed += random(-10, 10);
      if (game_state.motor_speed < 0) game_state.motor_speed = 0;
      if (game_state.motor_speed > 100) game_state.motor_speed = 100;
      change_direction = random(0, 99);
      if (change_direction < 5) {
        if (game_state.motor_direction == 0) {
          game_state.motor_direction = 1;
        } else {
          game_state.motor_direction = 0;
        }
      }
    } else if (game_state.mode_num == 2) {
      game_state.motor_speed += random(-10, 10) * 3;
      if (game_state.motor_speed < 0) game_state.motor_speed = 0;
      if (game_state.motor_speed > 100) game_state.motor_speed = 100;
      change_direction = random(0, 99);
      if (change_direction < 25) {
        if (game_state.motor_direction == 0) {
          game_state.motor_direction = 1;
        } else {
          game_state.motor_direction = 0;
        }
      }
    }
  }
  
  set_motor_speed_and_direction(game_state.motor_speed, game_state.motor_direction);

  for (int i = 0; i < max_num_options; ++i) {
    sprintf(options[i], "         ");
  }
  sprintf(options[0], "Stop");

  game_state.menu_option = 0;
  
  draw_menu(game_state.mode, options, 1, game_state.menu_option);
}

void state_speed() {
  for (int i = 0; i < max_num_options; ++i) {
    sprintf(options[i], "         ");
  }
  sprintf(options[0], "Speed %3d", game_state.motor_speed);
  game_state.menu_option = 0;
  draw_menu("Speed", options, 1, game_state.menu_option);
}

void state_direction() {
  for (int i = 0; i < max_num_options; ++i) {
    sprintf(options[i], "         ");
  }
  if (game_state.motor_direction == 0) {
    sprintf(options[0], "Rotat  CW");
  } else {
    sprintf(options[0], "Rotat CCW");
  }
  game_state.menu_option = 0;
  draw_menu("Rotation", options, 1, game_state.menu_option);
}

void draw_menu(const char* title, const char options[][10], int num_options, int start_option) {

  // Initialize display
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);

  // Print title
  display.setCursor(0, 0);
  display.print(title);

  if (start_option < 0) {
    start_option = 0;
  } else if (start_option >= num_options) {
    start_option = num_options - 1;
  }

  // Print first option
  for (int i = 2; i < 11; ++i)
    for (int j = 18; j < 27; ++j)
      display.drawPixel(i, j, WHITE);
  display.setCursor(16, 16);
  display.print(options[start_option]);

  // Print other options
  int row = 32;
  int next_option = start_option + 1;
  while (next_option < num_options && row < 64) {
    display.setCursor(16, row);
    display.print(options[next_option]);
    row += 16;
    next_option++;
  }

  display.display();
  
}

void set_motor_speed_and_direction(int spd, int dir) {

  static int motor_speed = -1;
  static int motor_direction = -1;

  if (motor_speed == spd && motor_direction == dir) {
    return;
  }

  int half_speed = motor_speed + (spd - motor_speed) / 2;

  if (motor_direction != dir) {
    analogWrite(MOTOR_ENABLE_PIN, motor_speed * 0.65);
    delay(200);
    digitalWrite(MOTOR_ENABLE_PIN, LOW);
    delay(200);
    if (dir == 0) {
      // NEVER SET BOTH TO THE SAME VALUE WHILE MOTOR IS ON!
      digitalWrite(H_BRIDGE_A, LOW);
      digitalWrite(H_BRIDGE_B, HIGH);
    }
    else {
      // NEVER SET BOTH TO THE SAME VALUE WHILE MOTOR IS ON!
      digitalWrite(H_BRIDGE_A, HIGH);
      digitalWrite(H_BRIDGE_B, LOW);
    }
    half_speed = spd / 2;
  }

  int intermediate = half_speed * 0.65;

  motor_speed = spd;
  motor_direction = dir;
  
  // spd should be between 0 and 100
  // dir can be zero or one
  spd =  spd * 1.275;
  if (spd > 127) spd = 127;
  if (spd < 0) spd = 0;
  
  analogWrite(MOTOR_ENABLE_PIN, intermediate);
  delay(200);
  analogWrite(MOTOR_ENABLE_PIN, spd);
  
}

