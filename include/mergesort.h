/*
 * mergesort.h - Header File
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>

typedef struct grayPixel* grayPixel_t;
typedef struct grayPixel grayPixelData;
struct grayPixel {
    int index;
    int intensity;
};

int merge(grayPixel_t A, int lo1, int hi1, int lo2, int hi2);
int mergesort(grayPixel_t A, int lo, int hi);
