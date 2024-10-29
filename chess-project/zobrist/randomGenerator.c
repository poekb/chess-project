#include "randomGenerator.h"
#include <stdint.h>

// Mersenne Twister:
// https://en.wikipedia.org/wiki/Mersenne_Twister#cite_note-3

#define w 64
#define m 156
#define r 31
#define UMASK (0xffffffffffffffffUL << r)
#define LMASK (0xffffffffffffffffUL >> (w-r))
#define a 0xB5026F5AA96619E9UL
#define u 29
#define s 17
#define t 37
#define l 43
#define b 0x71D67FFFEDA60000UL
#define c 0xFFF7EEE000000000UL
#define f 6364136223846793005UL



void initializeState(MtState* state, Uint64 seed)
{
    Uint64* state_array = &(state->state_array[0]);

    state_array[0] = seed;                          // suggested initial seed = 19650218UL

    for (int i = 1; i < n; i++)
    {
        seed = f * (seed ^ (seed >> (w - 2))) + i;    // Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier.
        state_array[i] = seed;
    }

    state->state_index = 0;
}


Uint64 randomUint64(MtState* state)
{
    Uint64* state_array = &(state->state_array[0]);

    int k = state->state_index;      // point to current state location

    int j = k - (n - 1);             // point to state n-1 iterations before
    if (j < 0) j += n;               // modulo n circular indexing

    Uint64 x = (state_array[k] & UMASK) | (state_array[j] & LMASK);

    Uint64 xA = x >> 1;
    if (x & 0x0000000000000001UL) xA ^= a;

    j = k - (n - m);                 // point to state n-m iterations before
    if (j < 0) j += n;               // modulo n circular indexing

    x = state_array[j] ^ xA;         // compute next value in the state
    state_array[k++] = x;            // update new state value

    if (k >= n) k = 0;               // modulo n circular indexing
    state->state_index = k;

    Uint64 y = x ^ (x >> u);         // tempering 
    y = y ^ ((y << s) & b);
    y = y ^ ((y << t) & c);
    Uint64 z = y ^ (y >> l);

    return z;
}