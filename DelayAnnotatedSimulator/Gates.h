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
#include <map>
#include "Fault.h"

class InputGate;
class OutputGate;
class DffGate;

//Polymorphic "gate" type. Evaluate is a hot function.
class Gate {
public:
    enum GateType { //good for now, can expand later
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
    unsigned int delay;  //nanoseconds KEEP
    bool scheduled;
    
    std::vector<bool> GIC_coverage;
    //faulty gate information
    bool propagates;
    LogicValue f_vals[NUM_FAULT_INJECT];
    Fault * assoc_faults[NUM_FAULT_INJECT];
    bool valid[NUM_FAULT_INJECT];
    
    bool toggled_up = false;
    bool toggled_down = false;
    //meta-information from faulty circuit
    static unsigned short fault_round;
    static unsigned int num_injected; 
    
public:
    static bool toggle_relax;
    
    bool calc_GIC;
    Gate(unsigned int idx) : gate_id(idx), output(LogicValue::X) {
        for(int i = 0; i < NUM_FAULT_INJECT; i++) {valid[i] = false;}
        
    }
    Gate(unsigned int idx, GateType type, unsigned int level) : gate_id(idx), m_type (type), output(LogicValue::X), levelnum(level), scheduled(false) {
        for(int i = 0; i < NUM_FAULT_INJECT; i++) {valid[i] = false;}
    }
    Gate(unsigned int idx, std::vector<Gate *> fin, std::vector<Gate *> fout, GateType type)
        : gate_id(idx), m_type(type), output(LogicValue::X),  fanin(fin), fanout(fout) {
            for(int i = 0; i < NUM_FAULT_INJECT; i++) {valid[i] = false;}
        }
    virtual ~Gate() { }
    
    virtual void evaluate(); //eval and schedule if transition
    
    void createGIC(){
        unsigned int num_gic = 0x01;
        for(int i = 0; i<fanin.size(); i++) {
            if(fanin[i]->type() != TIE_ZERO && fanin[i]->type() != TIE_ONE) {
                num_gic = num_gic << 1;
            }
        }
        GIC_coverage.resize(num_gic);
        for(int i = 0; i<GIC_coverage.size(); i++){
            GIC_coverage[i] = false;
        }
    }
    bool isDirty() {
        return dirty;
    }
    void resetDirty() {
        dirty = false;
    }
    GateType type() {
        return m_type;
    }
    virtual Gate* clone() {
        return new Gate(*this);
    }


    inline LogicValue getOut() {
        return output;
    }
    const std::vector<Gate *>& getFanout();
    const std::vector<Gate *>& getFanin();

    //fanin/out methods
    inline void addFanin(Gate * gate) {
        fanin.push_back(gate);
    }
    inline void addFanout(Gate * gate) {
        fanout.push_back(gate);
    }
    inline void setFanin(std::vector<Gate *>& set_fanin) {
        fanin = set_fanin;
    }
    inline void setFanout(std::vector<Gate *>& set_fanout) {
        fanout = set_fanout;
    }
    inline size_t getNumFanin() {
        return fanin.size();
    }
    inline size_t getNumFanout() {
        return fanout.size();
    }
    inline Gate* getFanin(unsigned int idx) {
        return fanin[idx];
    }
    inline Gate* getFanout(unsigned int idx) {
        return fanout[idx];
    }
    inline void clearFanin() {
        fanin.clear();
    }
    inline void clearFanout() {
        fanout.clear();
    }

    //Gate info methods
    inline unsigned int getLevel() {
        return levelnum;
    }
    inline void setLevel(unsigned int level) {
        levelnum = level;
    }
    inline unsigned int getDelay() {
        return delay;
    }
    inline void setDelay(unsigned int dly) {
        delay = dly;
    }
    inline unsigned int getId() {
        return gate_id;
    }
    inline void setScheduled() {
        scheduled = true;
    }
    inline void unsetScheduled(){
        scheduled = false;
    }
    inline bool isScheduled(){
        return scheduled;
    }

    //faulty gate methods
    void diverge(Fault *);
    void clearFaultValid();
    virtual void faultEvaluate() {}
    
    inline bool propagatesFault(){
        return propagates;
    }
    inline void addFault(Fault * flt){
        assoc_faults[flt->getFID() % NUM_FAULT_INJECT] = flt;
        valid[flt->getFID() % NUM_FAULT_INJECT] = true;
    }
    inline LogicValue getFaultyValue(Fault * flt){
        if(valid[flt->getFID() % NUM_FAULT_INJECT]){
            return f_vals[flt->getFID() % NUM_FAULT_INJECT];
        } else {
            return output;
        }
    }
    static void setFaultRound(unsigned short round) {
        fault_round = round;
    }
    static void setNumInjected(unsigned int num){
        num_injected = num;
    }
    
