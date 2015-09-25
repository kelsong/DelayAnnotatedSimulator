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

#include "Gates.h"
#include <iostream>

/********************************************************/
// GATE
/********************************************************/

void Gate::evaluate() {
    output = LogicValue::X;
}

const std::vector<Gate*>& Gate::getFanin() {
    return fanin;
}

const std::vector<Gate*>& Gate::getFanout() {
    return fanout;
}

//converges faulty and good circuit in FFR after non-propagation
void Gate::converge() {
    //to ensure the correct operation and minimize memory footprint,
    //we converge each gate when it matches the output of the good gate.
    if(!faulty) return;

    for(unsigned int i = 0; i<fanin.size(); i++) {
        if(fanin[i]->isFaulty()) {
            fanin[i]->removeFanout(this);
            fanin[i]->converge();
        }
    }
    
    if(fanout.size() != 0) {
        dirty = true;
    } else {
        good_gate->deleteFaulty(this);
    }
}

void Gate::replaceFanin(Gate * rep) {
    for(unsigned int i = 0; i<fanin.size(); i++) {
        if(fanin[i] == rep) {
            fanin[i] = rep->goodGate();
        }
    }
}

void Gate::replaceFanin(Gate* good, Gate * fault_in) {
    if (!this->isFaulty()) return;
    for(unsigned int i = 0; i < fanin.size(); i++) {
        if(fanin[i] == good){
            fanin[i] = fault_in;
        }
    }
}

Gate* Gate::createFaultyGate(Fault * fault_create) {
    //check if there is already a faulty copy
    //std::cerr << "CREATE FAULTY " << fault_create->faultGateId()
	//      << " " << fault_create->faultGateNet()
	//      << " " << fault_create->faultSA().ascii() << std::endl;
    Gate* clne = this->clone();
    clne->clearFanout(); //empties fanout because this is a faulty gate copy,
    //these will not be populated until a propagation occurs
    clne->setFault(fault_create);
    clne->setGoodGate(this);
    this->faulty_clones.push_back(clne);
    //std::cerr << "END CREATE FAULTY" << std::endl;
    return clne;
}

Gate* Gate::getFaulty(Fault* flt){
    for(unsigned int i = 0; i<faulty_clones.size(); i++){
        if(faulty_clones[i]->getFault() == flt){
            return faulty_clones[i];
        }
    }
    return NULL;
}

//diverges and creates faulty copies for all fanouts.
void Gate::diverge() {
    for(unsigned int i = 0; i < good_gate->getNumFanout(); i++) {
        Gate* flty = good_gate->getFanout(i)->getFaulty(fault);
        if(flty == NULL){
            flty = good_gate->getFanout(i)->createFaultyGate(fault);
            
        }
        flty->replaceFanin(good_gate, this);
        fanout.push_back(flty);
    }
}

//cleanup method for faulty gates
void Gate::deleteFaulty() {
    for(unsigned int i = 0; i<faulty_clones.size(); i++) {
        delete faulty_clones[i];
    }
    faulty_clones.clear();
}

//cleanup method for convergence
void Gate::deleteFaulty(Gate * del) {
    for(unsigned int i = 0; i<faulty_clones.size(); i++) {
        if(faulty_clones[i] == del) {
            faulty_clones.erase(faulty_clones.begin() + i);
        }
    }
}

/********************************************************/
// AND
/********************************************************/

void AndGate::evaluate() {
    LogicValue previous = output;
    bool fault_eval = false;
    if(faulty) {
        fault_eval = (fault->faultGateId() == gate_id);
    }
    
    if(!fault_eval) {
        LogicValue val = fanin[0]->getOut();
        for(unsigned int i = 1; i<fanin.size(); i++) {
            val = val & fanin[i]->getOut();
        }
        output = val;
        dirty = (previous != output);
    } else {
        if(fault->faultGateNet() == 0) { //applies to output
            output = fault->faultSA();
        } else { //apply to fanin
            LogicValue val = LogicValue::ONE;
            for(unsigned int i = 0; i<fanin.size(); i++) {
                val &= (fault->faultGateNet()-1 == i) ? fault->faultSA() : fanin[i]->getOut();
            }
            output = val;
        }
    }
    
    if(faulty){
        if(output != good_gate->getOut()) {
            dirty = true;
            diverge();
        } else {
            converge();
        }
    }
}


/********************************************************/
// NAND
/********************************************************/

