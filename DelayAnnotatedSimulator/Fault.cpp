//
//  Fault.cpp
//  DelayAnnotatedSimulator
//
//  Created by Kelson Gent on 9/27/15.
//  Copyright © 2015 Kelson Gent. All rights reserved.
//

#include "Fault.h"
#include "Gates.h"

std::vector<Gate*> Fault::injectState(){
    std::map<Gate*, LogicValue>::iterator it = stateStore.begin();
    std::vector<Gate*> injected;
    while(it != stateStore.end()){
        it->first->castDff()->injectStoredFault(this, it->second);
        std::map<Gate*, LogicValue>::iterator to_delete = it;
        injected.push_back(it->first);
        it++;
        stateStore.erase(to_delete);
    }
    return injected;
}

void Fault::storeState(Gate * gate, LogicValue val){
    stateStore[gate] = val;
}