//#pragma once
//#include <Arduino.h>
#ifndef utils_isr_timer_cpp
    #define utils_isr_timer_cpp
#endif
//#ifndef utils_isr_timer_h
    #include <utils_isr_timer.h>
//#endif


// #ifndef flagTimer
bool flagTimer = false;
bool flagTelegram  = false;
bool flagButton  = false;
// #endif

//bool    flagTimer = false;


//bool    flagTimer = false;
//uint32_t countTimer = 0;
uint32_t count100mSeg = 0;
uint16_t countPing = 0;
uint16_t countProg = 0;
uint16_t countTelegram = 0;

hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;


void IRAM_ATTR onTimer() {
    portENTER_CRITICAL_ISR(&timerMux);
    flagTimer = true;
    flagTelegram = true;
    flagButton = true;
    portEXIT_CRITICAL_ISR(&timerMux);
}

void initISR_Timer(void){
    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &onTimer, true);
    timerAlarmWrite(timer, 100000, true);
    timerAlarmEnable(timer);

}

void statusLED(void){

}