void NandGate::evaluate() {
    LogicValue previous = output;
    bool fault_eval = false;
    if(faulty) {
        fault_eval = (fault->faultGateId() == gate_id);
    }

    if(!fault_eval) {
        LogicValue val = fanin[0]->getOut();
        for(unsigned int i = 1; i<fanin.size(); i++) {
            val = val & fanin[i]->getOut();
        }
        output = ~val;
        dirty = (previous != output);
    } else {
        if(fault->faultGateNet() == 0) { //applies to output
            output = fault->faultSA();
        } else { //apply to fanin
            LogicValue val = LogicValue::ONE;
            for(unsigned int i = 0; i<fanin.size(); i++) {
                val &= (fault->faultGateNet()-1 == i) ? fault->faultSA() : fanin[i]->getOut();
            }
            output = ~val;
        }
    }
    
    if(faulty){
        if(output != good_gate->getOut()) {
            dirty = true;
            diverge();
        } else {
            converge();
        }
    }

}

/********************************************************/
// OR
/********************************************************/

void OrGate::evaluate() {
    LogicValue previous = output;
    bool fault_eval = false;
    if(faulty) {
        fault_eval = (fault->faultGateId() == gate_id);
    }
    
    if(!fault_eval) {
        LogicValue val = fanin[0]->getOut();
        for(unsigned int i = 1; i<fanin.size(); i++) {
            val = val | fanin[i]->getOut();
        }
        output = val;
        dirty = (previous != output);
    } else {
        if(fault->faultGateNet() == 0) { //applies to output
            output = fault->faultSA();
        } else { //apply to fanin
            LogicValue val = LogicValue::ZERO;
            for(unsigned int i = 0; i<fanin.size(); i++) {
                val |= (fault->faultGateNet()-1 == i) ? fault->faultSA() : fanin[i]->getOut();
            }
            output = val;
        }
    }
    
    if(faulty){
        if(output != good_gate->getOut()) {
            dirty = true;
            diverge();
        } else {
            converge();
        }
    }
}

/********************************************************/
// NOR
/********************************************************/

void NorGate::evaluate() {
    LogicValue previous = output;
    bool fault_eval = false;
    if(faulty) {
        fault_eval = (fault->faultGateId() == gate_id);
    }
    
    if(!fault_eval) {
        LogicValue val = fanin[0]->getOut();
        for(unsigned int i = 1; i<fanin.size(); i++) {
            val = val | fanin[i]->getOut();
        }
        output = ~val;
        dirty = (previous != output);
    } else {
        if(fault->faultGateNet() == 0) { //applies to output
            output = fault->faultSA();
        } else { //apply to fanin
            LogicValue val = LogicValue::ZERO;
            for(unsigned int i = 0; i<fanin.size(); i++) {
                val |= (fault->faultGateNet()-1 == i) ? fault->faultSA() : fanin[i]->getOut();
            }
            output = ~val;
        }
    }
    
    if(faulty){
        if(output != good_gate->getOut()) {
            dirty = true;
            diverge();
        } else {
            converge();
        }
    }
}

/********************************************************/
// XOR
// ONLY SUPPORT 2 INPUT RIGHT NOW
/********************************************************/

void XorGate::evaluate() {
    LogicValue previous = output;
    bool fault_eval = false;
    if(faulty) {
        fault_eval = (fault->faultGateId() == gate_id);
    }
    
    if(!fault_eval) {
        LogicValue val = fanin[0]->getOut();
        for(unsigned int i = 1; i<fanin.size(); i++) {
            val = val ^ fanin[i]->getOut();
        }
        output = val;
        dirty = (previous != output);
    } else {
        if(fault->faultGateNet() == 0) { //applies to output
            output = fault->faultSA();
        } else { //apply to fanin
            LogicValue val = LogicValue::ZERO;
            for(unsigned int i = 0; i<fanin.size(); i++) {
                val ^= (fault->faultGateNet()-1 == i) ? fault->faultSA() : fanin[i]->getOut();
            }
            output = val;
        }
    }
    
    if(faulty){
        if(output != good_gate->getOut()) {
            dirty = true;
            diverge();
        } else {
            converge();
        }
    }
}

/********************************************************/
// XNOR
// ONLY SUPPORT 2 INPUT RIGHT NOW
/********************************************************/

void XnorGate::evaluate() {
    LogicValue previous = output;
    bool fault_eval = false;
    if(faulty) {
        fault_eval = (fault->faultGateId() == gate_id);
    }
    
    if(!fault_eval) {
        LogicValue val = fanin[0]->getOut();
        for(unsigned int i = 1; i<fanin.size(); i++) {
            val = val ^ fanin[i]->getOut();
        }
        output = ~val;
        dirty = (previous != output);
    } else {
        if(fault->faultGateNet() == 0) { //applies to output
            output = fault->faultSA();
        } else { //apply to fanin
            LogicValue val = LogicValue::ZERO;
            for(unsigned int i = 0; i<fanin.size(); i++) {
                val ^= (fault->faultGateNet()-1 == i) ? fault->faultSA() : fanin[i]->getOut();
            }
            output = ~val;
        }
    }
    
    if(faulty){
        if(output != good_gate->getOut()) {
            dirty = true;
            diverge();
        } else {
            converge();
        }
    }
}