    inline void setGIC(){
        if(!calc_GIC) return;
        unsigned int idx = 0;
        for(unsigned int i = 0; i < fanin.size(); i++){
            if(fanin[i]->type() == TIE_ZERO || fanin[i]->type() == TIE_ONE) continue;
            if(fanin[i]->getOut() == LogicValue::X || fanin[i]->getOut() == LogicValue::Z){
                return;
            } else {
                if(fanin[i]->getOut() == LogicValue::ONE){
                    idx = (idx << 1) | 0x01;
                }
                else {
                    idx = (idx << 1);
                }
            }
        }
        GIC_coverage[idx] = true;
    }
    
    inline unsigned int getGICCov(){
        unsigned int cnt = 0;
        for(int i = 0; i<GIC_coverage.size(); i++){
            if(GIC_coverage[i] == true){
                cnt++;
            }
        }
        return cnt;
    }
    
    inline unsigned int getNumGICPts(){
        return GIC_coverage.size();
    }
    
    inline bool hasToggled() {
        return (toggled_up && toggled_down);
    }
    
    inline bool toggledUp() {
        return toggled_up;
    }
    
    inline bool toggledDown() {
        return toggled_down;
    }
    //dynamic cast methods for Flip Flops and Inputs
    InputGate* castInput();
    OutputGate* castOutput();
    DffGate* castDff();
};

class AndGate : public Gate {
public:
    AndGate(unsigned int gid, unsigned int level) : Gate(gid, Gate::AND, level) {}
    AndGate(unsigned int gid, std::vector<Gate *> fin, std::vector<Gate *> fout)
        : Gate(gid, fin, fout, Gate::AND) {}
    ~AndGate() {};
    void evaluate();
    void faultEvaluate();
    virtual AndGate* clone() {
        return new AndGate(*this);
    }
};

class NandGate : public Gate {
public:
    NandGate(unsigned int gid, unsigned int level) : Gate(gid, Gate::NAND, level) {}
    NandGate(unsigned int gid, std::vector<Gate *> fin, std::vector<Gate *> fout)
        : Gate(gid, fin, fout, Gate::NAND) {}
    ~NandGate() {}
    void evaluate();
    void faultEvaluate();
    virtual NandGate* clone() {
        return new NandGate(*this);
    }
};

class OrGate : public Gate {
public:
    OrGate(unsigned int gid, unsigned int level) : Gate(gid, Gate::OR, level) {}
    OrGate(unsigned int gid, std::vector<Gate *> fin, std::vector<Gate *> fout)
        : Gate(gid, fin, fout, Gate::OR) {}
    ~OrGate() {}
    void evaluate();
    void faultEvaluate();
    virtual OrGate* clone() {
        return new OrGate(*this);
    }
};

class NorGate : public Gate {
public:
    NorGate(unsigned int gid, unsigned int level) : Gate(gid, Gate::NOR, level) {}
    NorGate(unsigned int gid, std::vector<Gate *> fin, std::vector<Gate *> fout)
        : Gate(gid, fin, fout, Gate::NOR) {}
    ~NorGate() {}
    void evaluate();
    void faultEvaluate();
    virtual NorGate* clone() {
        return new NorGate(*this);
    }
};

class XorGate : public Gate {
public:
    XorGate(unsigned int gid, unsigned int level) : Gate(gid, Gate::XOR, level) {}
    XorGate(unsigned int gid, std::vector<Gate *> fin, std::vector<Gate *> fout)
        : Gate(gid, fin, fout, Gate::XOR) {}
    ~XorGate() {}
    void evaluate();
    void faultEvaluate();
    virtual XorGate* clone() {
        return new XorGate(*this);
    }
};

class XnorGate : public Gate {
public:
    XnorGate(unsigned int gid, unsigned int level) : Gate(gid, Gate::XNOR, level) {}
    XnorGate(unsigned int gid, std::vector<Gate *> fin, std::vector<Gate *> fout)
        : Gate(gid, fin, fout, Gate::XNOR) {}
    ~XnorGate () {}
    void evaluate();
    void faultEvaluate();
    virtual XnorGate* clone() {
        return new XnorGate(*this);
    }
};

class NotGate : public Gate {
public:
    NotGate(unsigned int gid, unsigned int level) : Gate(gid, Gate::NOT, level) {}
    NotGate(unsigned int gid, std::vector<Gate *> fin, std::vector<Gate *> fout)
        : Gate(gid, fin, fout, Gate::NOT) {}
    ~NotGate() {}
    void evaluate();
    void faultEvaluate();
    virtual NotGate* clone() {
        return new NotGate(*this);
    }
};

class InputGate : public Gate {
public:
    InputGate(unsigned int gid, unsigned int level) : Gate(gid, Gate::INPUT, level) {}
    InputGate(unsigned int gid, std::vector<Gate *> fin, std::vector<Gate *> fout)
        : Gate(gid, fin, fout, Gate::INPUT) {}
    ~InputGate() {}
    void evaluate();
    void faultEvaluate();
    void setInput(LogicValue::VALUES);

