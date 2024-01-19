#ifndef LOCK_CONTROL_H
#define LOCK_CONTROL_H

const int status_pin = 4;
const int lock_pin   = 2;

bool is_device_locked(void);
void unlock_controller(void);
#endif