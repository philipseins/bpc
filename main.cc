///////////////////////////////////////////////////////////////////////
//  Copyright 2015 Samsung Austin Semiconductor, LLC.                //
///////////////////////////////////////////////////////////////////////

//Description : Main file for CBP2016 

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <queue>
using namespace std;

#include "utils.h"
//#include "bt9.h"
#include "bt9_reader.h"
//#include "predictor.cc"
#include "predictor_base.h"
#include "predictor_gshare.h"

#define COUNTER     unsigned long long
// Max Branch Instructions in a single fetch
#define BRANCH_MAX  4
// Max Delay Cycles to update the predictor
#define DELAY_MAX 3
//Modified by Hujiuxin
//Max predicted_queue size
#define MAX_PREQUE_SIZE 100

/* Simulate Mode
 * Mode 0: predict new branch inst until all previous branch inst updates the predictor
 * Mode 1: follow the cycle
*/
int mode = 0;
// 0 for original trace, 1 for generated traces.
int trace_kind = 1;
bool Rand = false;
UINT64 total_instruction_counter;


class PredictedEntry {
  public:
    UINT64 PC;
    OpType opType;
    bool resolveDir;
    bool predDir;
    UINT64 branchTarget;
    UINT64 cycle;
    PredictedEntry(UINT64 _pc, OpType _opType, bool _resolveDir, bool _predDir, UINT64 _branchTarget, UINT64 _cycle): \
                  PC(_pc), \
                  opType(_opType), \
                  resolveDir(_resolveDir), \
                  predDir(_predDir), \
                  branchTarget(_branchTarget), \
                  cycle(_cycle) { }
};

// Global Variable
UINT64 numIter = 0;
UINT64 numMispred =0;  
PREDICTOR_BASE  *brpred;
UINT64 cond_branch_instruction_counter=0;
UINT64 uncond_branch_instruction_counter=0;
queue<PredictedEntry> predicted_queue;

void CheckHeartBeat(UINT64 numIter, UINT64 numMispred)
{
  UINT64 dotInterval=1000000;
  UINT64 lineInterval=30*dotInterval;

 UINT64 d1K   =1000;
 UINT64 d10K  =10000;
 UINT64 d100K =100000;
 UINT64 d1M   =1000000; 
 UINT64 d10M  =10000000;
 UINT64 d30M  =30000000;
 UINT64 d60M  =60000000;
 UINT64 d100M =100000000;
 UINT64 d300M =300000000;
 UINT64 d600M =600000000;
 UINT64 d1B   =1000000000;
 UINT64 d10B  =10000000000;


  if(numIter == d1K){ //prints MPKI after 100K branches
    printf("MPKBr_1K: %10.4f\n",   1000.0*(double)(numMispred)/(double)(numIter));   
    fflush(stdout);
  }

  if(numIter == d10K){ //prints MPKI after 100K branches
    printf("MPKBr_10K: %10.4f\n",   1000.0*(double)(numMispred)/(double)(numIter));   
    fflush(stdout);
  }
  
  if(numIter == d100K){ //prints MPKI after 100K branches
    printf("MPKBr_100K: %10.4f\n",   1000.0*(double)(numMispred)/(double)(numIter));   
    fflush(stdout);
  }

  if(numIter == d1M){
    printf("MPKBr_1M: %10.4f\n",   1000.0*(double)(numMispred)/(double)(numIter)); 
    fflush(stdout);
  }

  if(numIter == d10M){ //prints MPKI after 100K branches
    printf("MPKBr_10M: %10.4f\n",   1000.0*(double)(numMispred)/(double)(numIter));   
    fflush(stdout);
  }

  if(numIter == d30M){ //prints MPKI after 100K branches
    printf("MPKBr_30M: %10.4f\n",   1000.0*(double)(numMispred)/(double)(numIter));   
    fflush(stdout);
  }

  if(numIter == d60M){ //prints MPKI after 100K branches
    printf("MPKBr_60M: %10.4f\n",   1000.0*(double)(numMispred)/(double)(numIter));   
    fflush(stdout);
  }

  if(numIter == d100M){ //prints MPKI after 100K branches
    printf("MPKBr_100M: %10.4f\n",   1000.0*(double)(numMispred)/(double)(numIter));   
    fflush(stdout);
  }
  
  if(numIter == d300M){ //prints MPKI after 100K branches
    printf("MPKBr_300M: %10.4f\n",   1000.0*(double)(numMispred)/(double)(numIter));   
    fflush(stdout);
  }

  if(numIter == d600M){ //prints MPKI after 100K branches
    printf("MPKBr_600M: %10.4f\n",   1000.0*(double)(numMispred)/(double)(numIter));   
    fflush(stdout);
  }

  if(numIter == d1B){ //prints MPKI after 100K branches
    printf("MPKBr_1B: %10.4f\n",   1000.0*(double)(numMispred)/(double)(numIter));   
    fflush(stdout);
  }
  
  if(numIter == d10B){ //prints MPKI after 100K branches
    printf("MPKBr_10B: %10.4f\n",   1000.0*(double)(numMispred)/(double)(numIter));   
    fflush(stdout);
  }
 
}

