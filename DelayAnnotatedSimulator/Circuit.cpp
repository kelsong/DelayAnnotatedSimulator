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

#include "Circuit.h"

void Circuit::readLev(std::string filename, bool delay){
    std::fstream circuit_desc(filename.c_str(), std::fstream::in);
    if(circuit_desc.is_open()){
        std::string line;
        std::getline(circuit_desc, line);
        std::stringstream ss(line);
        std::vector<unsigned int> dff_inputs;
    
        unsigned int num_gates;
        ss >> num_gates;
        num_gates--;
        std::getline(circuit_desc, line); //throw away line
        for(unsigned int i = 0; i < num_gates; i++){
            std::getline(circuit_desc, line);
            std::stringstream gate_info(line);
        
            // Gate characteristics
            unsigned int max_level = 0;
            unsigned int id;
            unsigned int level;
            unsigned int type;
            unsigned int num_fanin;
            std::vector<unsigned int> fanin_ids;
        
            gate_info >> id;
            gate_info >> type;
            gate_info >> level;
            level = level/5;
            if (level > max_level){
                max_level = level;
                num_levels = max_level+1;
            }
            gate_info >> num_fanin;
            for(unsigned int j = 0; j < num_fanin; j++){
                unsigned int tmp;
                gate_info >> tmp;
                fanin_ids.push_back(tmp);
            }
            //ignore rest of the fields (including fan out, fan in linking handles this
            Gate * created_gate;
            switch (type){
                case 1: //INPUT
                    created_gate = new InputGate(id, level);
                    allGates.push_back(created_gate);
                    inputs.push_back(created_gate);
                    break;
                case 2: //OUTPUT
                    created_gate = new OutputGate(id, level);
                    allGates.push_back(created_gate);
                    outputs.push_back(created_gate);
                    break;
                case 3: //XOR
                    created_gate = new XorGate(id, level);
                    allGates.push_back(created_gate);
                    logicGates.push_back(created_gate);
                    break;
                case 4: //XNOR
                    created_gate = new XnorGate(id, level);
                    allGates.push_back(created_gate);
                    logicGates.push_back(created_gate);
                    break;
                case 5: //DFF
                    created_gate = new DffGate(id, level);
                    allGates.push_back(created_gate);
                    stateVars.push_back(created_gate);
                    if(num_fanin == 1){
                        dff_inputs.push_back(fanin_ids.at(0));
                    } else {
                        //error
                        exit(-1);
                    }
                    break;
                case 6: //AND
                    created_gate = new AndGate(id, level);
                    allGates.push_back(created_gate);
                    logicGates.push_back(created_gate);
                    break;
                case 7: //NAND
                    created_gate = new NandGate(id, level);
                    allGates.push_back(created_gate);
                    logicGates.push_back(created_gate);
                    break;
                case 8: //OR
                    created_gate = new OrGate(id, level);
                    allGates.push_back(created_gate);
                    logicGates.push_back(created_gate);
                    break;
                case 9: //NOR
                    created_gate = new NorGate(id, level);
                    allGates.push_back(created_gate);
                    logicGates.push_back(created_gate);
                    break;
                case 10: //NOT
                    created_gate = new NotGate(id, level);
                    allGates.push_back(created_gate);
                    logicGates.push_back(created_gate);
                    break;
                case 11: //BUF
                    created_gate = new BufGate(id, level);
                    allGates.push_back(created_gate);
                    logicGates.push_back(created_gate);
                    break;
                case 12: //TIE1
                    created_gate = new TieOneGate(id, level);
                    allGates.push_back(created_gate);
                    logicGates.push_back(created_gate);
                    break;
                case 13: //TIE0
                    created_gate = new TieZeroGate(id, level);
                    allGates.push_back(created_gate);
                    logicGates.push_back(created_gate);
                    break;
                case 14: //TIEX
                    created_gate = new TieXGate(id, level);
                    allGates.push_back(created_gate);
                    logicGates.push_back(created_gate);
                    break;
                case 15: //TIEZ
                    created_gate = new TieZGate(id, level);
                    allGates.push_back(created_gate);
                    logicGates.push_back(created_gate);
                    break;
                case 16: //MUX2
                    created_gate = new Mux2Gate(id, level);
                    allGates.push_back(created_gate);
                    logicGates.push_back(created_gate);
                    break;
                case 21: //TRISTATE
                    created_gate = new TristateGate(id, level);
                    allGates.push_back(created_gate);
                    logicGates.push_back(created_gate);
                    break;
                default:
                    exit(-1);
                    break;
            }
            if(created_gate->type() != Gate::D_FF){
                for(unsigned int j = 0; j < num_fanin; j++){
                    Gate * fanin = allGates[fanin_ids[j]-1];
                    created_gate->addFanin(fanin);
                    fanin->addFanout(created_gate);
                }
            }
            
            if(delay){
                if(gate_delays.count(created_gate->type()) != 0){
                    created_gate->setDelay(gate_delays[created_gate->type()]);
                } else {
                    //default
                    created_gate->setDelay(1);
                }
            }
        }        
        for(unsigned int i = 0; i<dff_inputs.size(); i++){
            stateVars[i]->addFanin(allGates[dff_inputs[i]-1]);
            allGates[dff_inputs[i]-1]->addFanout(stateVars[i]);
        }
    } else {
        std::cerr << "FILE DOES NOT EXIST" << std::endl;
        exit(-5);
    }
}

