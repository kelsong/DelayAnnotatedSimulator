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

void Gate::evaluate(){
    output = LogicValue::X;
}

const std::vector<Gate*>& Gate::getFanin(){
    return fanin;
}

const std::vector<Gate*>& Gate::getFanout(){
    return fanout;
}

//converges faulty and good circuit in FFR after non-propagation
void Gate::converge(){
    //to ensure the correct operation and minimize memory footprint, we converge each gate when it matches the output of the good gate.
    if(!faulty) return;
    if(fanout.size() )
    //pseudo-code:
    // A) remove from fanouts of fanin gates
    // B) converge prior gates if they have no fanouts
    // C) delete gate
    for(int i = 0; i<fanin.size(); i++){
        fanin[i]->removeFanout(this);
    }
    delete this; //not great... Fortunately, we can make some guarantees about the creation of this gate
}

Gate* Gate::createFaultyGate(Fault * fault_create, Gate * gate_create){
    Gate* clne = this->clone();
    return clne;
}

//diverges and creates faulty copies for all fanouts.
void Gate::diverge(){
    fanout.clear();
    for(int i=0; i < good_gate->getNumFanout(); i++){
        fanout.push_back(good_gate->getFanout(i)->createFaultyGate(fault, this));
    }
}

//cleanup method for faulty gates
void Gate::deleteFaulty(){
    for(int i = 0; i<faulty_clones.size(); i++){
        delete faulty_clones[i];
    }
    faulty_clones.clear();
}

/********************************************************/
// AND
/********************************************************/

void AndGate::evaluate(){
    LogicValue previous = output;
    
    LogicValue val = fanin[0]->getOut();
    for(unsigned int i = 1; i<fanin.size(); i++) {
        val = val & fanin[i]->getOut();
    }
    output = val;
    dirty = (previous != output);
}


/********************************************************/
// NAND
/********************************************************/

void NandGate::evaluate(){
    LogicValue previous = output;
    
    LogicValue val = fanin[0]->getOut();
    for(unsigned int i = 1; i<fanin.size(); i++) {
        val = val & fanin[i]->getOut();
    }
    
    output = ~val;
    
    dirty = (previous != output);
}

/********************************************************/
// OR
/********************************************************/

void OrGate::evaluate(){
    LogicValue previous = output;
    
    LogicValue val = fanin[0]->getOut();
    for(unsigned int i = 1; i<fanin.size(); i++) {
        val = val | fanin[i]->getOut();
    }
    
    output = val;
    dirty = (previous != output);
}

/********************************************************/
// NOR
/********************************************************/

void NorGate::evaluate(){
    LogicValue previous = output;
    
    LogicValue val = fanin[0]->getOut();
    for(unsigned int i = 1; i<fanin.size(); i++) {
        val = val | fanin[i]->getOut();
    }
    
    output = ~val;
    dirty = (previous != output);
}

/********************************************************/
// XOR
// ONLY SUPPORT 2 INPUT RIGHT NOW
/********************************************************/

void XorGate::evaluate(){
    LogicValue previous = output;
    
    LogicValue val = fanin[0]->getOut();
    for(int i = 1; i<2; i++) {
        val = val ^ fanin[i]->getOut();
    }
    output = val;
    dirty = (previous != output);
}

/********************************************************/
// XNOR
// ONLY SUPPORT 2 INPUT RIGHT NOW
/********************************************************/

void XnorGate::evaluate(){
    LogicValue previous = output;
    
    LogicValue val = fanin[0]->getOut();
    for(int i = 1; i<2; i++) {
        val = val ^ fanin[i]->getOut();
    }
    output = ~val;
    dirty = (previous != output);
}

/********************************************************/
// NOT
/********************************************************/
void NotGate::evaluate(){
    LogicValue previous = output;

    
    output = ~fanin[0]->getOut();
    
    dirty = (previous != output);
}

/********************************************************/
// BUF
/********************************************************/
void BufGate::evaluate(){
    LogicValue previous = output;
    
    fanin[0]->getOut();
    
    output = fanin[0]->getOut();
    
    dirty = (previous != output);
}

/********************************************************/
// OUTPUT
/********************************************************/

void OutputGate::evaluate(){
    
    output = fanin[0]->getOut();
}

/********************************************************/
// INPUT
/********************************************************/

void InputGate::evaluate(){
    dirty=true; //always true
}

void InputGate::setInput(LogicValue::VALUES in){ //must be used to set value
    output = in;
}

/********************************************************/
// TIE_1
/********************************************************/

void TieOneGate::evaluate(){
    dirty = false;
    output = LogicValue::ONE;
}

/********************************************************/
// TIE_0
/********************************************************/

void TieZeroGate::evaluate(){
    dirty = false;
    output = LogicValue::ZERO;
}
/********************************************************/
// TIE_X
/********************************************************/

void TieXGate::evaluate(){
    dirty = false;
    output = LogicValue::X;
}

/********************************************************/
// TIE_Z
/********************************************************/

void TieZGate::evaluate(){
    dirty = false;
    output = LogicValue::Z;
}

/********************************************************/
// D_FF
/********************************************************/

void DffGate::evaluate(){
    LogicValue previous = output;
    
    output = fanin[0]->getOut();
    
    dirty = (previous != output);
}

void DffGate::setDff(LogicValue::VALUES in){
    output = in;
}


/********************************************************/
// MUX2
/********************************************************/

void Mux2Gate::evaluate(){
    LogicValue previous = output;
    
    if(fanin[0]->getOut() == LogicValue::Z || fanin[0]->getOut() == LogicValue::X){
        output = LogicValue::X; 
    } else {
        output = (fanin[0]->getOut() == LogicValue::ONE) ? fanin[2]->getOut() : fanin[1]->getOut();
    }
    
    dirty = (previous != output);
}


/********************************************************/
// TRISTATE
/********************************************************/
void TristateGate::evaluate(){
    LogicValue previous = output;
    
    if(fanin[1]->getOut() == LogicValue::ZERO){
        output = fanin[0]->getOut();
    } else {
        output = LogicValue::Z;
    }
    dirty = (previous != output);
}