bool Predict(bt9::BT9Reader::BranchInstanceIterator & it, OpType &opType, bool & predDir) 
{
  // CheckHeartBeat(++numIter, numMispred);
  try {

      bt9::BrClass br_class = it->getSrcNode()->brClass();
      
      opType = OPTYPE_ERROR;
      if ((br_class.type == bt9::BrClass::Type::UNKNOWN) && (it->getSrcNode()->brNodeIndex())) { //only fault if it isn't the first node in the graph (fake branch)
        printf("Node_index: %ld\n", it->getSrcNode()->brNodeIndex());
        opType = OPTYPE_ERROR; //sanity check
      }
      else if (br_class.type == bt9::BrClass::Type::RET) {
        if (br_class.conditionality == bt9::BrClass::Conditionality::CONDITIONAL) {
          opType = OPTYPE_RET_COND;
          // printf("RET+CND: %ld\n", it->getSrcNode()->brNodeIndex());
        }
        else if (br_class.conditionality == bt9::BrClass::Conditionality::UNCONDITIONAL) {
          opType = OPTYPE_RET_UNCOND;
          // printf("PC: %llx\t Type: ret_unconditional\tTaken_cnt: %ld\tNotTaken_cnt: %ld\tResult: %d\n", \
                  it->getSrcNode()->brVirtualAddr(), \
                  it->getSrcNode()->brObservedTakenCnt(), \
                  it->getSrcNode()->brObservedNotTakenCnt(), \
                  it->getEdge()->isTakenPath());
        }
        else {
          printf("RET+??: %ld\n", it->getSrcNode()->brNodeIndex());
          opType = OPTYPE_ERROR;
        }
      }
      else if (br_class.directness == bt9::BrClass::Directness::INDIRECT) {
        if (br_class.type == bt9::BrClass::Type::CALL) {
          if (br_class.conditionality == bt9::BrClass::Conditionality::CONDITIONAL) {
            opType = OPTYPE_CALL_INDIRECT_COND;
            // printf("CALL+CND: %ld\n", it->getSrcNode()->brNodeIndex());
          }
          else if (br_class.conditionality == bt9::BrClass::Conditionality::UNCONDITIONAL) {
            opType = OPTYPE_CALL_INDIRECT_UNCOND;
            // printf("PC: %llx\t Type: call_indirect_unconditional\tTaken_cnt: %ld\tNotTaken_cnt: %ld\tResult: %d\n", \
                    it->getSrcNode()->brVirtualAddr(), \
                    it->getSrcNode()->brObservedTakenCnt(), \
                    it->getSrcNode()->brObservedNotTakenCnt(), \
                    it->getEdge()->isTakenPath());
          }
          else {
            opType = OPTYPE_ERROR;
          }
        }
        else if (br_class.type == bt9::BrClass::Type::JMP) {
          if (br_class.conditionality == bt9::BrClass::Conditionality::CONDITIONAL) {
            opType = OPTYPE_JMP_INDIRECT_COND;
            // printf("JMP+IND+CND: %ld\n", it->getSrcNode()->brNodeIndex());
          }
          else if (br_class.conditionality == bt9::BrClass::Conditionality::UNCONDITIONAL) {
            opType = OPTYPE_JMP_INDIRECT_UNCOND;
            // printf("PC: %llx\t Type: jmp_indirect_unconditional\tTaken_cnt: %ld\tNotTaken_cnt: %ld\tResult: %d\n", \
                    it->getSrcNode()->brVirtualAddr(), \
                    it->getSrcNode()->brObservedTakenCnt(), \
                    it->getSrcNode()->brObservedNotTakenCnt(), \
                    it->getEdge()->isTakenPath());
          }
          else {
            opType = OPTYPE_ERROR;
          }
        }
        else {
          opType = OPTYPE_ERROR;
        }
      }
      else if (br_class.directness == bt9::BrClass::Directness::DIRECT) {
        if (br_class.type == bt9::BrClass::Type::CALL) {
          if (br_class.conditionality == bt9::BrClass::Conditionality::CONDITIONAL) {
            opType = OPTYPE_CALL_DIRECT_COND;
            // printf("CALL+CND: %ld\n", it->getSrcNode()->brNodeIndex());
          }
          else if (br_class.conditionality == bt9::BrClass::Conditionality::UNCONDITIONAL) {
            opType = OPTYPE_CALL_DIRECT_UNCOND;
            // printf("PC: %llx\t Type: call_direct_unconditional\tTaken_cnt: %ld\tNotTaken_cnt: %ld\tResult: %d\n", \
                    it->getSrcNode()->brVirtualAddr(), \
                    it->getSrcNode()->brObservedTakenCnt(), \
                    it->getSrcNode()->brObservedNotTakenCnt(), \
                    it->getEdge()->isTakenPath());
          }
          else {
            opType = OPTYPE_ERROR;
          }
        }
        else if (br_class.type == bt9::BrClass::Type::JMP) {
          if (br_class.conditionality == bt9::BrClass::Conditionality::CONDITIONAL) {
            opType = OPTYPE_JMP_DIRECT_COND;
            // printf("PC: %llx\t Type: jmp_direct_conditional\tTaken_cnt: %ld\tNotTaken_cnt: %ld\tResult: %d\n", \
                    it->getSrcNode()->brVirtualAddr(), \
                    it->getSrcNode()->brObservedTakenCnt(), \
                    it->getSrcNode()->brObservedNotTakenCnt(), \
                    it->getEdge()->isTakenPath());
          }
          else if (br_class.conditionality == bt9::BrClass::Conditionality::UNCONDITIONAL) {
            opType = OPTYPE_JMP_DIRECT_UNCOND;
            // printf("PC: %llx\t Type: jmp_direct_unconditional\tTaken_cnt: %ld\tNotTaken_cnt: %ld\tResult: %d\n", \
                    it->getSrcNode()->brVirtualAddr(), \
                    it->getSrcNode()->brObservedTakenCnt(), \
                    it->getSrcNode()->brObservedNotTakenCnt(), \
                    it->getEdge()->isTakenPath());
          }
          else {
            opType = OPTYPE_ERROR;
          }
        }
        else {
          opType = OPTYPE_ERROR;
        }
      }
      else {
        // printf("Node_index: %ld\n", it->getSrcNode()->brNodeIndex());
        opType = OPTYPE_ERROR;
      }

      if (opType == OPTYPE_ERROR) {
        if (it->getSrcNode()->brNodeIndex()) {
          fprintf(stderr, "OPTYPE_ERROR\n");
          printf("OPTYPE_ERROR\n");
          exit(-1);
        }
        return false;
      }
      else if (br_class.conditionality == bt9::BrClass::Conditionality::CONDITIONAL) {
        predDir = brpred->GetPrediction(it->getSrcNode()->brVirtualAddr());
        if (predDir != it->getEdge()->isTakenPath()) {
          numMispred++;
        }
        cond_branch_instruction_counter++;
        // printf("Node_index: %d\t PC: %llx\t Predict:%d\n", it->getSrcNode()->brNodeIndex(), it->getSrcNode()->brVirtualAddr(), predDir);
        return true;
      }
      else if (br_class.conditionality == bt9::BrClass::Conditionality::UNCONDITIONAL) {
        uncond_branch_instruction_counter++;
        // brpred->TrackOtherInst(it->getSrcNode()->brVirtualAddr(), opType, \
                               it->getEdge()->isTakenPath(), it->getEdge()->brVirtualTarget());
        return false;
      }
      else {
        fprintf(stderr, "CONDITIONALITY ERROR\n");
        printf("CONDITIONALITY ERROR\n");
        exit(-1);
      }
  }
  catch (const std::out_of_range & ex) {
    std::cout << ex.what() << '\n';
  }
}

