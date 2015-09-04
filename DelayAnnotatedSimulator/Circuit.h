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
#include "Type.h"

//Circuit Class
//For Project 0 create the circuit using false for the delay (there is no dly file for the circuit)

class Circuit{
private:
    //define references for cleanup, set up and simulations
    std::vector<Gate*> allGates;
    std::vector<Gate*> inputs;
    std::vector<Gate*> stateVars;
    std::vector<Gate*> outputs;
    std::vector<Gate*> logicGates;
    unsigned int num_levels;
    std::map<unsigned int, unsigned int> gate_delays;
    unsigned int max_delay;
    
public:
    Circuit(std::string filename, bool delay) { if(delay) readDelay(filename + ".dly"); readLev(filename + ".lev", delay); };
    ~Circuit();
    
    void readLev(std::string filename, bool delay);
    void readBench(std::string filename);
    void readDelay(std::string filename);
    void levelize();
    
    std::vector<Gate*> getInputs() {return inputs;}
    InputGate * getInput(unsigned int idx) {return inputs[idx]->castInput();} //in lev format gate id starts at 1
    std::vector<Gate*> getStateVars() {return stateVars;}
    std::vector<Gate*> getOutputs() {return outputs;}
    inline unsigned int getNumLevels() { return num_levels; }
    inline size_t getNumInput() { return inputs.size(); }
    inline size_t getNumOutput() { return outputs.size(); }
    inline size_t getNumStateVar() { return stateVars.size(); }
    inline size_t getNumGates() { return allGates.size(); }
    inline Gate* getStateVar(unsigned int idx) { return ((idx < stateVars.size()) ? stateVars[idx] : NULL); }
    inline Gate* getOutput(unsigned int idx) { return ((idx < outputs.size()) ? outputs[idx] : NULL); }
    inline Gate* getGateById(unsigned int gate_id) { return ((gate_id < allGates.size()) ?  allGates[gate_id-1] : NULL); }
    
    void PrintPIFanoutCone(unsigned int idx);
    void DFSFanoutRecurse(Gate* gate, std::set<unsigned int>&);
};


#endif /* defined(__DelayAnnotatedSimulator__Circuit__) */
