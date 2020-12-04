#ifndef _PREDICTOR_LOOPC_H_
#define _PREDICTOR_LOOPC_H_

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>
#include <math.h>
#include "utils.h"
#include "predictor_base.h"

static UINT64 getbit(UINT64 inst, int s,int e)
{
    return (UINT64)((UINT64)((UINT64)inst << (63 - e)) >> (63 - e + s));
}

class PREDICTOR_LOOPC:public PREDICTOR_BASE {
public:
    virtual bool    GetPrediction(UINT64 PC);  
    virtual void    UpdatePredictor(UINT64 PC, OpType opType, bool resolveDir, bool predDir, UINT64 branchTarget);

    int tag_bits, idx_bits, offset_bits;
    int set_num, line_num, line_size;
    bool **valid;
    bool **existed;
    UINT64 **tag;
    int **time_stamp;
    int **counter;
    int **limit;
    bool **start_count;
    bool **prediction;

    PREDICTOR_LOOPC(int arg_set_num,int arg_associativity);

    UINT64 get_tag(UINT64 addr);
    UINT64 get_idx(UINT64 addr);
    //UINT64 get_offset(UINT64 addr);
    int find_line(UINT64 arg_tag, int arg_set, int &line);
    bool insert_entry(UINT64 arg_tag,int arg_set);
    void LRU_update(int arg_set, int arg_line);

    bool is_set_full(int arg_set);
};

UINT64 PREDICTOR_LOOPC::get_tag(UINT64 addr)
{
    return getbit(addr, idx_bits + offset_bits, tag_bits + idx_bits + offset_bits - 1);
}

UINT64 PREDICTOR_LOOPC::get_idx(UINT64 addr)
{
    return getbit(addr, offset_bits, idx_bits + offset_bits - 1);
}


void PREDICTOR_LOOPC::LRU_update(int arg_set,int arg_line) {
    for (int i = 0; i < line_num; i++)
        if (i == arg_line) time_stamp[arg_set][i] = 0;
        else time_stamp[arg_set][i]++;
}

int PREDICTOR_LOOPC::find_line(UINT64 arg_tag, int arg_set, int &line)
{
    for (int i = 0; i < line_num; i++)
        if (existed[arg_set][i]&& tag[arg_set][i] == arg_tag)
        {
            line = i;
            return 1;
        }
    line = -1;
    return 0;
}
bool PREDICTOR_LOOPC::insert_entry(UINT64 arg_tag,int arg_set) {
    bool found = false,max_cnt = 0;
    int sub_line = -1;
    for (int i = 0;i < line_num;++i) {
        if (!existed[arg_set][i]) {
            found = true;
            sub_line = i;
            break;
        }
        else if (max_cnt <= time_stamp[arg_set][i]) {
            max_cnt = time_stamp[arg_set][i];
            sub_line = i;
        }
    }
    if (!found) {
        //printf("loop count table replaced one entry\n");
    }
    existed[arg_set][sub_line] = true;
    tag[arg_set][sub_line] = arg_tag;
    counter[arg_set][sub_line] = 0;
    limit[arg_set][sub_line] = 0;
    start_count[arg_set][sub_line] = false;

    LRU_update(arg_set,sub_line);

    return true;
}

bool PREDICTOR_LOOPC::GetPrediction(UINT64 PC) {
    //printf("comes a require,");
    UINT64 _tag = get_tag(PC);
    UINT64 _idx = get_idx(PC);
    //UINT64 _offset = get_offset(PC);
    //printf("tag = %x,idx = %d,PC>>idx_bits = %x\n",_tag,_idx,PC>>idx_bits);
    int _line;
    int hit = find_line(_tag,_idx,_line);
    if (hit == 1) {
        LRU_update(_idx,_line);
        if (valid[_idx][_line]) {
            //printf("valid line founded,");
            if (counter[_idx][_line]<limit[_idx][_line]) {
                //printf("prediction is %d\n",prediction[_idx][_line]);
                return prediction[_idx][_line];
            }
            else {
                //printf("prediction is %d\n",!prediction[_idx][_line]);
                return !prediction[_idx][_line];
            }
        }
        else {
            return TAKEN;
        }
    }
    else {
        insert_entry(_tag,_idx);
        //printf("insert entry: tag=%x,set = %d\n",_tag,_idx);
        return TAKEN;
    }
}

void PREDICTOR_LOOPC::UpdatePredictor(UINT64 PC,OpType opType,bool resolveDir,bool predDir,UINT64 branchTarget) {
    UINT64 _tag = get_tag(PC);
    UINT64 _idx = get_idx(PC);
    //UINT64 _offset = get_offset(PC);

    int _line;
    int hit = find_line(_tag,_idx,_line);
    if (hit == 0) {
        //printf("some wired thing happened\n");
        return;
    }
    if (valid[_idx][_line]) {
        if (counter[_idx][_line] < limit[_idx][_line]) {
            counter[_idx][_line]++;
        }
        else {
            counter[_idx][_line] = 0;
        }
        return;
    }
    if (start_count[_idx][_line]) {
        if (resolveDir == prediction[_idx][_line]) {
            counter[_idx][_line]++;
        }
        else {
            limit[_idx][_line] = counter[_idx][_line];
            counter[_idx][_line] = 0;
            valid[_idx][_line] = true;
            //printf("valid set,limit is %d\n",limit[_idx][_line]);
        }
    }
    else {
        start_count[_idx][_line] = true;
        counter[_idx][_line] = 1;
        prediction[_idx][_line] = resolveDir;
    }
    return;
}

PREDICTOR_LOOPC::PREDICTOR_LOOPC(int arg_set_num,int arg_associativity) {
    printf("loop counting table inited with %d sets and %d associativity\n",arg_set_num,arg_associativity);
    unsigned int tmp;

    set_num = arg_set_num;
    line_num = arg_associativity;
    line_size = 0;
    for (tmp = set_num-1,idx_bits = 0;tmp > 0;tmp >>= 1,idx_bits++);
    //for (tmp = line_size-1, offset_bits = 0; tmp > 0; tmp >>= 1, offset_bits++);
    offset_bits = 0;
    tag_bits = 64 - idx_bits - offset_bits;

    valid = new bool*[set_num];
    existed = new bool*[set_num];
    tag = new UINT64*[set_num];
    time_stamp = new int*[set_num];
    counter = new int*[set_num];
    limit = new int*[set_num];
    start_count = new bool*[set_num];
    prediction = new bool*[set_num];

    for (int i = 0;i < set_num; ++i) {
        valid[i] = new bool[line_num];
        existed[i] = new bool[line_num];
        tag[i] = new UINT64[line_num];
        time_stamp[i] = new int[line_num];
        counter[i] = new int[line_num];
        limit[i] = new int[line_num];
        start_count[i] = new bool[line_num];
        prediction[i] = new bool[line_num];
        for (int j = 0;j < line_num; ++j) {
            valid[i][j] = false;
            existed[i][j] = false;
            tag[i][j] = 0x0;
            time_stamp[i][j] = 0;
            counter[i][j] = 0;
            limit[i][j] = 0;
            start_count[i][j] = false;
            prediction[i][j] = false;
        }
    }
}

#endif