void Circuit::readBench(std::string filename){
    //TODO: read in bench format
    
}

void Circuit::levelize(){ //if level information has not been provided
    //TODO: levelize if read in Bench
}

void Circuit::readDelay(std::string filename){
    std::fstream dly_stream(filename);
    if(dly_stream.is_open()){
        std::string line;
        while(dly_stream.good()){
            std::getline(dly_stream, line);
            std::stringstream ss (line);
            if(std::regex_search(line.c_str(), std::regex("[Nn][Aa][Nn][Dd]"))){
                std::string gate;
                unsigned int dly;
                ss >> gate >> dly;
                if(dly > max_delay) max_delay = dly;
                gate_delays[Gate::NAND] = dly;
            } else if (std::regex_search(line.c_str(), std::regex("[Aa][Nn][Dd]"))) {
                std::string gate;
                unsigned int dly;
                ss >> gate >> dly;
                if(dly > max_delay) max_delay = dly;
                gate_delays[Gate::AND] = dly;
            } else if (std::regex_search(line.c_str(), std::regex("[Nn][Oo][Rr]"))) {
                std::string gate;
                unsigned int dly;
                ss >> gate >> dly;
                if(dly > max_delay) max_delay = dly;
                gate_delays[Gate::NOR] = dly;
            } else if (std::regex_search(line.c_str(), std::regex("[Oo][Rr]"))) {
                std::string gate;
                unsigned int dly;
                ss >> gate >> dly;
                if(dly > max_delay) max_delay = dly;
                gate_delays[Gate::NOR] = dly;
            } else if (std::regex_search(line.c_str(), std::regex("[Xx][Oo][Rr]"))) {
                std::string gate;
                unsigned int dly;
                ss >> gate >> dly;
                if(dly > max_delay) max_delay = dly;
                gate_delays[Gate::XOR] = dly;
            } else if (std::regex_search(line.c_str(), std::regex("[Xx][Nn][Oo][Rr]"))) {
                std::string gate;
                unsigned int dly;
                ss >> gate >> dly;
                if(dly > max_delay) max_delay = dly;
                gate_delays[Gate::XNOR] = dly;
            } else if (std::regex_search(line.c_str(), std::regex("[Ii][Nn]"))) {
                std::string gate;
                unsigned int dly;
                ss >> gate >> dly;
                if(dly > max_delay) max_delay = dly;
                gate_delays[Gate::INPUT] = dly;
            } else if (std::regex_search(line.c_str(), std::regex("[Oo][Uu][Tt]"))) {
                std::string gate;
                unsigned int dly;
                ss >> gate >> dly;
                if(dly > max_delay) max_delay = dly;
                gate_delays[Gate::OUTPUT] = dly;
            } else if (std::regex_search(line.c_str(), std::regex("[Bb][Uu][Ff]"))) {
                std::string gate;
                unsigned int dly;
                ss >> gate >> dly;
                if(dly > max_delay) max_delay = dly;
                gate_delays[Gate::BUF] = dly;
            } else if (std::regex_search(line.c_str(), std::regex("[Nn][Oo][Tt]"))) {
                std::string gate;
                unsigned int dly;
                ss >> gate >> dly;
                if(dly > max_delay) max_delay = dly;
                gate_delays[Gate::NOT] = dly;
            } else if (std::regex_search(line.c_str(), std::regex("[Mm][Uu][Xx]"))) {
                std::string gate;
                unsigned int dly;
                ss >> gate >> dly;
                if(dly > max_delay) max_delay = dly;
                gate_delays[Gate::MUX_2] = dly;
            } else if (std::regex_search(line.c_str(), std::regex("[Tt][Ii][Ee]0"))) {
                std::string gate;
                unsigned int dly;
                ss >> gate >> dly;
                if(dly > max_delay) max_delay = dly;
                gate_delays[Gate::TIE_ZERO] = dly;
            } else if (std::regex_search(line.c_str(), std::regex("[Tt][Ii][Ee]1"))) {
                std::string gate;
                unsigned int dly;
                if(dly > max_delay) max_delay = dly;
                ss >> gate >> dly;
                gate_delays[Gate::TIE_ONE] = dly;
            } else if (std::regex_search(line.c_str(), std::regex("[Tt][Ii][Ee]X"))) {
                std::string gate;
                unsigned int dly;
                ss >> gate >> dly;
                if(dly > max_delay) max_delay = dly;
                gate_delays[Gate::TIE_X] = dly;
            } else if (std::regex_search(line.c_str(), std::regex("[Tt][Ii][Ee]Z"))) {
                std::string gate;
                unsigned int dly;
                ss >> gate >> dly;
                if(dly > max_delay) max_delay = dly;
                gate_delays[Gate::TIE_Z] = dly;
            } else if (std::regex_search(line.c_str(), std::regex("[Dd][Ff][Ff]"))) {
                std::string gate;
                unsigned int dly;
                ss >> gate >> dly;
                if(dly > max_delay) max_delay = dly;
                gate_delays[Gate::D_FF] = dly;
            } else if (std::regex_search(line.c_str(), std::regex("[Tt][Rr][Ii]"))) {
                std::string gate;
                unsigned int dly;
                ss >> gate >> dly;
                if(dly > max_delay) max_delay = dly;
                gate_delays[Gate::D_FF] = dly;
            } else {
                std::cerr << "INVALID GATE TYPE IN DELAY FILE: " << line << std::endl;
                exit(-1);
            }
        }
    }
}

