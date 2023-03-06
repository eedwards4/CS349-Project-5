//
// Created by student on 3/3/23.
//

#ifndef CS349_PROJECT_5_SPACESHIP_H
#define CS349_PROJECT_5_SPACESHIP_H


class Spaceship {
public:
    int value = 0;
    int perimeterWeight = 0;
    char shipClass;

    Spaceship();
    Spaceship(int, char);
    Spaceship(int, int, char);
    void setWeight(int);
};


#endif //CS349_PROJECT_5_SPACESHIP_H
