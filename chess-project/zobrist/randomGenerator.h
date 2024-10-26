#include "../global.h"

#define n 312

typedef struct MtState
{
    Uint64 state_array[n];         // the array for the state vector 
    int state_index;                 // index into state vector array, 0 <= state_index <= n-1   always
} MtState;

void initializeState(MtState* state, Uint64 seed);
Uint64 randomUint64(MtState* state);