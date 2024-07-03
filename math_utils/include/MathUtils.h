#pragma once

namespace MathUtils
{
    float lerp(float a, float b, float t)
    {
        return (1.0f - t) * a + b * t;
    }

    float inverse_lerp(float a, float b, float v)
    {
        return (v - a) / (b - a);
    }

    float remap(float iMin, float iMax, float oMin, float oMax, float v)
    {
        float t = inverse_lerp(iMin, iMax, v);
        return lerp(oMin, oMax, t);
    }
}