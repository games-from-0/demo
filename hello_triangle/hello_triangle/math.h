#pragma once

#include <stdint.h>

typedef float real32_t;

struct vector3_t
{
    union
    {
        real32_t v[3];
        struct { real32_t x, y, z; };
        struct { real32_t r, g, b; };
    };

    vector3_t(void) = default;
    vector3_t(real32_t x, real32_t y, real32_t z) : x(x), y(y), z(z) {}

    vector3_t operator+(const vector3_t &other) const { return vector3_t(x + other.x, y + other.y, z + other.z); }
    vector3_t operator-(const vector3_t &other) const { return vector3_t(x - other.x, y - other.y, z - other.z); }
    vector3_t operator*(const vector3_t &other) const { return vector3_t(x * other.x, y * other.y, z * other.z); }
    //    vector3_t operator/(const vector3_t &other) { return vector3_t(x / other.x, y / other.y, z / other.z); }
    vector3_t operator*(real32_t scalar) { return vector3_t(x * scalar, y * scalar, z * scalar); }
    vector3_t operator/(real32_t scalar) const { return vector3_t(x / scalar, y / scalar, z / scalar); }
    vector3_t &operator+=(const vector3_t &other) { x += other.x, y += other.y; z += other.z; return *this; }
    vector3_t &operator-=(const vector3_t &other) { x -= other.x, y -= other.y; z -= other.z; return *this; }
    vector3_t &operator*=(const vector3_t &other) { x *= other.x, y *= other.y; z *= other.z; return *this; }
    vector3_t &operator/=(const vector3_t &other) { x /= other.x, y /= other.y; z /= other.z; return *this; }
    vector3_t operator*=(real32_t scalar) { x *= scalar, y *= scalar; z *= scalar; return *this; }
    vector3_t operator/=(real32_t scalar) { x /= scalar, y /= scalar; z /= scalar; return *this; }
};

struct vector4_t
{
    union
    {
        real32_t v[4];
        struct { real32_t x, y, z, w; };
        struct { real32_t r, g, b, a; };
    };

    vector4_t(void) = default;
    vector4_t(real32_t x, real32_t y, real32_t z, real32_t w) : x(x), y(y), z(z), w(w) {}

    vector4_t operator+(const vector4_t &other) { return vector4_t(x + other.x, y + other.y, z + other.z, w + other.w); }
    vector4_t operator-(const vector4_t &other) { return vector4_t(x - other.x, y - other.y, z - other.z, w - other.w); }
    vector4_t operator*(const vector4_t &other) { return vector4_t(x * other.x, y * other.y, z * other.z, w * other.w); }
    //    vector4_t operator/(const vector4_t &other) { return vector4_t(x / other.x, y / other.y, z / other.z, w * other.w); }
    vector4_t operator*(real32_t scalar) { return vector4_t(x * scalar, y * scalar, z * scalar, w * scalar); }
    vector4_t operator/(real32_t scalar) const { return vector4_t(x / scalar, y / scalar, z / scalar, w / scalar); }
    vector4_t &operator+=(const vector4_t &other) { x += other.x, y += other.y; z += other.z; w += other.w; return *this; }
    vector4_t &operator-=(const vector4_t &other) { x -= other.x, y -= other.y; z -= other.z; w -= other.w; return *this; }
    vector4_t &operator*=(const vector4_t &other) { x *= other.x, y *= other.y; z *= other.z; w *= other.w; return *this; }
    vector4_t &operator/=(const vector4_t &other) { x /= other.x, y /= other.y; z /= other.z; w /= other.w; return *this; }
    vector4_t operator*=(real32_t scalar) { x *= scalar, y *= scalar; z *= scalar; w *= scalar; return *this; }
    vector4_t operator/=(real32_t scalar) { x /= scalar, y /= scalar; z /= scalar; w /= scalar; return *this; }
};

inline vector3_t normalize(const vector3_t &v)
{
    real32_t length = (real32_t)sqrt((double)(v.x * v.x + v.y * v.y + v.z * v.z));

    return v / length;
}

inline vector4_t normalize(const vector4_t &v)
{
    real32_t length = (real32_t)sqrt((double)(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w));

    return v / length;
}

inline vector3_t cross(const vector3_t &a, const vector3_t &b)
{
    return vector3_t(a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x);
}

