#include "../global.h"

#define transpositionCount 312

typedef struct MtState
{
    Uint64 state_array[transpositionCount];         // the array for the state vector 
    int state_index;                 // index into state vector array, 0 <= state_index <= transpositionCount-1   always
} MtState;

void InitializeState(MtState* state, Uint64 seed);
Uint64 RandomUint64(MtState* state);