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

/********************************************************/
// AND
/********************************************************/

void AndGate::evaluate(){
    LogicValue previous = output;
    
    LogicValue val = fanin[0]->getOut();
    for(int i = 1; i<fanin.size(); i++) {
        if(fanin[i]->getOut() != LogicValue::Z)
            val = LogicValue::VALUES(val & fanin[i]->getOut());
    }
    
    output = val;
    if(previous != output){
        dirty=true;
    }
}


/********************************************************/
// NAND
/********************************************************/

void NandGate::evaluate(){
    LogicValue previous = output;
    
    LogicValue val = fanin[0]->getOut();
    for(int i = 1; i<fanin.size(); i++) {
        if(fanin[i]->getOut() != LogicValue::Z)
            val = LogicValue::VALUES(val & fanin[i]->getOut());
    }
    
    output = (val == LogicValue::X) ? val : LogicValue(LogicValue::VALUES(LogicValue::ONE - val));
    
    if(previous != output){
        dirty=true;
    }
}

/********************************************************/
// OR
/********************************************************/

void OrGate::evaluate(){
    LogicValue previous = output;
    
    LogicValue val = fanin[0]->getOut();
    for(int i = 1; i<fanin.size(); i++) {
        if(fanin[i]->getOut() != LogicValue::Z)
            val = LogicValue::VALUES(val | fanin[i]->getOut());
    }
    
    output = val;
    if(previous != output){
        dirty=true;
    }
}

/********************************************************/
// NOR
/********************************************************/

void NorGate::evaluate(){
    LogicValue previous = output;
    
    LogicValue val = fanin[0]->getOut();
    for(int i = 1; i<fanin.size(); i++) {
        if(fanin[i]->getOut() != LogicValue::Z)
            val = LogicValue::VALUES(val | fanin[i]->getOut());
    }
    
    output = (val == LogicValue::X) ? val : LogicValue(LogicValue::VALUES(LogicValue::ONE - val));
    if(previous != output){
        dirty=true;
    }
}

/********************************************************/
// XOR
// ONLY SUPPORT 2 INPUT RIGHT NOW
/********************************************************/

void XorGate::evaluate(){
    LogicValue previous = output;
    
    LogicValue val = fanin[0]->getOut();
    for(int i = 1; i<2; i++) {
        if(fanin[i]->getOut() != LogicValue::Z) {
            val = (val == LogicValue::X || fanin[i]->getOut() == LogicValue::X)
            ? LogicValue::VALUES(val ^ fanin[i]->getOut()) : LogicValue::X;
        }
    }
    output = val;
    if(previous != output){
        dirty=true;
    }
}

/********************************************************/
// XNOR
// ONLY SUPPORT 2 INPUT RIGHT NOW
/********************************************************/

void XnorGate::evaluate(){
    LogicValue previous = output;
    
    LogicValue val = fanin[0]->getOut();
    for(int i = 1; i<2; i++) {
        if(fanin[i]->getOut() != LogicValue::Z) {
            val = (val == LogicValue::X || fanin[i]->getOut() == LogicValue::X)
            ? LogicValue::VALUES(val ^ fanin[i]->getOut()) : LogicValue::X;
        }
    }
    output = (val == LogicValue::X) ? val : LogicValue(LogicValue::VALUES(LogicValue::ONE - val));
    if(previous != output){
        dirty=true;
    }
}

/********************************************************/
// NOT
/********************************************************/
void NotGate::evaluate(){
    LogicValue previous = output;
    
    LogicValue val = fanin[0]->getOut();
    
    output = (val == LogicValue::X) ? val : LogicValue(LogicValue::VALUES(LogicValue::ONE - val));
    
    if(previous != output){
        dirty=true;
    }
}

/********************************************************/
// BUF
/********************************************************/
void BufGate::evaluate(){
    LogicValue previous = output;
    
    LogicValue val = fanin[0]->getOut();
    
    output = val;
    
    if(previous != output){
        dirty=true;
    }
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
    
    LogicValue val = fanin[0]->getOut();
    
    output = val;
    
    if(previous != output){
        dirty=true;
    }
}

/********************************************************/
// MUX2
/********************************************************/

void Mux2Gate::evaluate(){ //DFFS are a bit weird.
    LogicValue previous = output;
    
    if(fanin[0]->getOut() == LogicValue::Z || fanin[0]->getOut() == LogicValue::X){
        output = LogicValue::X;
    } else {
        output = (fanin[0]->getOut() == LogicValue::ONE) ? fanin[2]->getOut() : fanin[1]->getOut();
    }
    
    if (previous != output) {
        dirty = true;
    }
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
    
    if (previous != output) {
        dirty = true;
    }
}
