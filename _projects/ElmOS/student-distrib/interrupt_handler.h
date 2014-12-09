#ifndef _INTERRUPT_HANDLER_H
#define _INTERRUPT_HANDLER_H

#include	"x86_desc.h"
#include	"keyboard.h"
#include	"rtc.h"
#include    "scheduler.h"


void keyboard_handler();
void rtc_handler();
void pit_handler();

#endif
