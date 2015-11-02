/*
 The MIT License (MIT)

 Copyright (c) 2015 Kelson Gent

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */

#ifndef __DelayAnnotatedSimulator__Circuit__
#define __DelayAnnotatedSimulator__Circuit__

#include <cstdlib>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <set>

#include "Gates.h"
#include "Fault.h"
#include "Type.h"

#define FF_GROUPING_SIZE 5
//Circuit Class
//For Project 0 create the circuit using false for the delay (there is no dly file for the circuit)

class Circuit {
private:
    //define references for cleanup, set up and simulations
    std::vector<Gate*> allGates;
    std::vector<Gate*> inputs;
    std::vector<Gate*> stateVars;
    std::vector<Gate*> outputs;
    std::vector<Gate*> logicGates;
    unsigned int num_levels;
    
    std::vector<std::vector<bool> > stateGICCoverage;
    

    //fault info
    std::vector<Fault> faultlist;
    unsigned int injected_fault_idx;

public:
    Gate* global_reset;
    Circuit(std::string filename, bool delay, bool fault) {
        if(delay) readDelay(filename + ".dly"); //KEEP
        readLev(filename + ".lev");
        injected_fault_idx = 0;
    };

    ~Circuit();

    void readLev(std::string filename, bool delay); //KEEP
    void readFaultList(std::string filename);
    void readDelay(std::string filename); //KEEP

    std::vector<Gate*> getInputs() {
        return inputs;
    }
    InputGate * getInput(unsigned int idx) {
        return inputs[idx]->castInput();   //in lev format gate id starts at 1
    }
    std::vector<Gate*> getStateVars() {
        return stateVars;
    }
    std::vector<Gate*> getOutputs() {
        return outputs;
    }
    inline unsigned int getNumLevels() {
        return num_levels;
    }
    inline size_t getNumInput() {
        return inputs.size();
    }
    inline size_t getNumOutput() {
        return outputs.size();
    }
    inline size_t getNumStateVar() {
        return stateVars.size();
    }
    inline size_t getNumGates() {
        return allGates.size();
    }
    inline Gate* getStateVar(unsigned int idx) {
        return ((idx < stateVars.size()) ? stateVars[idx] : NULL);
    }
    inline Gate* getOutput(unsigned int idx) {
        return ((idx < outputs.size()) ? outputs[idx] : NULL);
    }
    inline Gate* getGateById(unsigned int gate_id) {
        return ((gate_id <= allGates.size() && gate_id > 0) ?  allGates[gate_id-1] : NULL);
    }

    //this can be used to aid in limiting memory footprint
    void injectFaults(std::vector<Gate*>&);
    void invalidateFaultArrays();
    double calculateFaultCov() const;
    inline size_t numFaults() {
        return faultlist.size();
    }
    inline void resetInjection() {
        injected_fault_idx = 0;

    }
    void printFaults();
    
    void setStateGIC(){
        for(unsigned int i = 0; i<stateVars.size(); i += FF_GROUPING_SIZE){
            unsigned int idx = 0x01;
            bool has_X = false;
            for(unsigned int j = 0; j<FF_GROUPING_SIZE; i++){
                if(stateVars[i+j]->getOut() == LogicValue::X){
                    has_X = true;
                    break;
                }
                
                if(stateVars[i+j]->getOut() == LogicValue::ONE){
                    idx = (idx << 1) | 0x01;
                } else {
                    idx = (idx << 1);
                }
            }
            if(!has_X){
                stateGICCoverage[i/FF_GROUPING_SIZE][idx] = true;
            }
        }
        if(stateVars.size() % FF_GROUPING_SIZE != 0) {
            unsigned int i = stateVars.size() - (stateVars.size() % FF_GROUPING_SIZE);
            bool has_X = false;
            unsigned int idx = 0x01;
            for(; i< stateVars.size(); i++){
                if(stateVars[i]->getOut() == LogicValue::X){
                    has_X = true;
                    break;
                }
                if(stateVars[i]->getOut() == LogicValue::ONE){
                    idx = (idx << 1) | 0x01;
                } else {
                    idx = (idx << 1);
                }
            }
            if(!has_X){
                stateGICCoverage[i/FF_GROUPING_SIZE][idx] = true;
            }
        }
    }
    
    double calculateGIC(){
        unsigned int num_pts = 0;
        unsigned int covered = 0;
        for(unsigned int i = 0; i < allGates.size(); i++) {
            if((allGates[i]->type() != Gate::INPUT) &&
               (allGates[i]->type() != Gate::OUTPUT) &&
               (allGates[i]->type() != Gate::TIE_ONE) &&
               (allGates[i]->type() != Gate::TIE_ONE) &&
               (allGates[i]->type() != Gate::TIE_Z) &&
               (allGates[i]->type() != Gate::TIE_X) &&
               (allGates[i]->type() != Gate::D_FF))
            {
                num_pts += allGates[i]->getNumGICPts();
                covered += allGates[i]->getGICCov();
            }
        }
        
        for(unsigned int i = 0; i < stateGICCoverage.size(); i++){
            num_pts += stateGICCoverage[i].size();
            for(unsigned int j = 0; j < stateGICCoverage.size(); j++){
                if(stateGICCoverage[i][j]){
                    covered++;
                }
            }
        }
        //calculate the state GIC coverage
        return (double) covered / ((double) num_pts);
    }
};


#endif /* defined(__DelayAnnotatedSimulator__Circuit__) */
