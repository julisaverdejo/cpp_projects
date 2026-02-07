#include "dot_product.h"

void dot_product(data_t x_rom[N], data_t y_rom[N], data_t &result) {

    data_t sum = 0;
    for (int i = 0; i < N; i++) {
        sum += x_rom[i] * y_rom[i];
    }

    result = sum;
}
