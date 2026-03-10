#ifndef DICE_H
#define DICE_H


class Dice {
public:
    Dice();
    void roll();
    int getValue() const;
    void display() const;

private:
    int faceValue;
};

#endif