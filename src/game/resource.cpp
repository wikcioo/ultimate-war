#include "resource.h"

Resources& Resources::operator+=(const Resources& other)
{
    Wood  += other.Wood;
    Rock  += other.Rock;
    Steel += other.Steel;
    Gold  += other.Gold;
    return *this;
}

Resources& Resources::operator-=(const Resources& other)
{
    Wood  -= other.Wood;
    Rock  -= other.Rock;
    Steel -= other.Steel;
    Gold  -= other.Gold;
    return *this;
}

bool Resources::operator>=(const Resources& other)
{
    return (
        Wood  >= other.Wood &&
        Rock  >= other.Rock &&
        Steel >= other.Steel &&
        Gold  >= other.Gold
    );
}
