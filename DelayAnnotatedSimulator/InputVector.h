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

#ifndef __DelayAnnotatedSimulator__InputVector__
#define __DelayAnnotatedSimulator__InputVector__

#include <cstdlib>
#include <iostream>
#include <vector>
#include <sstream>

//handles the input vector either from user input or from a file.
// eventually this will do more complex file buffering. For now, it's simple.
class InputVector {
    std::istream& source;
    std::string current_buffer;
    unsigned long int line_no;
    unsigned int pi_length;
    bool found_end;
public:
    InputVector(std::istream& source): source(source), line_no(0), found_end(false) {
        if(isStdIn()) {
            std::cout << "Enter ckt input (END to quit): " << std::endl;
        } else {
            std::string line;
            std::getline(source, line);
            std::stringstream ss(line);
            ss >> pi_length;
            std::getline(source, current_buffer);
        }
        line_no++;
    }
    std::vector<char> getNext();
    inline bool isDone() {
        return found_end;
    }
    inline bool isStdIn() {
        return &source == &std::cin;
    }
};
#endif /* defined(__DelayAnnotatedSimulator__InputVector__) */