void Update(const PredictedEntry & pe)
{
  brpred->UpdatePredictor(pe.PC, pe.opType, pe.resolveDir, pe.predDir, pe.branchTarget);
  // printf("PC: %llx\t%llu cycles\n", pe.PC, pe.cycle);
}

// usage: predictor <trace>
int main(int argc, char* argv[]){
  
  if (argc != 5) {
    printf("usage: %s <trace> <trace_kind> (original|generated) <simulator mode> (0|1) <br_num> (0, 1, 2, ...)\n", argv[0]);
    exit(-1);
  }
  
  ///////////////////////////////////////////////
  // Init variables
  ///////////////////////////////////////////////
    
    brpred = new PREDICTOR_GSHARE();  // this instantiates the predictor code
  ///////////////////////////////////////////////
  // read each trace recrod, simulate until done
  ///////////////////////////////////////////////

    // printf("Using GShare Predictor...\n");
    
    // Ensure the queue is empty
    while (!predicted_queue.empty()) {
      predicted_queue.pop();
    }

    std::string trace_path;
    trace_path = argv[1];
    
    std::string trace_kind = argv[2];
    std::string sim_mode = argv[3];
    if (sim_mode == "0") {
      printf("Mode 0\n");
      mode = 0;
    }
    else if (sim_mode == "1") {
      mode = 1;
      printf("Mode 1\n");
    }
    else {
      printf("Simulating in unknown mode!!!\n");
      exit(-1);
    }
    UINT32 br_num; // branch inst number for a single fetch
    std::string fetch_cnt = argv[4];
    if (fetch_cnt == "0") {
      Rand = true;
      printf("Random\n");
    }
    else {
      Rand = false;
      br_num = std::stoul(fetch_cnt, nullptr, 0);
      if (br_num > 16) {
        printf("br_num is too large!\n");
        exit(-1);
      }
      printf("br_num: %d\n", br_num);
    }

    if (trace_kind == "original") {
      bt9::BT9Reader bt9_reader(trace_path);
      std::string key = "total_instruction_count:";
      std::string value;
      bt9_reader.header.getFieldValueStr(key, value);
      total_instruction_counter = std::stoull(value, nullptr, 0);
      UINT64 current_instruction_counter = 0;
      key = "branch_instruction_count:";
      bt9_reader.header.getFieldValueStr(key, value);
      UINT64     branch_instruction_counter = std::stoull(value, nullptr, 0);
      numMispred =0;  

      cond_branch_instruction_counter=0;
    
      uncond_branch_instruction_counter=0;

      OpType opType;
      //  UINT64 PC;
      //  bool branchTaken;
      //  UINT64 branchTarget;
      numIter = 0;

      UINT64 cycle = 0;
      srand((int)time(NULL));
      bt9::BT9Reader::BranchInstanceIterator it = bt9_reader.begin();
      // auto it = bt9_reader.begin();
      while (it != bt9_reader.end()) {
        cycle++;
        if (mode == 0) {
          while (!predicted_queue.empty()) {
            while (cycle < predicted_queue.front().cycle)
              cycle++;
            Update(predicted_queue.front());
            predicted_queue.pop(); 
          }
        }
        else if (mode == 1) {
          if (!predicted_queue.empty()) {
            while (!predicted_queue.empty() && cycle >= predicted_queue.front().cycle) {
              Update(predicted_queue.front());
              predicted_queue.pop();
            }
            //Modified by Hujiuxin
            //predicted_queue_size limit exceeded
            if (predicted_queue.size()>=MAX_PREQUE_SIZE) {
              while(!predicted_queue.empty()) {
                Update(predicted_queue.front());
                predicted_queue.pop();
              }
            }
          }
        }
        else {
          printf("Simulating in unknown mode!\n");
          exit(-1);
        }
        if (Rand) {
          br_num = rand() % BRANCH_MAX;
        }
        // br_num = rand() % BRANCH_MAX; // br_num: 0 ~ BRANCH_MAX - 1
        // br_num = 1;
        bool predDir;
        UINT64 cycle_delay = cycle;
        for (int i = 0; i < br_num; i++) {
          predDir = false;
          if (Predict(it, opType, predDir)) {
            cycle_delay += 1 + (rand() % DELAY_MAX); // the cycle when updates the predictor 
            PredictedEntry pe(it->getSrcNode()->brVirtualAddr(), opType, it->getEdge()->isTakenPath(), predDir, it->getEdge()->brVirtualTarget(), cycle_delay);
            predicted_queue.push(pe);
          }
          ++it;
          if (it == bt9_reader.end())
            break;
        }
      }
    }
    else if (trace_kind == "generated") {
      total_instruction_counter = 50000000;
      numMispred = 0;
      cond_branch_instruction_counter = 0;
      OpType opType;
      numIter = 0;
      UINT64 cycle = 0;
      srand((int)time(NULL));
      fstream iofile;
      iofile.open(trace_path, ios::in | ios::out);
      if (!iofile) {
        printf("Open file error.\n");
        exit(-1);
      }
      std::string _line;
      while (true) {
        std::getline(iofile, _line);
        // std::cout << _line << endl;
        if (_line.find("PC") != std::string::npos) {
          break;
        }
      }

      while (_line.find("PC") != std::string::npos) {
        cycle++;
        if (mode == 0) {
          while (!predicted_queue.empty()) {
            while (cycle < predicted_queue.front().cycle)
              cycle++;
            Update(predicted_queue.front());
            predicted_queue.pop(); 
          }
        }
        else if (mode == 1) {
          if (!predicted_queue.empty()) {
            while (!predicted_queue.empty() && cycle >= predicted_queue.front().cycle) {
              Update(predicted_queue.front());
              predicted_queue.pop();
            }
            //Modified by Hujiuxin
            //predicted_queue_size limit exceeded
            if (predicted_queue.size()>=MAX_PREQUE_SIZE) {
              while(!predicted_queue.empty()) {
                Update(predicted_queue.front());
                predicted_queue.pop();
              }
            }
          }
        }
        else {
          printf("Simulating in unknown mode!\n");
          exit(-1);
        }

        if (Rand)
          br_num = 1 + (rand() % BRANCH_MAX);
        bool predDir;
        UINT64 cycle_delay = cycle;
        std::string token;
        for (int i = 0; i < br_num; i++) {
          UINT64 _pc;
          bool taken;
          std::stringstream ss(_line);
          ss >> token;
          if (token == "PC:") {
            ss >> token;
            _pc = std::stoull(token, nullptr, 16);
          }
          else {
            printf("Format error!\n");
            exit(-1);
          }
          ss >> token;
          if (token == "Taken:") {
            ss >> token;
            taken = std::stoi(token, nullptr, 0);
          }
          predDir = brpred->GetPrediction(_pc);
          if (predDir != taken) {
            numMispred++;
          }

          // cout << hex << "PC: " << _pc << " pred: " << predDir << " taken: " << taken << endl;

          cond_branch_instruction_counter++;
          cycle_delay += (rand() % DELAY_MAX);
          PredictedEntry pe(_pc, OPTYPE_ERROR, taken, predDir, 0, cycle_delay);
          predicted_queue.push(pe);
          std::getline(iofile, _line);
          if (_line.find("PC") == std::string::npos)
            break;
        }
      }

      iofile.close();
    }
    else {
      printf("Wrong trace kind!!!\n");
      exit(-1);
    }

  
    ///////////////////////////////////////////
    //print_stats
    ///////////////////////////////////////////

    //NOTE: competitors are judged solely on MISPRED_PER_1K_INST. The additional stats are just for tuning your predictors.

      // printf("TRACE: %s\n" , trace_path.c_str()); 

      // printf("NUM_INSTRUCTIONS: %10llu\n", total_instruction_counter);
      // printf("NUM_BR: %10llu\n", branch_instruction_counter-1);
      // printf("NUM_UNCOND_BR:%10llu\t", uncond_branch_instruction_counter);
      printf("NUM_CONDITIONAL_BR:%10llu\t", cond_branch_instruction_counter);
      printf("NUM_MISPREDICTIONS:%10llu\t", numMispred);
      printf("ACCURACY:%10.4f\n", 1 - (double)numMispred / (double)cond_branch_instruction_counter);
      printf("MISPRED_PER_1K_INST:%10.4f\n", 1000.0*(double)(numMispred)/(double)(total_instruction_counter));
      // printf("\n");
}
