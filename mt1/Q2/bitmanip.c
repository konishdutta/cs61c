//
// Created by Konish Dutta on 1/21/24.
//
#include <stdio.h>
#include <inttypes.h>

uint8_t rotate(uint8_t num);
uint64_t bit_manip(uint64_t num);
uint8_t turn(uint8_t num);

uint64_t bit_manip(uint64_t num) {
    //grab the last 5 with a mask
    uint64_t res = 0;
    uint64_t mask = 31;
    int i = 0;
    while (num) {
        printf("num = %llu\n", num);
        printf("i = %d\n", i);
        uint8_t curr = mask & num;
        curr = rotate(curr);
        curr = turn(curr);
        uint64_t place = curr;
        place <<= i;
        printf("place = %llu\n", place);
        res |= place;
        i += 5;
        printf("res = %llu\n", res);
        num >>= 5;

    }
    printf("%d\n", res);
    return res;
}

uint8_t rotate(uint8_t num) {
    /*
     * function that rotates three to the right.
     * 0001 0110
     * 0000 1011 rotate once right
     * 0001 0101 rotate twice right
     * 0001 1010 ANSWER: rotate three right
     * 10110
     * 0110101101011010110101101011010110101101011010110101101011010110
     * 11100
     * 1100111001110011100111001110011100111001110011100111001110011100
     */

    /*
     * my approach
     * 0101 1000 t1 move the bits two the left
     * 0000 0010 t2 move the bits three to the right
     * 0101 1010 t1 t2 OR
     * 0001 1010 t1 AND 31 to block the MSD
     */

    uint8_t t1 = num << 2;
    uint8_t t2 = num >> 3;
    uint8_t res = (t1 | t2) & 31;
    return res;
}

uint8_t turn(uint8_t num) {
    /*
     * turns off bit 1
     * turns on bit 2
     */
    num &= ~2;
    num |= 4;
    return num & 31;
}

int main(int argc, char *argv[]) {
    uint64_t n = bit_manip(7735731385749166806);
    printf("%llu\n",  n);
}