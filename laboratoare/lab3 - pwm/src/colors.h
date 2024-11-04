#ifndef COLORS_H_
#define COLORS_H_

#include <stdint.h>

/*
 * Convert a given HSV (Hue Saturation Value) to RGB (Red Green Blue).
 * - `h` is Hue, integer between 0 and 360;
 * - `s` is Saturation, double between 0 and 1;
 * - `v` is Value (color intensity), double between 0 and 1;
 * - `out_rgb` is an 3-byte array where obtained values will be written to;
 */
void convert_HSV_to_RGB(int h, double s, double v, uint8_t out_rgb[3]);

#endif // COLORS_H_