    virtual InputGate* clone() {
        return new InputGate(*this);
    }
};

class OutputGate : public Gate {
public:
    OutputGate(unsigned int gid, unsigned int level) : Gate(gid, Gate::OUTPUT, level) {}
    OutputGate(unsigned int gid, std::vector<Gate *> fin, std::vector<Gate *> fout)
        : Gate(gid, fin, fout, Gate::OUTPUT) {}
    ~OutputGate() {}
    void evaluate();
    void faultEvaluate();
    virtual OutputGate* clone() {
        return new OutputGate(*this);
    }
};


class TieZeroGate : public Gate {
public:
    TieZeroGate(unsigned int gid, unsigned int level) : Gate(gid, Gate::TIE_ZERO, level) {}
    TieZeroGate(unsigned int gid, std::vector<Gate *> fin, std::vector<Gate *> fout)
        : Gate(gid, fin, fout, Gate::TIE_ZERO) {}
    ~TieZeroGate() {}
    void evaluate();
    void faultEvaluate() {}
    virtual TieZeroGate* clone() {
        return new TieZeroGate(*this);
    }
};

class TieOneGate : public Gate {
public:
    TieOneGate(unsigned int gid, unsigned int level) : Gate(gid, Gate::TIE_ONE, level) {}
    TieOneGate(unsigned int gid, std::vector<Gate *> fin, std::vector<Gate *> fout)
        : Gate(gid, fin, fout, Gate::TIE_ONE) {}
    ~TieOneGate() {}
    void evaluate();
    void faultEvaluate() {}
    virtual TieOneGate* clone() {
        return new TieOneGate(*this);
    }
};

class TieXGate : public Gate {
public:
    TieXGate(unsigned int gid, unsigned int level) : Gate(gid, Gate::TIE_X, level) {}
    TieXGate(unsigned int gid, std::vector<Gate *> fin, std::vector<Gate *> fout)
        : Gate(gid, fin, fout, Gate::TIE_X) {}
    ~TieXGate() {}
    void evaluate();
    void faultEvaluate() {}
    virtual TieXGate* clone() {
        return new TieXGate(*this);
    }
};

class TieZGate : public Gate {
public:
    TieZGate(unsigned int gid, unsigned int level) : Gate(gid, Gate::TIE_Z, level) {}
    TieZGate(unsigned int gid, std::vector<Gate *> fin, std::vector<Gate *> fout)
        : Gate(gid, fin, fout, Gate::TIE_Z) {}
    ~TieZGate() {}
    void evaluate();
    void faultEvaluate() {}
    virtual TieZGate* clone() {
        return new TieZGate(*this);
    }
};

class BufGate : public Gate {
public:
    BufGate(unsigned int gid, unsigned int level) : Gate(gid, Gate::BUF, level) {}
    BufGate(unsigned int gid, std::vector<Gate *> fin, std::vector<Gate *> fout)
        : Gate(gid, fin, fout, Gate::BUF) {}
    ~BufGate() {}
    void evaluate();
    void faultEvaluate();
    virtual BufGate* clone() {
        return new BufGate(*this);
    }
};

class DffGate : public Gate {
private:
    bool doneGoodSim;
public:
    DffGate(unsigned int gid, unsigned int level) : Gate(gid, Gate::D_FF, level) {doneGoodSim = false;}
    DffGate(unsigned int gid, std::vector<Gate *> fin, std::vector<Gate *> fout)
        : Gate(gid, fin, fout, Gate::D_FF) {doneGoodSim = false;}
    ~DffGate() {}
    void evaluate();
    void faultEvaluate();
    void setDff(LogicValue::VALUES);
    void injectStoredFault(Fault * flt, LogicValue val);
    virtual DffGate* clone() {
        return new DffGate(*this);
    }
};

class Mux2Gate : public Gate {
public:
    Mux2Gate(unsigned int gid, unsigned int level) : Gate(gid, Gate::MUX_2, level) {}
    Mux2Gate(unsigned int gid, std::vector<Gate *> fin, std::vector<Gate *> fout)
        : Gate(gid, fin, fout, Gate::MUX_2) {}
    ~Mux2Gate() {}
    void evaluate();
    void faultEvaluate() {}
    virtual Mux2Gate* clone() {
        return new Mux2Gate(*this);
    }
};

class TristateGate : public Gate {
public:
    TristateGate(unsigned int gid, unsigned int level) : Gate(gid, Gate::TRISTATE, level) {}
    TristateGate(unsigned int gid, std::vector<Gate *> fin, std::vector<Gate *> fout)
        : Gate(gid, fin, fout, Gate::TRISTATE) {}
    ~TristateGate() {}
    void evaluate();
    void faultEvaluate() {}
    virtual TristateGate* clone() {
        return new TristateGate(*this);
    }
};

//INLINE GATE METHODS
inline InputGate* Gate::castInput() {
    return dynamic_cast<InputGate* >(this);
}

inline DffGate* Gate::castDff() {
    return dynamic_cast<DffGate*>(this);
}

inline OutputGate* Gate::castOutput() {
    return dynamic_cast<OutputGate*>(this);
}

#endif

