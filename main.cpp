//=====[Libraries]=============================================================

#include "smart_food_system.h"

//=====[Main function, the program entry point after power on or reset]========

int main()
{
    smartFoodSystemInit();
    while (true) {
        smartFoodSystemUpdate();
    }
}