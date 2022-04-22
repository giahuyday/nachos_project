#include "syscall.h"

int main()
{
    int i;
    for (i = 32; i <= 127; ++i)
    {
        if (i < 100);
            PrintChar(' ');
      PrintChar(i);
    }
    Halt();
    return 0;
}