/********************************************************/
// NOT
/********************************************************/
void NotGate::evaluate() {
    LogicValue previous = output;
    bool fault_eval = false;
    if(faulty) {
        fault_eval = (fault->faultGateId() == gate_id);
    }

    if(!fault_eval) {
        output = ~fanin[0]->getOut();
        dirty = (previous != output);
    } else {
        if(fault->faultGateNet() == 1) {
            output = ~fault->faultSA();
        } else {
            output = fault->faultSA();
        }
        
    }

    if(faulty) {
        if(output!=good_gate->getOut()) {
            dirty = true;
            diverge();
        } else {
            converge();
        }
    }
}

/********************************************************/
// BUF
/********************************************************/
void BufGate::evaluate() {
    LogicValue previous = output;
    bool fault_eval = false;
    if(faulty) {
        fault_eval = (fault->faultGateId() == gate_id);
    }

    if(!fault_eval) {
        output = fanin[0]->getOut();
        dirty = (previous != output);
    } else {
        output = fault->faultSA();
    }
    
    if(faulty){
        if(output!=good_gate->getOut()) {
            dirty = true;
            diverge();
        } else {
            converge();
        }
    }
}

/********************************************************/
// OUTPUT
/********************************************************/

void OutputGate::evaluate() {
    bool fault_eval = false;
    if(faulty) {
        fault_eval = (fault->faultGateId() == gate_id);
    }
    
    if(!fault_eval) {
        output = fanin[0]->getOut();
    } else {
        output = fault->faultSA();
    }
    
    if (faulty){
        if (output != good_gate->getOut()){
            /*if(!fault->isDetected()){
                std::cout << fault->faultGateId() << " "
                        << fault->faultGateNet() << " "
                        << fault->faultSA().ascii() << std::endl;
            }*/
            fault->setDetected();
        } else {
            converge();
        }
    }

}

/********************************************************/
// INPUT
/********************************************************/

void InputGate::evaluate() {
    dirty=true; //always true
    if(faulty){
        output = fault->faultSA();
        if(output != good_gate->getOut()){
            diverge();
        } else {
            converge();
        }
    }
}

void InputGate::setInput(LogicValue::VALUES in) { //must be used to set value
    output = in;
}

/********************************************************/
// TIE_1
/********************************************************/

void TieOneGate::evaluate() {
    dirty = false;
    output = LogicValue::ONE;
}

/********************************************************/
// TIE_0
/********************************************************/

void TieZeroGate::evaluate() {
    dirty = false;
    output = LogicValue::ZERO;
}
/********************************************************/
// TIE_X
/********************************************************/

void TieXGate::evaluate() {
    dirty = false;
    output = LogicValue::X;
}

/********************************************************/
// TIE_Z
/********************************************************/

void TieZGate::evaluate() {
    dirty = false;
    output = LogicValue::Z;
}

/********************************************************/
// D_FF
/********************************************************/

void DffGate::evaluate() {
    LogicValue previous = output;
    bool fault_eval = false;
    if(faulty) {
        fault_eval = (fault->faultGateId() == gate_id);
    }
    
    if(!fault_eval) {
        output = fanin[0]->getOut();
        dirty = (previous != output);
    } else {
        output = fault->faultSA();
    }
    
    if(faulty){
        if(output!=good_gate->getOut()) {
            dirty = true;
            diverge();
        } else {
            converge();
        }
    }
}

void DffGate::setDff(LogicValue::VALUES in) {
    if(!faulty)
        output = in;
}


/********************************************************/
// MUX2
/********************************************************/

void Mux2Gate::evaluate() {
    LogicValue previous = output;

    if(fanin[0]->getOut() == LogicValue::Z || fanin[0]->getOut() == LogicValue::X) {
        output = LogicValue::X;
    } else {
        output = (fanin[0]->getOut() == LogicValue::ONE) ? fanin[2]->getOut() : fanin[1]->getOut();
    }

    dirty = (previous != output);
}


/********************************************************/
// TRISTATE
/********************************************************/
void TristateGate::evaluate() {
    LogicValue previous = output;

    if(fanin[1]->getOut() == LogicValue::ZERO) {
        output = fanin[0]->getOut();
    } else {
        output = LogicValue::Z;
    }
    dirty = (previous != output);
}
