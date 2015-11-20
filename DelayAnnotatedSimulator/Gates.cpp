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

unsigned short Gate::fault_round = 0;
unsigned int Gate::num_injected = 0;

bool Gate::toggle_relax = false;

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


//diverges and creates faulty copies for all fanouts.
inline void Gate::diverge(Fault* flt) {
    for(unsigned int i = 0; i<fanout.size(); i++){
        if(fanout[i]->type() != Gate::D_FF){
            fanout[i]->addFault(flt);
        } else {
            flt->storeState(fanout[i], f_vals[flt->getFID() % NUM_FAULT_INJECT]);
        }
    }
}

void Gate::clearFaultValid(){
    for(int i = 0; i<NUM_FAULT_INJECT; i++){
        valid[i] = false;
    }
}

/********************************************************/
// AND
/********************************************************/

void AndGate::evaluate() {
    //default logic sim
    LogicValue previous = output;
    LogicValue val = fanin[0]->getOut();
    for(unsigned int i = 1; i<fanin.size(); i++) {
        val = val & fanin[i]->getOut();
    }
    output = val;
    dirty = (output != previous);
    if(((previous == LogicValue::ZERO) || toggle_relax) && (output == LogicValue::ONE)) {
        toggled_up = true;
    }
    if(((previous == LogicValue::ONE) || toggle_relax) && (output == LogicValue::ZERO)) {
        toggled_down = true;
    }
    setGIC();
}

void AndGate::faultEvaluate(){
    //fault sim;
    propagates = false;
    for( unsigned int i = 0; i < NUM_FAULT_INJECT; i++){
        if(!valid[i]){
            continue;
        }
        LogicValue fval = LogicValue::ONE;
        bool injection_site = (gate_id == assoc_faults[i]->faultGateId());
        
        for( unsigned int inputs = 0; inputs < fanin.size(); inputs++ ){
            fval &= (injection_site && (assoc_faults[i]->faultGateNet()-1 == inputs))
            ? assoc_faults[i]->faultSA() : fanin[inputs]->getFaultyValue(assoc_faults[i]);
        }
        f_vals[i] = (injection_site && (assoc_faults[i]->faultGateNet() == 0)) ? assoc_faults[i]->faultSA() : fval;
        
        if(f_vals[i] != output){
            propagates = true;
            diverge(assoc_faults[i]);
        }
    }
}


/********************************************************/
// NAND
/********************************************************/

void NandGate::evaluate() {
    //default logic sim
    LogicValue previous = output;
    LogicValue val = fanin[0]->getOut();
    for(unsigned int i = 1; i<fanin.size(); i++) {
        val = val & fanin[i]->getOut();
    }
    output = ~val;
    dirty = (output != previous);
    if(((previous == LogicValue::ZERO) || toggle_relax) && (output == LogicValue::ONE)) {
        toggled_up = true;
    }
    if(((previous == LogicValue::ONE) || toggle_relax) && (output == LogicValue::ZERO)) {
        toggled_down = true;
    }
    setGIC();
}

void NandGate::faultEvaluate(){
    //fault sim;
    propagates = false;
    for( unsigned int i = 0; i < NUM_FAULT_INJECT; i++){
        if(!valid[i]){
            continue;
        }
        LogicValue fval = LogicValue::ONE;
        bool injection_site = (gate_id == assoc_faults[i]->faultGateId());
        
        for( unsigned int inputs = 0; inputs < fanin.size(); inputs++ ){
            fval &= (injection_site && ((assoc_faults[i]->faultGateNet()-1) == inputs))
            ? assoc_faults[i]->faultSA() : fanin[inputs]->getFaultyValue(assoc_faults[i]);
        }
        f_vals[i] = (injection_site && (assoc_faults[i]->faultGateNet() == 0)) ? assoc_faults[i]->faultSA() : ~fval;
        
        if(f_vals[i] != output){
            propagates = true;
            diverge(assoc_faults[i]);
        }
    }
}

/********************************************************/
// OR
/********************************************************/

