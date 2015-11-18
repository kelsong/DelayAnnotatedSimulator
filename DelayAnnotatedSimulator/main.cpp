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

#include <iostream>
#include <fstream>
#include <cstdlib>
#include "Circuit.h"
#include "Type.h"
#include "Gates.h"
#include "Simulator.h"
#include "Args.h"
#include "InputVector.h"

int main(int argc, const char * argv[]) {
    // insert code here...
    Args args;
    args.readArgs(argc, argv);
    Circuit * circuit = new Circuit(args.getCircuitName(), false, false);
    circuit->setGICGroupingSize(args.getGroupingSize());
    LogicSimulator * simulator = new LogicSimulator(circuit);
    InputVector test_vector(args.getInputSource());
    //std::fstream fault_out(args.getCircuitName() + "_fault.csv", std::fstream::out);
    unsigned int vec_num = 0;
    while(!test_vector.isDone()) {
        std::vector<char> vec = test_vector.getNext();
        if(test_vector.isDone())
            break;
        
        simulator->simCycle(vec);
        //std::cerr << "VECTOR # " << vec_num++ << std::endl;
        //fault_out << vec_num++ << ", " << circuit->calculateFaultCov() << "\n";
        if (args.isOutputState()) {
            simulator->dumpState(args.getOutputSource());
        }
        if(args.isOutputPO()) {
            simulator->dumpPO(args.getOutputSource());
        }
    }
    std::fstream gic_out(args.getCircuitName() + std::string("_gic.csv"), std::fstream::out);
    gic_out << "GIC, GateToggle\n";
    simulator->dumpGIC(gic_out);
    gic_out.close();
    //std::cout << "DONE" << std::endl;
    delete circuit;
    delete simulator;
}
