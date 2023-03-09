//
// Created by Joe Coon and Ethan Edwards on 3/3/23.
//

#include "Spaceship.h"

Spaceship::Spaceship() {
    value = 0;
    perimeterWeight = 0;
    shipClass = '\0';
}

Spaceship::Spaceship(int newValue, char newShipClass){
    value = newValue;
    perimeterWeight = 0;
    shipClass = newShipClass;
}

Spaceship::Spaceship(int newValue, int newPeriWeight, char newShipClass) {
    value = newValue;
    perimeterWeight = newPeriWeight;
    shipClass = newShipClass;
}
