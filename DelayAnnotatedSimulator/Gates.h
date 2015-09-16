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

#ifndef DelayAnnotatedSimulator_Gates_h
#define DelayAnnotatedSimulator_Gates_h

//forward declarations
#include <cstdlib>
#include "Type.h"
#include <vector>
#include "Fault.h"

class InputGate;
class DffGate;

//Polymorphic "gate" type. Evaluate is a hot function.
class Gate {
public:
    enum GateType{ //good for now, can expand later
        NONE = 0,
        INPUT,
        OUTPUT,
        AND,
        NAND,
        OR,
        NOR,
        NOT,
        XOR,
        XNOR,
        TIE_ZERO,
        TIE_ONE,
        TIE_X,
        TIE_Z,
        BUF,
        MUX_2,
        TRISTATE,
        D_FF
    };
    
protected:
    unsigned int gate_id;
    enum GateType m_type;
    LogicValue output;
    std::vector<Gate *> fanin;
    std::vector<Gate *> fanout;
    bool dirty; //output changed during eval;
    unsigned int levelnum;
    unsigned int delay; //nanoseconds
    
    //faulty gate information
    bool faulty;
    bool propagates;
    Fault* fault;
    std::vector<Gate*> faulty_clones;
    Gate* good_gate;
    
public:
    Gate(unsigned int idx) : gate_id(idx), output(LogicValue::X) {}
    Gate(unsigned int idx, GateType type, unsigned int level) : gate_id(idx), m_type (type), output(LogicValue::X), levelnum(level), delay(1)  { }
    Gate(unsigned int idx, GateType type, unsigned int level, unsigned int delay) :gate_id(idx), m_type (type), output(LogicValue::X), levelnum(level), delay(delay) {}
    Gate(unsigned int idx, std::vector<Gate *> fin, std::vector<Gate *> fout, GateType type)
    : gate_id(idx), m_type(type), output(LogicValue::X),  fanin(fin), fanout(fout) { }
    virtual ~Gate() { }
    virtual void evaluate(); //eval and schedule if transition
    bool isDirty() { return dirty; }
    void resetDirty() { dirty = false; }
    GateType type() { return m_type; }
    
    
    LogicValue getOut() { return output; }
    const std::vector<Gate *>& getFanout();
    const std::vector<Gate *>& getFanin();
    
    inline void addFanin(Gate * gate) { fanin.push_back(gate); }
    inline void addFanout(Gate * gate) { fanout.push_back(gate); }
    inline void setFanin(std::vector<Gate *>& set_fanin) { fanin = set_fanin; }
    inline void setFanout(std::vector<Gate *>& set_fanout) { fanout = set_fanout; }
    inline size_t getNumFanout() { return fanout.size(); }
    inline size_t getNumFanin() { return fanin.size(); }
    inline Gate* getFanout(unsigned int idx) { return fanout[idx]; }
    inline Gate* getFanin(unsigned int idx) { return fanin[idx]; }
    inline unsigned int getLevel() { return levelnum; }
    inline void setLevel(unsigned int level) { levelnum = level; }
    inline unsigned int getDelay() {return delay;}
    inline void setDelay(unsigned int dly) { delay = dly;}
    inline unsigned int getId() {return gate_id;}
    
    inline void idX() {if(output==LogicValue::X) output.newID();}
    //dynamic cast methods for inputs (don't care at all about logic gates)
    InputGate* castInput();
    DffGate* castDff();
    
    Gate* diverge();
    Gate* createFaultyGate(Fault*);
    void converge();
    void endOfCycleConverge();
};

class AndGate : public Gate{
 public:
    AndGate(unsigned int gid, unsigned int level) : Gate(gid, Gate::AND, level){}
    AndGate(unsigned int gid, std::vector<Gate *> fin, std::vector<Gate *> fout)
        : Gate(gid, fin, fout, Gate::AND) {}
    ~AndGate() {};
    void evaluate();
};

class NandGate : public Gate{
 public:
    NandGate(unsigned int gid, unsigned int level) : Gate(gid, Gate::NAND, level) {}
    NandGate(unsigned int gid, std::vector<Gate *> fin, std::vector<Gate *> fout)
        : Gate(gid, fin, fout, Gate::NAND) {}
    ~NandGate() {}
    void evaluate();
    
};

class OrGate : public Gate{
 public:
    OrGate(unsigned int gid, unsigned int level) : Gate(gid, Gate::OR, level) {}
    OrGate(unsigned int gid, std::vector<Gate *> fin, std::vector<Gate *> fout)
        : Gate(gid, fin, fout, Gate::OR) {}
    ~OrGate() {}
    void evaluate();
};

class NorGate : public Gate{
 public:
    NorGate(unsigned int gid, unsigned int level) : Gate(gid, Gate::NOR, level) {}
    NorGate(unsigned int gid, std::vector<Gate *> fin, std::vector<Gate *> fout)
        : Gate(gid, fin, fout, Gate::NOR) {}
    ~NorGate() {}
    void evaluate();
};

