//
//  Fault.cpp
//  DelayAnnotatedSimulator
//
//  Created by Kelson Gent on 9/27/15.
//  Copyright © 2015 Kelson Gent. All rights reserved.
//

#include "Fault.h"
#include "Gates.h"

void Fault::injectState(std::vector<Gate*> & injected){
    while(!stateStore.isEmpty()){
        LogicValue tmp;
        Gate * gate_tmp;
        stateStore.pop(gate_tmp, tmp);
        gate_tmp->castDff()->injectStoredFault(this, tmp);
        injected.push_back(gate_tmp);
    }
}

void Fault::storeState(Gate * gate, LogicValue val){
    stateStore.push(gate, val);
}