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

#ifndef __DelayAnnotatedSimulator__Fault__
#define __DelayAnnotatedSimulator__Fault__

#include <map>
#include <vector>
#include "Type.h"
class Gate;

//base stuck at fault for the simulator.
class Fault {
private:
    unsigned int fault_id;
    unsigned int gate_id;
    unsigned int gate_net;
    LogicValue stuck_at_value;
    bool detected;
    std::map<Gate*, LogicValue> stateStore;
public:
    Fault(unsigned int gid, unsigned int net, LogicValue stuck_at, unsigned int fault_id) : gate_id(gid), gate_net(net),  stuck_at_value(stuck_at), detected(false), fault_id(fault_id)  {}
    inline unsigned int faultGateId() const {
        return gate_id;
    }
    inline unsigned int faultGateNet() const {
        return gate_net;
    }
    inline LogicValue faultSA() const {
        return stuck_at_value;
    }
    inline void setDetected() {
        detected = true;
    }
    inline bool isDetected() const {
        return detected;
    }
    inline unsigned int getFID(){
        return fault_id;
    }
    inline void setRoundID(unsigned int r_id){
        fault_id = r_id;
    }
    std::vector<Gate*> injectState();
    void storeState(Gate * gate, LogicValue val);
};
#endif /* defined(__DelayAnnotatedSimulator__Fault__) */