void OrGate::evaluate() {
    //default logic sim
    LogicValue previous = output;
    LogicValue val = fanin[0]->getOut();
    for(unsigned int i = 1; i<fanin.size(); i++) {
        val = val | fanin[i]->getOut();
    }
    output = val;
    dirty = (output != previous);
    if(((previous == LogicValue::ZERO) || toggle_relax) && (output == LogicValue::ONE)) {
        toggled_up = true;
    }
    if(((previous == LogicValue::ONE) || toggle_relax) && (output == LogicValue::ZERO)) {
        toggled_down = true;
    }
    setGIC();
}

void OrGate::faultEvaluate(){
    //fault sim;
    propagates = false;
    for( unsigned int i = 0; i < NUM_FAULT_INJECT; i++){
        if(!valid[i]){
            continue;
        }
        LogicValue fval = LogicValue::ZERO;
        bool injection_site = (gate_id == assoc_faults[i]->faultGateId());
        
        for( unsigned int inputs = 0; inputs < fanin.size(); inputs++ ){
            fval |= (injection_site && (assoc_faults[i]->faultGateNet()-1 == inputs))
            ? assoc_faults[i]->faultSA() : fanin[inputs]->getFaultyValue(assoc_faults[i]);
        }
        f_vals[i] = (injection_site && (assoc_faults[i]->faultGateNet() == 0)) ? assoc_faults[i]->faultSA() : fval;
        
        if(f_vals[i] != output){
            propagates = true;
            diverge(assoc_faults[i]);
        }
    }
}

/********************************************************/
// NOR
/********************************************************/

void NorGate::evaluate() {
    //default logic sim
    LogicValue previous = output;
    LogicValue val = fanin[0]->getOut();
    for(unsigned int i = 1; i<fanin.size(); i++) {
        val = val | fanin[i]->getOut();
    }
    output = ~val;
    dirty = (output != previous);
    if(((previous == LogicValue::ZERO) || toggle_relax) && (output == LogicValue::ONE)) {
        toggled_up = true;
    }
    if(((previous == LogicValue::ONE) || toggle_relax) && (output == LogicValue::ZERO)) {
        toggled_down = true;
    }
    setGIC();
}

void NorGate::faultEvaluate(){
    //fault sim;
    propagates = false;
    for( unsigned int i = 0; i <NUM_FAULT_INJECT; i++){
        if(!valid[i]){
            continue;
        }
        LogicValue fval = LogicValue::ZERO;
        bool injection_site = (gate_id == assoc_faults[i]->faultGateId());
        
        for( unsigned int inputs = 0; inputs < fanin.size(); inputs++ ){
            fval |= (injection_site && (assoc_faults[i]->faultGateNet()-1 == inputs))
            ? assoc_faults[i]->faultSA() : fanin[inputs]->getFaultyValue(assoc_faults[i]);
        }
        f_vals[i] = (injection_site && (assoc_faults[i]->faultGateNet() == 0)) ? assoc_faults[i]->faultSA() : ~fval;
        
        if(f_vals[i] != output){
            propagates = true;
            diverge(assoc_faults[i]);
        }
    }
}

/********************************************************/
// XOR
// ONLY SUPPORT 2 INPUT RIGHT NOW
/********************************************************/

void XorGate::evaluate() {
    //default logic sim
    LogicValue previous = output;
    LogicValue val = fanin[0]->getOut();
    for(unsigned int i = 1; i<fanin.size(); i++) {
        val = val ^ fanin[i]->getOut();
    }
    output = val;
    dirty = (output != previous);
    if(((previous == LogicValue::ZERO) || toggle_relax) && (output == LogicValue::ONE)) {
        toggled_up = true;
    }
    if(((previous == LogicValue::ONE) || toggle_relax) && (output == LogicValue::ZERO)) {
        toggled_down = true;
    }
    setGIC();
}

void XorGate::faultEvaluate(){
    //fault sim;
    propagates = false;
    for( unsigned int i = 0; i < NUM_FAULT_INJECT; i++){
        if(!valid[i]){
            continue;
        }
        LogicValue fval = LogicValue::ZERO;
        bool injection_site = (gate_id == assoc_faults[i]->faultGateId());
        
        for( unsigned int inputs = 0; inputs < fanin.size(); inputs++ ){
            fval ^= (injection_site && (assoc_faults[i]->faultGateNet()-1 == inputs))
            ? assoc_faults[i]->faultSA() : fanin[inputs]->getFaultyValue(assoc_faults[i]);
        }
        f_vals[i] = (injection_site && (assoc_faults[i]->faultGateNet() == 0)) ? assoc_faults[i]->faultSA() : fval;
        
        if(f_vals[i] != output){
            propagates = true;
            diverge(assoc_faults[i]);
        }
    }
}

