#pragma once
#ifndef utils_isr_timer_h
    #define utils_isr_timer_h

#ifndef Arduino_h
  #include <Arduino.h>
#endif

#define TIME_1mSeg  1000        //Tiempo para calcular 1000 Interrupciones del Timer = 1mSeg
#define TIME_100mSeg  100000
//#define  TIME_2Seg   1200
#define  TIME_2Seg   20
//#define  TIME_4Seg   2400
#define  TIME_4Seg   40
#define  TIME_2Min   1200

extern bool    flagTimer;
extern bool    flagTelegram;
extern bool    flagButton;


// //bool    flagTimer = false;
// uint16_t countTimer = 0;
extern uint32_t count100mSeg;
extern uint16_t countPing;
extern uint16_t countProg;
extern uint16_t countTelegram;

// hw_timer_t * timer = NULL;
// portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

extern void IRAM_ATTR onTimer();
extern void initISR_Timer(void);

extern portMUX_TYPE timerMux;

#endif