class XorGate : public Gate {
public:
    XorGate(unsigned int gid, unsigned int level) : Gate(gid, Gate::XOR, level) {}
    XorGate(unsigned int gid, std::vector<Gate *> fin, std::vector<Gate *> fout)
        : Gate(gid, fin, fout, Gate::XOR) {}
    ~XorGate() {}
    void evaluate();
};
    
class XnorGate : public Gate{
 public:
    XnorGate(unsigned int gid, unsigned int level) : Gate(gid, Gate::XNOR, level) {}
    XnorGate(unsigned int gid, std::vector<Gate *> fin, std::vector<Gate *> fout)
        : Gate(gid, fin, fout, Gate::XNOR) {}
    ~XnorGate () {}
    void evaluate();
};

class NotGate : public Gate{
 public:
    NotGate(unsigned int gid, unsigned int level) : Gate(gid, Gate::NOT, level) {}
    NotGate(unsigned int gid, std::vector<Gate *> fin, std::vector<Gate *> fout)
        : Gate(gid, fin, fout, Gate::NOT) {}
    ~NotGate() {}
    void evaluate();
};

class InputGate : public Gate{
 public:
    InputGate(unsigned int gid, unsigned int level) : Gate(gid, Gate::INPUT, level) {}
    InputGate(unsigned int gid, std::vector<Gate *> fin, std::vector<Gate *> fout)
        : Gate(gid, fin, fout, Gate::INPUT) {}
    ~InputGate() {}
    void evaluate();
    void setInput(LogicValue::VALUES);
};

class OutputGate : public Gate{
 public:
    OutputGate(unsigned int gid, unsigned int level) : Gate(gid, Gate::OUTPUT, level) {}
    OutputGate(unsigned int gid, std::vector<Gate *> fin, std::vector<Gate *> fout)
        : Gate(gid, fin, fout, Gate::OUTPUT) {}
    ~OutputGate() {}
    void evaluate();
};


class TieZeroGate : public Gate{
 public:
    TieZeroGate(unsigned int gid, unsigned int level) : Gate(gid, Gate::TIE_ZERO, level) {}
    TieZeroGate(unsigned int gid, std::vector<Gate *> fin, std::vector<Gate *> fout)
    : Gate(gid, fin, fout, Gate::TIE_ZERO) {}
    ~TieZeroGate(){}
    void evaluate();
    
};

class TieOneGate : public Gate{
 public:
    TieOneGate(unsigned int gid, unsigned int level) : Gate(gid, Gate::TIE_ONE, level) {}
    TieOneGate(unsigned int gid, std::vector<Gate *> fin, std::vector<Gate *> fout)
    : Gate(gid, fin, fout, Gate::TIE_ONE) {}
    ~TieOneGate() {}
    void evaluate();
    
};

class TieXGate : public Gate{
 public:
    TieXGate(unsigned int gid, unsigned int level) : Gate(gid, Gate::TIE_X, level) {}
    TieXGate(unsigned int gid, std::vector<Gate *> fin, std::vector<Gate *> fout)
        : Gate(gid, fin, fout, Gate::TIE_X) {}
    ~TieXGate() {}
    void evaluate();
    
};

class TieZGate : public Gate{
 public:
    TieZGate(unsigned int gid, unsigned int level) : Gate(gid, Gate::TIE_Z, level) {}
    TieZGate(unsigned int gid, std::vector<Gate *> fin, std::vector<Gate *> fout)
        : Gate(gid, fin, fout, Gate::TIE_Z) {}
    ~TieZGate() {}
    void evaluate();
    
};

class BufGate : public Gate{
 public:
    BufGate(unsigned int gid, unsigned int level) : Gate(gid, Gate::BUF, level) {}
    BufGate(unsigned int gid, std::vector<Gate *> fin, std::vector<Gate *> fout)
        : Gate(gid, fin, fout, Gate::BUF) {}
    ~BufGate() {}
    void evaluate();
    
};

class DffGate : public Gate{
 public:
    DffGate(unsigned int gid, unsigned int level) : Gate(gid, Gate::D_FF, level) {}
    DffGate(unsigned int gid, std::vector<Gate *> fin, std::vector<Gate *> fout)
        : Gate(gid, fin, fout, Gate::D_FF) {}
    ~DffGate() {}
    void evaluate();
    void setDff(LogicValue::VALUES);
    
};

class Mux2Gate : public Gate{
 public:
    Mux2Gate(unsigned int gid, unsigned int level) : Gate(gid, Gate::MUX_2, level) {}
    Mux2Gate(unsigned int gid, std::vector<Gate *> fin, std::vector<Gate *> fout)
    : Gate(gid, fin, fout, Gate::MUX_2) {}
    ~Mux2Gate() {}
    void evaluate();
};

class TristateGate : public Gate{
public:
    TristateGate(unsigned int gid, unsigned int level) : Gate(gid, Gate::TRISTATE, level) {}
    TristateGate(unsigned int gid, std::vector<Gate *> fin, std::vector<Gate *> fout)
    : Gate(gid, fin, fout, Gate::TRISTATE) {}
    ~TristateGate() {}
    void evaluate();
};

inline InputGate* Gate::castInput() { return dynamic_cast<InputGate* >(this); }
inline DffGate* Gate::castDff() { return dynamic_cast<DffGate*>(this); }
#endif