/********************************************************/
// XNOR
/********************************************************/

void XnorGate::evaluate() {
    //default logic sim
    LogicValue previous = output;
    LogicValue val = fanin[0]->getOut();
    for(unsigned int i = 1; i<fanin.size(); i++) {
        val = val ^ fanin[i]->getOut();
    }
    output = ~val;
    dirty = (output != previous);
    if(((previous == LogicValue::ZERO) || toggle_relax) && (output == LogicValue::ONE)) {
        toggled_up = true;
    }
    if(((previous == LogicValue::ONE) || toggle_relax) && (output == LogicValue::ZERO)) {
        toggled_down = true;
    }
    setGIC();
}

void XnorGate::faultEvaluate(){
    //fault sim;
    propagates = false;
    for( unsigned int i = 0; i < NUM_FAULT_INJECT; i++){
        if(!valid[i]){
            continue;
        }
        LogicValue fval = LogicValue::ZERO;
        bool injection_site = (gate_id == assoc_faults[i]->faultGateId());
        
        for( unsigned int inputs = 0; inputs < fanin.size(); inputs++ ){
            fval ^= (injection_site && (assoc_faults[i]->faultGateNet()-1 == inputs))
            ? assoc_faults[i]->faultSA() : fanin[inputs]->getFaultyValue(assoc_faults[i]);
        }
        f_vals[i] = (injection_site && (assoc_faults[i]->faultGateNet() == 0)) ? assoc_faults[i]->faultSA() : ~fval;
        
        if(f_vals[i] != output){
            propagates = true;
            diverge(assoc_faults[i]);
        }
    }
}

/********************************************************/
// NOT
/********************************************************/
void NotGate::evaluate() {
    //default logic sim
    LogicValue previous = output;
    output = ~(fanin[0]->getOut());
    dirty = (output != previous);
    if(((previous == LogicValue::ZERO) || toggle_relax) && (output == LogicValue::ONE)) {
        toggled_up = true;
    }
    if(((previous == LogicValue::ONE) || toggle_relax) && (output == LogicValue::ZERO)) {
        toggled_down = true;
    }
    setGIC();
}

void NotGate::faultEvaluate(){
    //fault sim;
    propagates = false;
    for(unsigned int i = 0; i < NUM_FAULT_INJECT; i++){
        if(!valid[i]){
            continue;
        }
        bool injection_site = (gate_id == assoc_faults[i]->faultGateId());
        if(injection_site) {
            f_vals[i] = (assoc_faults[i]->faultGateNet() == 0) ? assoc_faults[i]->faultSA() : ~assoc_faults[i]->faultSA() ;
        } else {
            f_vals[i] = ~(fanin[0]->getFaultyValue(assoc_faults[i]));
        }
        
        if(f_vals[i] != output){
            propagates = true;
            diverge(assoc_faults[i]);
        }
    }
}

/********************************************************/
// BUF
/********************************************************/
void BufGate::evaluate() {
    //default logic sim
    LogicValue previous = output;
    output = fanin[0]->getOut();
    dirty = (output != previous);
    if(((previous == LogicValue::ZERO) || toggle_relax) && (output == LogicValue::ONE)) {
        toggled_up = true;
    }
    if(((previous == LogicValue::ONE) || toggle_relax) && (output == LogicValue::ZERO)) {
        toggled_down = true;
    }
    setGIC();
}

void BufGate::faultEvaluate(){
    //fault sim;
    propagates = false;
    for(unsigned int i = 0; i < NUM_FAULT_INJECT; i++){
        if(!valid[i]){
            continue;
        }
        bool injection_site = (gate_id == assoc_faults[i]->faultGateId());
        if(injection_site) {
            f_vals[i] = assoc_faults[i]->faultSA() ;
        } else {
            f_vals[i] = fanin[0]->getFaultyValue(assoc_faults[i]);
        }
        
        if(f_vals[i] != output){
            propagates = true;
            diverge(assoc_faults[i]);
        }
    }
}

