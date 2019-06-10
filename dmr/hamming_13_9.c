#include "hamming_13_9.h"

/*
generator matrix accodring to etsi B.3.4
1 0 0 0 0 0 0 0 0 1 1 1 1
0 1 0 0 0 0 0 0 0 1 1 1 0
0 0 1 0 0 0 0 0 0 0 1 1 1
0 0 0 1 0 0 0 0 0 1 0 1 0
0 0 0 0 1 0 0 0 0 0 1 0 1
0 0 0 0 0 1 0 0 0 1 0 1 1
0 0 0 0 0 0 1 0 0 1 1 0 0
0 0 0 0 0 0 0 1 0 0 1 1 0
0 0 0 0 0 0 0 0 1 0 0 1 1

parity check matrix according to H = [-PT|In-k]
1 1 0 1 0 1 1 0 0 1 0 0 0
1 1 1 0 1 0 1 1 0 0 1 0 0
1 1 1 1 0 1 0 1 1 0 0 1 0
1 0 1 0 1 1 0 0 1 0 0 0 1
*/

// parity check matrix as code
uint16_t hamming_13_9_parity_check_matrix[] = {
    0b1101011001000,
    0b1110101100100,
    0b1111010110010,
    0b1010110010001
};

struct correction {
    uint16_t syndrome;
    uint16_t error_pattern;
};

// generated by applying possible combinations of errors
static struct correction corrections[] = {
    { 1, 1 },
    { 2, 2 },
    { 4, 4 },
    { 8, 8 },
    { 3, 16 },
    { 6, 32 },
    { 12, 64 },
    { 11, 128 },
    { 5, 256 },
    { 10, 512 },
    { 7, 1024 },
    { 14, 2048 },
    { 15, 4096 }
};

uint16_t hamming_13_9_parity(uint16_t* data) {
    uint16_t parity = 0;

    uint8_t k;
    for (k = 0; k < 4; k++) {
        uint8_t bit = 0, l;
        for (l = 0; l < 13; l++) {
            if ((hamming_13_9_parity_check_matrix[k] >> l) & 1) {
                bit ^= ((*data >> l) & 1);
            }
        }

        parity = (parity << 1) | (bit & 1);
    }

    return parity;
}

bool hamming_13_9(uint16_t* data) {
    uint16_t parity = hamming_13_9_parity(data);

    if (parity == 0) return true;

    int num_corrections = sizeof(corrections)/sizeof(corrections[0]);
    for (int i = 0; i < num_corrections; i++) {
        if (corrections[i].syndrome == parity) {
            *data = *data ^ corrections[i].error_pattern;
            return true;
        }
    }

    return false;
}
