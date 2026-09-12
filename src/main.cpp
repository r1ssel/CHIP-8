#include <iostream>
#include "chip8.h"

chip8 myChip;

int main() {
    static bool init = true;
    if (init){
        init = false;
        printf("Init...\r\n");
        printf("Init completed\r\n");
    }

    myChip.emulateCycle();
    
    return 0;
}