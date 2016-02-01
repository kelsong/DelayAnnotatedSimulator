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

#ifndef DelayAnnotatedSimulator_EventWheel_h
#define DelayAnnotatedSimulator_EventWheel_h

#include <cstdlib>
#include <vector>
#include <queue>
#include <set>
#include <list>
#include "Gates.h"
#include "Type.h"
#include <iostream>

//base zero delay eventwheel, uses levels.
//TODO: Inherit to make delay
class EventWheel {
protected:
    //for now no delay annotation, just logic simulator
    //list of scheduled events
    std::vector< std::queue<Gate *, std::deque<Gate*> > > scheduled_events;
    std::unordered_set<Gate *> scheduled_set;
    unsigned int current_event_queue;

public:
    EventWheel() : current_event_queue(0) {}
    EventWheel(unsigned int num_levels) : current_event_queue(0) {
        scheduled_events.resize(num_levels);
    }
    virtual ~EventWheel() {}
    virtual void insertEvent(Gate *);
    virtual Gate * getNextScheduled();
    void clearWheel();
};

class GateDelayWheel : EventWheel {
private:
    unsigned int current_time_ns; //used to track the time taken this cycle, so each gates completion time can be annotated.
public:
    GateDelayWheel(unsigned int max_delay) : current_time_ns(0) {
        scheduled_events.resize(max_delay);
    }
    ~GateDelayWheel() {}
    void insertEvent(Gate *);
    Gate * getNextScheduled();
};
#endif
