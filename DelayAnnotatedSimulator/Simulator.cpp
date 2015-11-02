/*
 The MIT License (MIT) modified

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

#include "Simulator.h"

std::vector<LogicValue> Simulator::getOutputs() {
    std::vector<Gate*> outs = circuit->getOutputs();
    std::vector<LogicValue> ret;
    for(unsigned int i = 0; i < outs.size(); i++) {
        ret.push_back(outs[i]->getOut());
    }
    return ret;
}

void Simulator::simCycle(const std::vector<char>& input) {
    //do nothing in base simulator.
}

void Simulator::dumpPO(std::ostream& out_stream) {
    for(unsigned int i = 0; i < circuit->getNumOutput(); i++) {
        out_stream << circuit->getOutput(i)->getOut().ascii();
    }
    out_stream << std::endl;
}
//dumps the circuit state to output stream
void Simulator::dumpState(std::ostream& out_stream) {
    for(unsigned int i = 0; i < circuit->getNumStateVar(); i++) {
        out_stream << circuit->getStateVar(i)->getOut().ascii();
    }
    out_stream << std::endl;
}

void LogicSimulator::simCycle(const std::vector<char>& input) {
    //check if input is correct size
    if(input.size() != circuit->getNumInput()) {
        std::cerr << "INVALID INPUT AT: " << cycle_id << std::endl;
    }
    for(unsigned int i = 0; i < input.size(); i++) { //insert all inputs as events
        InputGate * in = circuit->getInput(i);
        if(in) {
            in->setInput(LogicValue::fromChar(input[i]));
            eventwheel->insertEvent(in);
        } else {
            std::cerr << "INVALID INPUT GATE: CKT ERROR" << std::endl;
            exit(-1);
        }
    }

    //always schedule all state vars (there are some optimizations possible, but this is easiest for now)
    for(unsigned int i = 0; i<circuit->getNumStateVar(); i++) {
        eventwheel->insertEvent(circuit->getStateVar(i));
    }

    Gate * gate_to_eval = eventwheel->getNextScheduled();
    while (gate_to_eval != NULL) {
        gate_to_eval->evaluate();
        if(!gate_to_eval->isDirty()) {
            gate_to_eval = eventwheel->getNextScheduled();
            continue;
        }

        for(unsigned int i = 0; i<gate_to_eval->getNumFanout(); i++) {
            if(gate_to_eval->getFanout(i)->type() != Gate::D_FF) {
                eventwheel->insertEvent(gate_to_eval->getFanout(i));
            }
        }

        //clear dirty and move on
        gate_to_eval->resetDirty();
        gate_to_eval = eventwheel->getNextScheduled();
    }
}

/****************************************************************************
 * LogicDelaySimulator
 ****************************************************************************/

void LogicDelaySimulator::simCycle(const std::vector<char> & input) {
    if(input.size() != circuit->getNumInput()) {
        std::cerr << "INVALID INPUT AT: " << cycle_id << std::endl;
    }
    for(unsigned int i = 0; i < input.size(); i++) { //insert all inputs as events
        InputGate * in = circuit->getInput(i);
        if(in) {
            in->setInput(LogicValue::fromChar(input[i]));
            eventwheel->insertEvent(in);
        } else {
            std::cerr << "INVALID INPUT GATE: CKT ERROR" << std::endl;
            exit(-1);
        }
    }

    //always schedule all state vars (there are some optimizations possible, but this is easiest for now)
    for(unsigned int i = 0; i<circuit->getNumStateVar(); i++) {
        //inject X_ids

        eventwheel->insertEvent(circuit->getStateVar(i));
    }

    Gate * gate_to_eval = eventwheel->getNextScheduled();
    while (gate_to_eval != NULL) {
        gate_to_eval->evaluate();
        if(!gate_to_eval->isDirty()) {
            gate_to_eval = eventwheel->getNextScheduled();
            continue;
        }

        for(unsigned int i = 0; i<gate_to_eval->getNumFanout(); i++) {
            if(gate_to_eval->getFanout(i)->type() != Gate::D_FF) {
                eventwheel->insertEvent(gate_to_eval->getFanout(i));
            }
        }

        //clear dirty and move on
        gate_to_eval->resetDirty();
        gate_to_eval = eventwheel->getNextScheduled();
    }
}

/****************************************************************************
 * FaultSimulator
 ****************************************************************************/
void FaultSimulator::simCycle(const std::vector<char>& input) {
    //check if input is correct size
    if(input.size() != circuit->getNumInput()) {
        std::cerr << "INVALID INPUT AT: " << cycle_id << std::endl;
    }
    for(unsigned int i = 0; i < input.size(); i++) { //insert all inputs as events
        InputGate * in = circuit->getInput(i);
        if(in) {
            in->setInput(LogicValue::fromChar(input[i]));
            eventwheel->insertEvent(in);
        } else {
            std::cerr << "INVALID INPUT GATE: CKT ERROR" << std::endl;
            exit(-1);
        }
    }
    
    //always schedule all state vars (there are some optimizations possible, but this is easiest for now)
    for(unsigned int i = 0; i<circuit->getNumStateVar(); i++) {
        eventwheel->insertEvent(circuit->getStateVar(i));
    }
    
    //goodsim
    //std::cerr << "GOODSIM" << std::endl;
    simEvents();
    
    
    //faultsim
    circuit->resetInjection();
    //std::cerr << "FAULTSIM" << std::endl;
    std::vector<Gate*> injected = circuit->injectFaults();
    while(!injected.empty()){
        for(unsigned int i = 0; i < injected.size(); i++){
            eventwheel->insertEvent(injected[i]);
        }
        simEvents();
        circuit->invalidateFaultArrays();
        injected = circuit->injectFaults();
    }
    circuit->clearStateGoodSim();
    
    //Calculate Fault Coverage
    std::cout << "FAULT COV: " << circuit->calculateFaultCov() << std::endl;
}

void FaultSimulator::simEvents(){
    Gate * gate_to_eval = eventwheel->getNextScheduled();
    while (gate_to_eval != NULL) {
        gate_to_eval->evaluate();
        if(!gate_to_eval->isDirty() && !gate_to_eval->propagatesFault()) {
            gate_to_eval = eventwheel->getNextScheduled();
            continue;
        }
        
        for(unsigned int i = 0; i<gate_to_eval->getNumFanout(); i++) {
            if(gate_to_eval->getFanout(i)->type() != Gate::D_FF) {
                eventwheel->insertEvent(gate_to_eval->getFanout(i));
            }
        }
        
        //clear dirty and move on
        gate_to_eval->resetDirty();
        gate_to_eval = eventwheel->getNextScheduled();
    }
}
