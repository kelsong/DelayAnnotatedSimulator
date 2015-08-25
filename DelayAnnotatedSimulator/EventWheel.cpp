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

#include "EventWheel.h"
/*****************************************************************************
 *
 * EventWheel class:
 * Base Zero Delay event wheel
 *
 *****************************************************************************/
void EventWheel::insertEvent(Gate * gate) {
    if(scheduled_set.find(gate) == scheduled_set.end()){
        scheduled_events.at(gate->getLevel()).push(gate);
        scheduled_set.insert(gate);
    }
}

Gate * EventWheel::getNextScheduled() {
    while(scheduled_events.at(current_event_queue).size() == 0) {
        if(current_event_queue == scheduled_events.size()-1){ //reached end of wheel (sim round is done.)
            current_event_queue = 0;
            return NULL;
        }
        current_event_queue++;
    }
    
    Gate * ret = scheduled_events.at(current_event_queue).front();
    scheduled_events.at(current_event_queue).pop();
    scheduled_set.erase(ret);
    return ret;
}

void EventWheel::clearWheel(){
    for(int i = 0; i < scheduled_events.size(); i++){
        scheduled_events.at(i).empty();
    }
    scheduled_set.clear();
}

/*****************************************************************************
 *
 * GateDelayWheel class:
 * Simple Delay event wheel
 *
 *****************************************************************************/
void GateDelayWheel::insertEvent(Gate * gate) {
    unsigned int wheel_space = current_event_queue + gate->getDelay();
    if(wheel_space >= scheduled_events.size()){
        wheel_space = wheel_space - scheduled_events.size();
    }
    scheduled_events.at(wheel_space).push(gate);
}

Gate * GateDelayWheel::getNextScheduled(){
    unsigned int start_position = current_event_queue;
    while(scheduled_events.at(current_event_queue).size() == 0){
        
        current_event_queue++;
        current_time_ns++;
        if(current_event_queue >= scheduled_events.size()){
            current_event_queue -= scheduled_events.size();
        }
        
        if(start_position == current_event_queue){
            current_event_queue = 0;
            return NULL;
        }
    }
    
    Gate * ret = scheduled_events.at(current_event_queue).front();
    scheduled_events.at(current_event_queue).pop();
    return ret;
}
