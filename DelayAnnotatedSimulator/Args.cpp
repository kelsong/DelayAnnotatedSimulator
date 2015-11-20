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

#include "Args.h"

void Args::readArgs(int argc, const char * argv[] ) {
    auto printhelp = [](){
        std::cerr << "USAGE: " << std::endl;
        std::cerr << "(Req.) -ckt <ckt_name> : define circuit name\n"
        << "       -vec       : flag for reading input vector from <ckt_name>.vec\n"
        << "                   defaults to stdin\n"
        << "       -fsim      : flag for fault simulation\n"
        << "                   defaults to logic simulation\n"
        << "       -dly       : reads gate delays from <ckt_name.dly>\n"
        << "       -wpo       : output POs\n"
        << "       -wstate    : output flip flops\n"
        << "       -cov <options>: a variety of gate level coverage metrics\n"
        << "Available Coverage Metrics:\n"
        << "    g: GEX/GIC coverage\n"
        << "    t: net toggle coverage\n"
        << "Coverage Specific Arguments: \n"
        << "  GEX/GIC Coverage: "
        << "    -exclude <net ids>: exclude nets from the GIC calculation UNIMP\n"
        << "    -gic_off <list<input, value>>: turns GIC off if the input matches the value UNIMP\n"
        << "    -grp <int>: Grouping size of flip flips (default 5)\n"
        << "  Toggle Coverage:\n"
        << "    -relaxed: only requires 0 and a 1 value (default excludes transistions from X)\n";
        exit(-1);
    };
    
    if(argc == 1) {
        printhelp();
    }
    //set defaults
    outputState = false;
    outputPO = false;
    simulator_type = 0; //logic simulator
    input_source = &std::cin;
    output_source = &std::cout;
    bool from_file=false;
    for(int i = 1; i < argc; i++) {
        std::string arg(argv[i]);
        if(arg.compare("-ckt") == 0) {
            if(i + 1 >= argc) {
                std::cerr << "ERROR: Invalid usage -ckt <ckt_name> requires <ckt_name>" << std::endl;
                exit(-10);
            }
            setCircuitName(argv[++i]);
        } else if(arg.compare("-vec") == 0) {
            from_file = true;
        } else if(arg.compare("-fsim") == 0) {
            simulator_type = 1;
        } else if(arg.compare("-dly") == 0) {
            simulator_type += 1;
        } else if(arg.compare("-wpo") == 0) {
            outputPO = true;
        } else if(arg.compare("-wstate") == 0) {
            outputState = true;
        } else if(arg.compare("-grp") == 0) {
            std::stringstream ss(argv[++i]);
            ss >> grouping_size;
        } else if(arg.compare("-cov") == 0) {
            std::string options(argv[++i]);
            if(options.size() == 0) printhelp();
            for(int i = 0; i < options.size(); i++){
                if( options[i] == 'g') {
                    GIC = true;
                } else if ( options[i] == 't'){
                    toggle = true;
                } else {
                    printhelp();
                }
            }
        } else {
            std::cerr << "USAGE: " << std::endl;
            std::cerr << "(Req.) -ckt <ckt_name>  define circuit name" << std::endl;
            exit(-1);
        }
    }

    if(from_file) {
        if(circuit.empty()) {
            exit(-1);
        }
        std::string vec_file = circuit + ".vec";
        input_source = new std::fstream(vec_file.c_str(), std::fstream::in);
    }
}
