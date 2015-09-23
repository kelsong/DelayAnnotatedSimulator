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

//base stuck at fault for the simulator.
class Fault {
private:
    unsigned int gate_id;
    unsigned int gate_net;
    LogicValue stuck_at_value;
    bool detected;
    bool active;
public:
    Fault(unsigned int gid, unsigned int net, LogicValue stuck_at) : gate_id(gid), gate_net(net),  stuck_at_value(stuck_at), detected(false) {}
    inline unsigned int faultGateId() {return gate_id;}
    inline unsigned int faultGateNet() {return gate_net;}
    inline LogicValue faultSA() {return stuck_at_value;}
    inline void setDetected(){detected = false;}
};
#endif /* defined(__DelayAnnotatedSimulator__Fault__) */

