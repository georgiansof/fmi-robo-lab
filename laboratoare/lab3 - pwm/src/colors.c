#include <stdint.h>
#include <math.h>

#define VAL_CONSTRAIN(val, max)  ( (val) > (max) ? (max) : (val))


/*
 * Convert a given HSV (Hue Saturation Value) to RGB (Red Green Blue).
 * - `h` is Hue, integer between 0 and 360;
 * - `s` is Saturation, double between 0 and 1;
 * - `v` is Value (color intensity), double between 0 and 1;
 * - `out_rgb` is an 3-byte array where obtained values will be written to;
 */
void convert_HSV_to_RGB(int h, double s, double v, uint8_t out_rgb[3])
{
    double r = 0, g = 0, b = 0;
    int i = (int)floor(h / 60.0);
    double f = h / 60.0 - i;
    double pv = v * (1 - s);
    double qv = v * (1 - s * f);
    double tv = v * (1 - s * (1 - f));

    switch (i) {
        case 0:
            r = v;
            g = tv;
            b = pv;
            break;
        case 1:
            r = qv;
            g = v;
            b = pv;
            break;
        case 2:
            r = pv;
            g = v;
            b = tv;
            break;
        case 3:
            r = pv;
            g = qv;
            b = v;
            break;
        case 4:
            r = tv;
            g = pv;
            b = v;
            break;
        case 5:
            r = v;
            g = pv;
            b = qv;
            break;
    }

    /* set each component to a integer value between 0 and 255 */
    out_rgb[0] = VAL_CONSTRAIN(255 * r, 255);
    out_rgb[1] = VAL_CONSTRAIN(255 * g, 255);
    out_rgb[2] = VAL_CONSTRAIN(255 * b, 255);
}
