// instead of writing a bunch of ifdef statements in every file
// redefine hardware timer's behaviour so that when interval timer is 
// called on an STM, it still works
#pragma once

#if defined(TEENSYDUINO)
    #include <IntervalTimer.h>

#elif defined(ARDUINO_ARCH_STM32)
    #include <Arduino.h>
    #include <HardwareTimer.h>

class IntervalTimer{
public:
    bool begin(void (*callback)(), uint32_t periodUs)
    {
        if (timer_ == nullptr)
        {
            TIM_TypeDef* instance = nextFreeTimer();
            if (instance == nullptr)
            {
                return false; // (no more hardware timers)
            }
            timer_ = new HardwareTimer(instance);
        }
        timer_->pause();
        timer_->setOverflow(periodUs, MICROSEC_FORMAT);
        timer_->attachInterrupt(callback);
        timer_->resume();
        return true;
    }
    void end()
    {
        if (timer_ != nullptr) {
            timer_->pause();
        }
    }
private :
    HardwareTimer* timer_ = nullptr;

    static TIM_TypeDef* nextFreeTimer()
    {
        static TIM_TypeDef* const pool[] = { TIM13, TIM14, TIM10, TIM11 };
        static size_t next = 0;
        if (next < sizeof(pool) / sizeof(pool[0])) {
            return pool[next++];
        }
        return nullptr;
    }
};
#else
    #error "No IntervalTimer implementation for this board"
#endif