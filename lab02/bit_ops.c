#include <stdio.h>
#include "bit_ops.h"

// Return the nth bit of x.
// Assume 0 <= n <= 31
unsigned get_bit(unsigned x,
                 unsigned n) {
    // YOUR CODE HERE
    // Returning -1 is a placeholder (it makes
    // no sense, because get_bit only returns 
    // 0 or 1)
    x = x >> n;
    return x & 1;
}
// Set the nth bit of the value of x to v.
// Assume 0 <= n <= 31, and v is 0 or 1
void set_bit(unsigned * x,
             unsigned n,
             unsigned v) {
    // YOUR CODE HERE
    unsigned clear_mask = ~(1 << n);
    unsigned check_mask = v << n;
    *x = (*x & clear_mask);
    *x = (*x | check_mask);

    /*
    1
    0
    xor v 0

    0010
    0010
    1101
    0101
    0000
    1011
    0001


    0101
    0010
    0111

     case 1 string 1:
    0010 0000 0000 and mask
     or
     now flip
    1101 1111 1111


     case 0 string 1:
    0010 0000 0000
     xor
    0000 0000 0000

     string 2:
    0010 0000 0000

     now string1 = string 2 or string 1
    0010 0000 0000

    0010 0100 1110

    0100 1110
    generate a string1 that is v and push it n spaces
    0000 0000

    make another string2 that's a 1 and push it n spaces
    0000 0100
    now flip it
    1111 1011

    now string1 = string 2 or string 1
    1111 1011

    string 1 and original string

    0100 1010
    x = 0 ~x = 1 v = 1 ~v = 0 x = 1; (x and v) or (~x and v)
    x = 1 ~x = 0 v = 1 ~v = 0 x = 1; (x and v) or (~x and v)
    x = 1 ~x = 0 v = 0 ~v = 1 x = 0; x and v and (~x ^ v)
    x = 0 ~x = 1 v = 0 ~v = 1 x = 0; x and v
    */


}
// Flip the nth bit of the value of x.
// Assume 0 <= n <= 31
void flip_bit(unsigned * x,
              unsigned n) {
    // YOUR CODE HERE
    unsigned mask = 1 << n;
    *x = mask ^ *x;


    /*
    1 1 --> xor
    0 1
    1000
    1000
    0111
    or
    1111

    0000

    1010
    1000
    0101
    1101
    0010

    0101
    1000
    1010
    0010

    */


}