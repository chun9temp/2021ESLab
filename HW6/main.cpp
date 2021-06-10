#include "mbed.h"


InterruptIn button(BUTTON1);
DigitalOut led(LED1);
Timeout  press_threhold;
// EventQueue queue(32*EVENTS_EVENT_SIZE);
// Thread t;
int long_press_count = 0;

void button_release_detecting()
{
    button.enable_irq();
}



void button_pressed()
{
    button.disable_irq();
    
    press_threhold.attach(&button_release_detecting, 1s);
}

void button_released()
{
    led=!led;
    ++long_press_count;
    // queue.call(printf,"Long pressed %d time(s).\n",long_press_count);
    printf("Long pressed %d time(s).\n",long_press_count);
}


// main() runs in its own thread in the OS
int main()
{
    // t.start(callback(&queue,&EventQueue::dispatch_forever));
    EventQueue *queue = mbed_event_queue();
    printf("Starting in context %p\r\n", ThisThread::get_id());
    button.fall(&button_pressed);
    // button.rise(&button_released);
    // button.rise(queue.event(&button_released));
    button.rise(queue->event(&button_released));
    queue->dispatch_forever();
    // while (1);
}

