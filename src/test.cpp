
#include <string>
#include <fstream>
#include <iostream>
#include <complex>
#include <iomanip>

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "perf.h"
#include "math.h"

using namespace std;

void test_preproc()
{
    const int w = 30, h = 62;
    double* cos = new double[w * h];
    cosWindow(cos, w, h);

    double* f = new double[w * h];
    uint8_t count = 0;
    for (size_t j = 0; j < h; j++) {
        for (size_t i = 0; i < w; i++) {
            f[j * w + i] = count / 255.0;
            count++;
        }
    }

    double* dst = new double[w * h];
    preproc(f, cos, dst, w, h);

    dump2text("test_preproc", dst, w, h);

    delete[] f, cos, dst;
}

void test_dft()
{
    const int w = 30, h = 62;
    double* f = new double[w * h];
    uint8_t count = 0;
    for (size_t j = 0; j < h; j++) {
        for (size_t i = 0; i < w; i++) {
            f[j * w + i] = count / 255.0;
            count++;
        }
    }
    dump2text("test_dft_f", f, w, h);

    double* F = new double[2 * w * h];
    dft2d(w, h, f, F);
    dump2text("test_dft_F", F, w*2, h);

    double* f2 = new double[w * h];
    idft2d(w, h, F, f2);
    dump2text("test_dft_f2", f2, w, h);
}

void test_cvFFT()
{
    int count = 0;
    const size_t w = 600, h = 400;
    double* f = new double[w * h];
    for (size_t y = 0; y < h; y++) {
        for (size_t x = 0; x < w; x++) {
            f[y * w + x] = (count++)%256;
        }
    }
    double* F = new double[2 * w * h];

    cvFFT2d(w, h, f, F);

    Mat matF(Size(w, h), CV_64FC2, F);

    delete[] f, F;
}

int main(int argc, int** argv) 
{
    //test_preproc();
    //test_dft();

    test_cvFFT();

    printf("\ndone\n");
    return 0;
}
