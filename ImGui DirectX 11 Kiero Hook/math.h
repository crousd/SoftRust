#pragma once
struct Vector3 {
    float x, y, z;
    Vector3 operator+( Vector3 a ) {
        return { this->x + a.x,this->y + a.y,this->z + a.z };
    }
};