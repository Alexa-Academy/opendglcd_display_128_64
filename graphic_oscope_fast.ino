#include "openGLCD_AA.h"
#include <TimeLib.h>
#include <Encoder.h>

uint8_t a_sin[GLCD_AA.Width];

int idx;

#define START_TIME 0.00005

// Encoder
Encoder knobTime(3, 2);
long lastEncoderPosCount = 0;

const float cicli_secondo=15625;
float intervallo_sec = START_TIME; // intervallo
int cicli_necessari=((int)(cicli_secondo*intervallo_sec));
uint16_t timer_deadline = 65535 - cicli_necessari;

bool triggered=false;

int offset_y = 0;

int trigger_y = 32;

unsigned long trigTime;

#define TRIG_AUTO_TIME 100

bool showTrigger=true;

char buf[100] = {0};

uint8_t buffer[GLCD_AA.Width * (GLCD_AA.Height / 8) + 2] = {0};

void clearBuffer(uint8_t* b) {
  for (int i=0; i<GLCD_AA.Width * (GLCD_AA.Height / 8) + 2; ++i) {
    b[i] = 0;
  }
}

ISR(TIMER1_OVF_vect) {                           // Interrupt service routine
  if (idx < GLCD_AA.Width) {
    int val = analogRead(A5);
    int calc_val = ((1024-val) / 1024.0 * GLCD_AA.Height);
    if (triggered) {
      a_sin[idx++] = calc_val;
    } else {
      if (calc_val < trigger_y) {
        triggered=true;
      } else {
        if (millis() - trigTime > TRIG_AUTO_TIME) {
          triggered=true;
        }
      }
    }
  } else {
    clearBuffer(buffer);
    for (uint8_t x = 0; x < GLCD_AA.Width-1; ++x) {
      if (a_sin[x]-offset_y >= 0 && a_sin[x]-offset_y <=64 && a_sin[x+1]-offset_y >= 0 && a_sin[x+1]-offset_y <= 64)
        GLCD_AA.drawLine(buffer, x, a_sin[x]-offset_y, x+1, a_sin[x+1]-offset_y);
    }

    GLCD_AA.drawText(buffer, 4, 4, buf);

    if (showTrigger)
      GLCD_AA.drawHLine(buffer, 0, trigger_y-offset_y, GLCD_AA.Width, 1);

    GLCD_AA.drawBitmap(buffer, 0, 0, BLACK);

    idx = 0;
    triggered=false;
    trigTime = millis();
  }

  TCNT1 = timer_deadline;     // Reinit del timer
}

void setup() {
  Serial.begin(9600);

  knobTime.write(lastEncoderPosCount);

  trigTime = millis();

  TCCR1A = 0;                        //Abilita il timer 0, disattiva le modalita' di controllo basate su pin esterni
  TCCR1B = 0;                        //
  TCCR1B |= (1<<CS10)|(1 << CS12);   //Configura il prescale ponendo a 1 i soli bit CS10 e CS12 (vedi tabella sopra)
  TCNT1 = timer_deadline;            //Imposta il valore calcolato in precedenza                            
   
  TIMSK1 |= (1 << TOIE1);           // Abilita il timer 1 usando il registro TIMSK1

  GLCD_AA.Init();
  GLCD_AA.ClearScreen();

  idx = 0;

  changeVal();
}

void changeVal() {
  long encoderPosCount = knobTime.read();
    
  intervallo_sec = START_TIME + encoderPosCount*0.0000025;
  if (intervallo_sec < 0.000005) {
    intervallo_sec = 0.000005;
    knobTime.write(lastEncoderPosCount);
  } else {
    lastEncoderPosCount = encoderPosCount;
  }

  dtostrf(intervallo_sec*128.0, 4, 6, buf);

  cicli_necessari=((int)(cicli_secondo*intervallo_sec));
  timer_deadline = 65535 - cicli_necessari;
}

void loop() {
  changeVal();
}
