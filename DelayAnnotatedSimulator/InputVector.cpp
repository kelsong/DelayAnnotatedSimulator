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

#include "InputVector.h"

std::vector<char> InputVector::getNext() {
    if(isDone()) {
        std::cerr << "No more vectors for getNext" << std::endl;
        exit(-5);
    }
    std::vector<char> ret;

    if(isStdIn()) {
        std::getline(source, current_buffer);
        if(current_buffer.compare("END") == 0) {
            found_end = true;
            return ret;
        }
    }
    if(current_buffer.compare("END") == 0) {
        found_end = true;
    } else {
        for(unsigned int i = 0; i<pi_length; i++) {
            if(current_buffer[i] == '0' || current_buffer[i] == '1' || current_buffer[i] == 'X' || current_buffer[i] == 'x') {
                ret.push_back(current_buffer[i]);
            }  else {
                std::cerr << "Input Error on input " << line_no << ": "
                          << "Inputs must be a '0', '1', X or 'END' " << std::endl;
                exit(-5);
            }
            //line_no++;
        }
	line_no++;
    }

    if(!isStdIn() && !found_end) {
        std::getline(source,current_buffer);
    }

    return ret;
}
