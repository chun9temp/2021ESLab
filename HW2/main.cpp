/*
 * Copyright (c) 2014-2020 Arm Limited and affiliates.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include <cstdio>

// Adjust pin name to your board specification.
// You can use LED1/LED2/LED3/LED4 if any is connected to PWM capable pin,
// or use any PWM capable pin, and see generated signal on logical analyzer.
PwmOut led(PWM_OUT);

int main()
{
    // specify period first
    //led.period(0.1f); // 100ms
    //led.period(0.05f); // 50ms
    led.period(0.01f); // 10ms
    //led.period(0.005f); // 5ms

    //led.write(0.01f);
    //led.write(0.2f);
    //led.write(0.4f);      // 50% duty cycle, relative to period
    //led.write(0.6f);
    //led.write(0.8f);
    //led.write(0.99f);
    //led = 0.5f;          // shorthand for led.write()
    //led.pulsewidth(2);   // alternative to led.write, set duty cycle time in seconds
    
    
    float n=0.01;
    float dc=0;
    while (1){
        /*
        if (dc<=1){
            dc=dc+n/100000;
        }
        else {
            dc=0;
        }
        led.write(dc);
        */
        wait_us(20000);
        led.write(dc);
        dc=dc+n;
        if (dc>1||dc<0){
            n=-n;
        }
        
        /*
        led.write(0);
        wait_us(200000);
        led.write(0.1);
        wait_us(200000);
        led.write(0.2);
        wait_us(200000);
        led.write(0.3);
        wait_us(200000);
        led.write(0.4);
        wait_us(200000);
        led.write(0.5);
        wait_us(200000);
        led.write(0.6);
        wait_us(200000);
        led.write(0.7);
        wait_us(200000);
        led.write(0.8);
        wait_us(200000);
        led.write(0.9);
        wait_us(200000);
        led.write(1);
        wait_us(200000);
        */
    }
}
