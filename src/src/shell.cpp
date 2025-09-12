/*========VEX SHELL PRODUCT - DO NOT EDIT!========*/
// this is a file created to run commands in vsh in "real time"

#include "shell.h"
#include "main.h"

void shell() {
    chassis.moveToPoint(10,20,4000, {.forwards = true});
}
