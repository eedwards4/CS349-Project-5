//
// Created by Joe Coon and Ethan Edwards on 3/3/23.
//

#include "Spaceship.h"

Spaceship::Spaceship(int newValue, char newShipClass){
    value = newValue;
    shipClass = newShipClass;
}

void Spaceship::setWeight(int weight) {
    perimeterDist = weight;
}
