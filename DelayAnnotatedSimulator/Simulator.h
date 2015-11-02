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

#ifndef __DelayAnnotatedSimulator__Simulator__
#define __DelayAnnotatedSimulator__Simulator__

#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>
#include "EventWheel.h"
#include "Circuit.h"
#include "Gates.h"
#include "Args.h"
#include "Type.h"

//Base class for simulators. Will be used for LogicSimulator, FaultSimulator, DelaySimulator.
class Simulator {
protected:
    Circuit * circuit;
    unsigned int cycle_id;
public:
    Simulator(Circuit * ckt) : circuit(ckt), cycle_id(0) {}
    virtual ~Simulator() {}
    std::vector<LogicValue> getOutputs();
    virtual void simCycle(const std::vector<char>&); //root function
    void dumpPO( std::ostream& );
    void dumpState( std::ostream& );
};


//this simulator simCycle simulates the positive edge.
//Therefore, flip flops latch in at the beginning of simCycle.

class LogicSimulator: public Simulator {
    EventWheel * eventwheel;
public:
    LogicSimulator(Circuit * ckt): Simulator(ckt) {
        eventwheel = new EventWheel(ckt->getNumLevels());
    }
    ~LogicSimulator() {
        delete eventwheel;
    }
    void simCycle(const std::vector<char>&);
};

class LogicDelaySimulator: public Simulator {
    GateDelayWheel * eventwheel;
    std::vector<unsigned int> output_time;
public:
    LogicDelaySimulator(Circuit * ckt): Simulator(ckt) {
        eventwheel = new GateDelayWheel(ckt->getMaxDelay());
    }
    ~LogicDelaySimulator() {
        delete eventwheel;
    }
    void simCycle(const std::vector<char>&);
};

class FaultSimulator : public Simulator{
    EventWheel * eventwheel;
public:
    FaultSimulator(Circuit * ckt): Simulator(ckt) {
        eventwheel = new EventWheel(ckt->getNumLevels());
    }
    ~FaultSimulator() {
        delete eventwheel;
    }
    void simCycle(const std::vector<char>&);
    void simEvents();
};

class SimulatorFactory {
public:
    static Simulator* create(Args& args);
};
#endif /* defined(__DelayAnnotatedSimulator__Simulator__) */
