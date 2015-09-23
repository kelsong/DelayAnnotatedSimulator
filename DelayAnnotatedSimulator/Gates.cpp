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

    for(size_t i = 0; i<fanout.size(); i++) {
        //replace fanouts with good gate
        fanout[i]->replaceFanin(this);
    }

    for(size_t i = 0; i<fanin.size(); i++) {
        fanin[i]->removeFanout(this);
        fanin[i]->converge();
    }

    good_gate->deleteFaulty(this);
}

void Gate::replaceFanin(Gate * rep) {
    for(size_t i = 0; i<fanin.size(); i++) {
        if(fanin[i] == rep) {
            fanin[i] = rep->goodGate();
        }
    }
}

Gate* Gate::createFaultyGate(Fault * fault_create, Gate * gate_create) {
    //check if there is already a faulty copy

    Gate* clne = this->clone();
    clne->clearFanout(); //empties fanout because this is a faulty gate copy,
    //these will not be populated until a propagation occurs
    return clne;
}

//diverges and creates faulty copies for all fanouts.
void Gate::diverge() {
    for(size_t i=0; i < good_gate->getNumFanout(); i++) {
        if(fanout.size() > i) {
            if(fanout[i]->getId() != good_gate->getFanout(i)->getId()) {
                //this keeps things in the same order
                fanout.insert(fanout.begin()+i, good_gate->getFanout(i)->createFaultyGate(fault, this));
            } else { //already exists
                continue;
            }
        } else {
            fanout.push_back(good_gate->getFanout(i)->createFaultyGate(fault, this));
        }
    }
}

//cleanup method for faulty gates
void Gate::deleteFaulty() {
    for(size_t i = 0; i<faulty_clones.size(); i++) {
        delete faulty_clones[i];
    }
    faulty_clones.clear();
}

//cleanup method for convergence
void Gate::deleteFaulty(Gate * del) {
    for(size_t i = 0; i<faulty_clones.size(); i++) {
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
        if(fault->faultGateNet() >= fanin.size()) { //applies to output
            output = fault->faultSA();
        } else { //apply to fanin
            LogicValue val = LogicValue::ONE;
            for(int i = 0; i<fanin.size(); i++) {
                val &= (fault->faultSA() == i) ? fault->faultSA() : fanin[i]->getOut();
            }
            output = val;
        }
        dirty = true;
        if(output!=good_gate->getOut()) {
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
        if(fault->faultGateNet() >= fanin.size()) { //applies to output
            output = fault->faultSA();
        } else { //apply to fanin
            LogicValue val = LogicValue::ONE;
            for(int i = 0; i<fanin.size(); i++) {
                val &= (fault->faultSA() == i) ? fault->faultSA() : fanin[i]->getOut();
            }
            output = ~val;
        }
        dirty = true;
        if(output!=good_gate->getOut()) {
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
        if(fault->faultGateNet() >= fanin.size()) { //applies to output
            output = fault->faultSA();
        } else { //apply to fanin
            LogicValue val = LogicValue::ONE;
            for(int i = 0; i<fanin.size(); i++) {
                val |= (fault->faultSA() == i) ? fault->faultSA() : fanin[i]->getOut();
            }
            output = val;
        }
        dirty = true;
        if(output!=good_gate->getOut()) {
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
        if(fault->faultGateNet() >= fanin.size()) { //applies to output
            output = fault->faultSA();
        } else { //apply to fanin
            LogicValue val = LogicValue::ONE;
            for(int i = 0; i<fanin.size(); i++) {
                val |= (fault->faultSA() == i) ? fault->faultSA() : fanin[i]->getOut();
            }
            output = ~val;
        }
        dirty = true;
        if(output!=good_gate->getOut()) {
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
        if(fault->faultGateNet() >= fanin.size()) { //applies to output
            output = fault->faultSA();
        } else { //apply to fanin
            LogicValue val = LogicValue::ONE;
            for(int i = 0; i<fanin.size(); i++) {
                val ^= (fault->faultSA() == i) ? fault->faultSA() : fanin[i]->getOut();
            }
            output = val;
        }
        dirty = true;
        if(output!=good_gate->getOut()) {
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
        if(fault->faultGateNet() >= fanin.size()) { //applies to output
            output = fault->faultSA();
        } else { //apply to fanin
            LogicValue val = LogicValue::ONE;
            for(int i = 0; i<fanin.size(); i++) {
                val ^= (fault->faultSA() == i) ? fault->faultSA() : fanin[i]->getOut();
            }
            output = ~val;

        }
        dirty = true;
        if(output!=good_gate->getOut()) {
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
        if(fault->faultGateNet() == 0) {
            output = ~fault->faultSA();
        } else {
            output = fault->faultSA();
        }
        dirty = true;
        if(output!=good_gate->getOut()) {
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
        dirty = true;
        if(output!=good_gate->getOut()) {
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

    output = fanin[0]->getOut();
}

/********************************************************/
// INPUT
/********************************************************/

void InputGate::evaluate() {
    dirty=true; //always true
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

    output = fanin[0]->getOut();

    dirty = (previous != output);
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
