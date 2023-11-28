#include <stdint.h>
#include <stdio.h>

#define BIT(x) (1 << x)

static const uint32_t sbox1[64] = {
    14, 4,  13, 1, 2,  15, 11, 8,  3,  10, 6,  12, 5,  9,  0, 7,
    0,  15, 7,  4, 14, 2,  13, 1,  10, 6,  12, 11, 9,  5,  3, 8,
    4,  1,  14, 8, 13, 6,  2,  11, 15, 12, 9,  7,  3,  10, 5, 0,
    15, 12, 8,  2, 4,  9,  1,  7,  5,  11, 3,  14, 10, 0,  6, 13
};
    
int main() {
    int i;
    printf("static const uint32_t s[64] = {\n");
    for (i = 0; i < 64; i++) {
        if ((i & 15) == 0)
            printf("       ");
        printf("%3d%c",
               sbox1[((i & BIT(5)) ? 32 : 0) | (i & BIT(0) ? 16 : 0) | ((i >> 1) & 0xf)],
               (i < 63) ? ',' : ' ');
        if ((i & 15) == 15)
            printf("\n");
    }
    printf("    };\n");
    return (0);
}