/********************************************************/
// OUTPUT
/********************************************************/

void OutputGate::evaluate() {
    //default logic sim
    LogicValue previous = output;
    output = fanin[0]->getOut();
    
    dirty = (output != previous);
    if(((previous == LogicValue::ZERO) || toggle_relax)&& (output == LogicValue::ONE)) {
        toggled_up = true;
    }
    if(((previous == LogicValue::ONE) || toggle_relax) && (output == LogicValue::ZERO)) {
        toggled_down = true;
    }
}

void OutputGate::faultEvaluate(){
    //fault sim;
    propagates = false;
    for( int i = 0; i < NUM_FAULT_INJECT; i++){
        if(!valid[i]){
            continue;
        }
        bool injection_site = (gate_id == assoc_faults[i]->faultGateId());
        if(injection_site) {
            f_vals[i] = assoc_faults[i]->faultSA() ;
        } else {
            f_vals[i] = fanin[0]->getFaultyValue(assoc_faults[i]);
        }
        
        if((f_vals[i] != output) && (output != LogicValue::X) && (f_vals[i] != LogicValue::X)){
            if(!assoc_faults[i]->isDetected()){
                std::cerr << assoc_faults[i]->faultGateId() << " "
                << assoc_faults[i]->faultGateNet() << " "
                << assoc_faults[i]->faultSA().ascii() << std::endl;
            }
            assoc_faults[i]->setDetected();
        }
    }
}

/********************************************************/
// INPUT
/********************************************************/

void InputGate::evaluate() {
    //default logic sim
    
    dirty = true;
}

void InputGate::faultEvaluate(){
    //fault sim;
    propagates = false;
    for( int i = 0; i < NUM_FAULT_INJECT; i++){
        if(!valid[i]){
            continue;
        }
        if(gate_id != assoc_faults[i]->faultGateId()){
            continue;
        }
        f_vals[i] = assoc_faults[i]->faultSA();
        
        if(f_vals[i] != output){
            propagates = true;
            diverge(assoc_faults[i]);
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
    output = fanin[0]->getOut();
    dirty = (output != previous);
    if(((previous == LogicValue::ZERO) || toggle_relax) && (output == LogicValue::ONE)) {
        toggled_up = true;
    }
    if(((previous == LogicValue::ONE) || toggle_relax) && (output == LogicValue::ZERO)) {
        toggled_down = true;
    }
}

void DffGate::faultEvaluate(){
    //fault sim;
    propagates = false;
    for(unsigned int i = 0; i < NUM_FAULT_INJECT; i++){
        if(!valid[i]){
            continue;
        }
        bool injection_site = (gate_id == assoc_faults[i]->faultGateId());
        if(injection_site) {
            f_vals[i] = assoc_faults[i]->faultSA();
        }
        
        if(f_vals[i] != output && output!=LogicValue::X){
            propagates = true;
            diverge(assoc_faults[i]);
        }
    }
}

void DffGate::setDff(LogicValue::VALUES in) {
    output = in;
}

void DffGate::injectStoredFault(Fault * flt, LogicValue val){
    assoc_faults[flt->getFID() % NUM_FAULT_INJECT] = flt;
    f_vals[flt->getFID() % NUM_FAULT_INJECT] = val;
    valid[flt->getFID() % NUM_FAULT_INJECT] = true;
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
    if(((previous == LogicValue::ZERO) || toggle_relax) && (output == LogicValue::ONE)) {
	toggled_up = true;
    }
    if(((previous == LogicValue::ONE) || toggle_relax) && (output == LogicValue::ZERO)) {
	toggled_down = true;
    }
    setGIC();
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
    if(((previous == LogicValue::ZERO) || toggle_relax) && (output == LogicValue::ONE)) {
	toggled_up = true;
    }
    if(((previous == LogicValue::ONE) || toggle_relax) && (output == LogicValue::ZERO)) {
	toggled_down = true;
    }
}
