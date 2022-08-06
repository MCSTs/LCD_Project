#include "interface.h"


int main(int argc, char *argv[])
{
    int *p = lcdInit();
    InterfaceSelect(p, "MainInterface.bmp", 0, 0);

    system("/IOT/iot");

    return 0;
}
