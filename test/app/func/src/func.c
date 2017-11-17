#include "func.h"
#include <stdio.h>

void func()
{
    printf("func() was called\n");
#ifndef default_message
#error No default message: force include is not working
#endif
    printf("Message: %s\n", default_message);
}
