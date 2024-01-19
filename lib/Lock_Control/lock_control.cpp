#include <lock_control.h>
#include <Arduino.h>

bool is_device_locked(void)
{
    bool status;
    status = digitalRead(status_pin);
    if(status)
    {
        return false;
    }
    else
        return true;
}

void unlock_controller(void)
{
    //send high for 500ms to unlock
    // digitalWrite(lock_pin,1);
    // delay(500);
    digitalWrite(lock_pin,0);

}