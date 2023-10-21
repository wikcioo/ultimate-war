#pragma once

struct Resources
{
    int Wood;
    int Rock;
    int Steel;
    int Gold;

    Resources& operator+=(const Resources& other);
    Resources& operator-=(const Resources& other);
    bool operator>=(const Resources& other);
};