std::vector<Gate*> Circuit::getInputs(){
    return inputs;
}

std::vector<Gate*> Circuit::getStateVars(){
    return stateVars;
}

std::vector<Gate*> Circuit::getOutputs(){
    return outputs;
}

Circuit::~Circuit(){
    for(size_t i = 0; i<allGates.size(); i++){
        delete allGates[i];
    }
}

void Circuit::PrintPIFanoutCone(unsigned int gate_id){
    if(gate_id <= inputs.size() && gate_id > 0){
        std::set<unsigned int> visited;
        DFSFanoutRecurse(getInput(gate_id-1), visited);
    } else {
        std::cerr << "ERROR: NOT VALID INPUT GATE ID" << std::endl;
    }
}

void Circuit::DFSFanoutRecurse(Gate * gate, std::set<unsigned int>& visited){
    if((gate->type() == Gate::D_FF) || (gate->type() == Gate::OUTPUT)){
        //base case
        std::cerr << gate->getId() << " ";
    } else if (visited.find(gate->getId()) == visited.end()){
        //recurse
        std::cerr << gate->getId() << " ";
        for(unsigned int i = 0; i < gate->getNumFanout(); i++){
            visited.insert(gate->getId());
            DFSFanoutRecurse(gate->getFanout(i), visited);
        }
    }
}

