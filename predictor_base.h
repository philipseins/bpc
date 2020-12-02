#ifndef _PREDICTOR_BASE_H_
#define _PREDICTOR_BASE_H_

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>
#include <math.h>
#include "utils.h"

class PREDICTOR_BASE {
public:
    PREDICTOR_BASE(){

    }
    virtual bool    GetPrediction(UINT64 PC) = 0;  
    virtual void    UpdatePredictor(UINT64 PC, OpType opType, bool resolveDir, bool predDir, UINT64 branchTarget) = 0;
    ~PREDICTOR_BASE() {
        
    }
};

#endif