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


#ifndef DelayAnnotatedSimulator_Args_h
#define DelayAnnotatedSimulator_Args_h

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

class Args {
private:
    //all flags and global properties
    std::string circuit;
    std::istream* input_source;
    std::ostream* output_source;
    unsigned int simulator_type;
    unsigned int grouping_size;
    bool outputState;
    bool outputPO;
public:
    //getter/setters
    inline void setCircuitName(std::string name) {
        circuit = name;
    }
    inline std::string getCircuitName() const {
        return circuit;
    }
    inline void setInputSource(std::istream* isource) {
        input_source = isource;
    }
    inline std::istream& getInputSource() const {
        return *input_source;
    }
    inline std::ostream& getOutputSource() const {
        return *output_source;
    }
    inline void setSimulatorType(unsigned int type) {
        simulator_type = type;
    }
    inline unsigned int getSimulatorType() const {
        return simulator_type;
    }
    inline unsigned int getGroupingSize() const {
        return grouping_size;
    }
    inline bool isOutputState() const {
        return outputState;
    }
    inline bool isOutputPO() const {
        return outputPO;
    }

    void readArgs(int argc, const char* argv[]);
};

#endif
