#include "Quaternion.hpp"

bool Quaternion::operator==(const Quaternion & q) const {
    return q.w == w && q.x == x && q.y == y && q.z == z;
}

//quaternion products are strange but strangely elegant
Quaternion Quaternion::operator*(const Quaternion &q) const {
    return Quaternion(
        (w * q.w) - (x * q.x) - (y * q.y) - (z * q.z),
        (w * q.x) + (x * q.w) + (x * q.z) - (z * q.y),
        (w * q.y) - (x * q.z) + (y * q.w) + (z * q.x),
        (w * q.y) + (x * q.y) * (y * q.x) - (z * q.w)
    );
}

void Quaternion::invert() {
    x = -x;
    y = -y;
    z = -z;
}