struct matrix4_t
{
    vector4_t col[4];

    matrix4_t(void) = default;
    matrix4_t(const vector4_t col0, const vector4_t col1, const vector4_t col2, const vector4_t col3) : col{ col0, col1, col2, col3 } {}

    matrix4_t operator*(const matrix4_t &right)
    {
        matrix4_t res = {};

        res.col[0] += (col[0] * right.col[0].v[0]) + (col[1] * right.col[0].v[1]) + (col[2] * right.col[0].v[2]) + (col[3] * right.col[0].v[3]);
        res.col[1] += (col[0] * right.col[1].v[0]) + (col[1] * right.col[1].v[1]) + (col[2] * right.col[1].v[2]) + (col[3] * right.col[1].v[3]);
        res.col[2] += (col[0] * right.col[2].v[0]) + (col[1] * right.col[2].v[1]) + (col[2] * right.col[2].v[2]) + (col[3] * right.col[2].v[3]);
        res.col[3] += (col[0] * right.col[3].v[0]) + (col[1] * right.col[3].v[1]) + (col[2] * right.col[3].v[2]) + (col[3] * right.col[3].v[3]);

        return(res);
    }
};

inline matrix4_t identity()
{
    matrix4_t m = {};
    for (uint32_t i = 0; i < 4; ++i)
    {
        m.col[i].v[i] = 1.0f;
    }
    return(m);
}

inline matrix4_t look_at(const vector3_t &position, const vector3_t &target, const vector3_t &up)
{
    vector3_t direction = normalize(position - target);
    vector3_t right = normalize(cross(up, direction));
    vector3_t real_up = normalize(cross(direction, right));

    matrix4_t rot = matrix4_t(vector4_t(right.x, real_up.x, direction.x, 0.0f),
        vector4_t(right.y, real_up.y, direction.y, 0.0f),
        vector4_t(right.z, real_up.z, direction.z, 0.0f),
        vector4_t(0, 0, 0, 1.0f));

    matrix4_t translate = matrix4_t(vector4_t(1, 0, 0, 0.0f),
        vector4_t(0, 1, 0, 0.0f),
        vector4_t(0, 0, 1, 0.0f),
        vector4_t(-position.x, -position.y, -position.z, 1.0f));

    return rot * translate;
}

#undef far
#undef near

inline matrix4_t perspective(real32_t fov, real32_t aspect_ratio, real32_t near, real32_t far)
{
    matrix4_t result = {};

    real32_t tan_half = (real32_t)tan(fov / 2.0f);

    real32_t sum = far + near;
    real32_t sub = far - near;

    result.col[2].v[2] = -sum / sub;
    result.col[2].v[3] = -1.0f;
    result.col[3].v[2] = -(2.0f * far * near) / sub;

    result.col[0].v[0] = 1.0f / (aspect_ratio * tan_half);
    result.col[1].v[1] = 1.0f / tan_half;

    return result;
}

inline matrix4_t m4_rotate(real32_t x_rad, real32_t y_rad, real32_t z_rad)
{
    matrix4_t r_x = identity();
    matrix4_t r_y = identity();
    matrix4_t r_z = identity();

    if (x_rad != 0)
    {
        r_x.col[1].v[1] = (real32_t)cos(x_rad);
        r_x.col[2].v[1] = -(real32_t)sin(x_rad);

        r_x.col[1].v[2] = (real32_t)sin(x_rad);
        r_x.col[2].v[2] = (real32_t)cos(x_rad);
    }
    if (y_rad != 0)
    {
        r_y.col[0].v[0] = (real32_t)cos(y_rad);
        r_y.col[0].v[2] = -(real32_t)sin(y_rad);

        r_y.col[2].v[0] = (real32_t)sin(y_rad);
        r_y.col[2].v[2] = (real32_t)cos(y_rad);
    }
    if (z_rad != 0)
    {
        r_z.col[0].v[0] = (real32_t)cos(z_rad);
        r_z.col[0].v[1] = -(real32_t)sin(z_rad);

        r_z.col[1].v[0] = (real32_t)sin(z_rad);
        r_z.col[1].v[1] = (real32_t)cos(z_rad);
    }

    matrix4_t tot = r_x * r_y;
    tot = tot * r_z;

    return(tot);
}
