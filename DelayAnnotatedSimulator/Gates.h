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
#include <iterator>
#include "Fault.h"

class Gate;
class InputGate;
class OutputGate;
class DffGate;

enum class GateType { //good for now, can expand later
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

class GateStore {
    friend Gate;
private:
    unsigned int num_gates;
    std::unique_ptr<LogicValue[]> goodVal;
    std::unique_ptr<LogicValue[]> faultVal;
    std::unique_ptr<unsigned int[]> delay;
    std::unique_ptr<Gate*[]> fanins; //all that's needed here is the idx to lookup the good/fault val
    std::unique_ptr<Gate*[]> fanouts; //want to schedule the next gate, so pointer reduces indirection, since all gates owned by circuit, this is memory safe
    std::unique_ptr<uint32_t[]> GIC_coverage_flags; //up to 5 input gates supported.
    std::unique_ptr<char[]> toggle_up;
    std::unique_ptr<char[]> toggle_down;
    
    //active fault lists
    
public:
    //no default constructor
    //waste 1 item, but it's worth it.
    GateStore(unsigned int num_gates, unsigned int num_fanouts) {
        goodVal = std::move( std::unique_ptr<LogicValue[]>(new LogicValue[num_gates+1]) );
        faultVal = std::move( std::unique_ptr<LogicValue[]>(new LogicValue[num_gates+1]) );
        delay = std::move( std::unique_ptr<unsigned int[]>(new unsigned int[num_gates+1]) );
        fanins = std::move( std::unique_ptr<Gate*[]>(new Gate*[num_fanouts]) );
        fanouts = std::move( std::unique_ptr<Gate*[]>(new Gate*[num_fanouts]) );
        GIC_coverage_flags = std::move( std::unique_ptr<uint32_t[]>(new uint32_t[num_gates+1]) );
        toggle_up = std::move( std::unique_ptr<char[]>( new char[(num_gates + 1)/8 + 1]) );
        toggle_down = std::move( std::unique_ptr<char[]>( new char(num_gates + 1)/8 + 1) );
    }
    
    //can't be copied
    GateStore(const GateStore&) = delete;
    GateStore& operator=(const GateStore&) = delete;
    
    //can be moved
    GateStore(GateStore&& rhs) {
        goodVal = std::move(rhs.goodVal);
        faultVal = std::move(rhs.faultVal);
        fanins = std::move(rhs.fanins);
        fanouts = std::move(rhs.fanouts);
        GIC_coverage_flags = std::move(rhs.GIC_coverage_flags);
        toggle_up = std::move(rhs.toggle_up);
        toggle_down = std::move(rhs.toggle_down);
    }
    
    GateStore& operator=(GateStore&& rhs) {
        goodVal = std::move(rhs.goodVal);
        faultVal = std::move(rhs.faultVal);
        delay = std::move(rhs.delay);
        fanins = std::move(rhs.fanins);
        fanouts = std::move(rhs.fanouts);
        GIC_coverage_flags = std::move(rhs.GIC_coverage_flags);
        toggle_up = std::move(rhs.toggle_up);
        toggle_down = std::move(rhs.toggle_down);
        return *this;
    }
    
    ~GateStore() {
    }
};

//Polymorphic "gate" type. Evaluate is a hot function.
//change this to be more cache friendly.
class Gate {
protected:
    //smallest to largest
    GateType type;
    uint16_t num_fanin;
    uint16_t num_fanout;
    uint32_t gate_id;
    uint32_t fanin_idx;
    uint32_t fanout_idx;
    uint32_t levelnum;
    uint32_t delay;
    GateStore* gate_store = nullptr;
    
public:
    static uint8_t coverageFlags;
    
    class const_fanin_iterator : public std::iterator<std::bidirectional_iterator_tag, unsigned int> {
    private:
        unsigned int idx;
        Gate& creator;
    public:
        typedef Gate::const_fanin_iterator self_type;
        
        const_fanin_iterator(unsigned int idx, Gate& creator) : idx(idx), creator(creator) {}
        self_type operator++() { idx++; return *this; }
        self_type operator--() { idx--; return *this; }
        const Gate*& operator*() { return (const Gate*) creator.gate_store->fanins[idx]; }
        const Gate** operator->() { return (const Gate**) &(creator.gate_store->fanins[idx]); };
        bool operator== (const self_type rhs) { return idx == rhs.idx; }
        bool operator!= (const self_type rhs) { return idx != rhs.idx; }
    };

    class const_fanout_iterator : public std::iterator<std::bidirectional_iterator_tag, Gate*> {
    private:
        unsigned int idx;
        Gate& creator;
    public:
        typedef Gate::const_fanout_iterator self_type;
        
        const_fanout_iterator(unsigned int idx, Gate& creator) : idx(idx), creator(creator) {}
        self_type operator++() { idx++; return *this; }
        self_type operator--() { idx--; return *this; }
        const Gate* operator*() { return creator.gate_store->fanouts[idx]; }
        const Gate** operator->() { return (const Gate**)&(creator.gate_store->fanouts[idx]); }
        bool operator== (const self_type rhs) { return idx == rhs.idx; }
        bool operator!= (const self_type rhs) { return idx != rhs.idx; }
    };
    
    
    
    Gate(unsigned int idx, GateStore* gs) : gate_id(idx), gate_store(gs) {}
    Gate(unsigned int idx, GateStore* gs, GateType type, unsigned int level) : gate_id(idx), gate_store(gs) {
        gate_store->level[gate_id];
    }
    
    virtual ~Gate() { }
    
    virtual bool evaluate(); //eval and schedule if transition
    
    void createGIC(){    }

    inline LogicValue getOut() { return gate_store->goodVal[gate_id]; }
    inline uint16_t getNumFanin() { return num_fanout; }
    inline uint16_t getNumFanout() { return num_fanout; }
    inline Gate* getFanin(unsigned int net) {return  (net < num_fanin) ? gate_store->fanins[fanin_idx+net] : nullptr; }
    inline Gate* getFanout(unsigned int net) {return (net < num_fanout) ? gate_store->fanouts[fanout_idx+net] : nullptr; }
    //Gate info methods
    inline unsigned int getLevel() { return levelnum; }
    inline void setLevel(unsigned int level) { levelnum = level; }
    inline unsigned int getDelay() { return delay; }
    inline void setDelay(unsigned int dly) { delay = dly; }
    inline unsigned int getId() { return gate_id; }

    //faulty gate methods
    void diverge(Fault *);
    virtual bool faultEvaluate() {}
    
    inline LogicValue getFaultyValue() {}

    inline void setGIC(){
        /*if(!calc_GIC) return;
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
        GIC_coverage[idx] = true;*/
    }
    
    inline unsigned int getGICCov(){}
    
    inline unsigned int getNumGICPts() {return (2 << getNumFanin());}
    
    inline bool toggledUp() { return false; }
    
    inline bool toggledDown() { return false; }
    //dynamic cast methods for Flip Flops and Inputs
    InputGate* castInput();
    OutputGate* castOutput();
    DffGate* castDff();
    
    //iterator methods
    const_fanin_iterator fanin_begin() { return const_fanin_iterator(fanin_idx, *this);}
    const_fanin_iterator fanin_end() { return const_fanin_iterator(fanin_idx + num_fanin, *this); }
    const_fanout_iterator fanout_begin() { return const_fanout_iterator(fanout_idx, *this); }
    const_fanout_iterator fanout_end() { return const_fanout_iterator(fanout_idx + num_fanout, *this); }
};


class AndGate : public Gate {
public:
    AndGate(unsigned int gid, GateStore* gs, unsigned int level) : Gate(gid, gs, GateType::AND, level) {}
    ~AndGate() {};
    void evaluate();
    void faultEvaluate();
    virtual AndGate* clone() {
        return new AndGate(*this);
    }
};

class NandGate : public Gate {
public:
    NandGate(unsigned int gid, GateStore* gs, unsigned int level) : Gate(gid, gs, GateType::NAND, level) {}
    ~NandGate() {}
    void evaluate();
    void faultEvaluate();
    virtual NandGate* clone() {
        return new NandGate(*this);
    }
};

class OrGate : public Gate {
public:
    OrGate(unsigned int gid, GateStore* gs, unsigned int level) : Gate(gid, gs, GateType::OR, level) {}
    ~OrGate() {}
    void evaluate();
    void faultEvaluate();
    virtual OrGate* clone() {
        return new OrGate(*this);
    }
};

class NorGate : public Gate {
public:
    NorGate(unsigned int gid, GateStore* gs, unsigned int level) : Gate(gid, gs, GateType::NOR, level) {}
    ~NorGate() {}
    void evaluate();
    void faultEvaluate();
    virtual NorGate* clone() {
        return new NorGate(*this);
    }
};

class XorGate : public Gate {
public:
    XorGate(unsigned int gid, GateStore* gs, unsigned int level) : Gate(gid, gs, GateType::XOR, level) {}
    ~XorGate() {}
    void evaluate();
    void faultEvaluate();
    virtual XorGate* clone() {
        return new XorGate(*this);
    }
};

class XnorGate : public Gate {
public:
    XnorGate(unsigned int gid, GateStore* gs, unsigned int level) : Gate(gid, gs, GateType::XNOR, level) {}
    ~XnorGate () {}
    void evaluate();
    void faultEvaluate();
    virtual XnorGate* clone() {
        return new XnorGate(*this);
    }
};

class NotGate : public Gate {
public:
    NotGate(unsigned int gid, GateStore* gs, unsigned int level) : Gate(gid, gs, GateType::NOT, level) {}
    ~NotGate() {}
    void evaluate();
    void faultEvaluate();
    virtual NotGate* clone() {
        return new NotGate(*this);
    }
};

class InputGate : public Gate {
public:
    InputGate(unsigned int gid, GateStore* gs, unsigned int level) : Gate(gid, gs, GateType::INPUT, level) {}
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
    OutputGate(unsigned int gid, GateStore* gs, unsigned int level) : Gate(gid, gs, GateType::OUTPUT, level) {}
    ~OutputGate() {}
    void evaluate();
    void faultEvaluate();
    virtual OutputGate* clone() {
        return new OutputGate(*this);
    }
};


class TieZeroGate : public Gate {
public:
    TieZeroGate(unsigned int gid, GateStore* gs, unsigned int level) : Gate(gid, gs, GateType::TIE_ZERO, level) {}
    ~TieZeroGate() {}
    void evaluate();
    void faultEvaluate() {}
    virtual TieZeroGate* clone() {
        return new TieZeroGate(*this);
    }
};

class TieOneGate : public Gate {
public:
    TieOneGate(unsigned int gid, GateStore* gs, unsigned int level) : Gate(gid, gs, GateType::TIE_ONE, level) {}
    ~TieOneGate() {}
    void evaluate();
    void faultEvaluate() {}
    virtual TieOneGate* clone() {
        return new TieOneGate(*this);
    }
};

class TieXGate : public Gate {
public:
    TieXGate(unsigned int gid, GateStore* gs, unsigned int level) : Gate(gid, gs, GateType::TIE_X, level) {}
    ~TieXGate() {}
    void evaluate();
    void faultEvaluate() {}
    virtual TieXGate* clone() {
        return new TieXGate(*this);
    }
};

class TieZGate : public Gate {
public:
    TieZGate(unsigned int gid, GateStore* gs, unsigned int level) : Gate(gid, gs, GateType::TIE_Z, level) {}
    ~TieZGate() {}
    void evaluate();
    void faultEvaluate() {}
    virtual TieZGate* clone() {
        return new TieZGate(*this);
    }
};

class BufGate : public Gate {
public:
    BufGate(unsigned int gid, GateStore* gs, unsigned int level) : Gate(gid, gs, GateType::BUF, level) {}
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
    DffGate(unsigned int gid, GateStore* gs, unsigned int level) : Gate(gid, gs, GateType::D_FF, level) {doneGoodSim = false;}
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
    Mux2Gate(unsigned int gid, GateStore* gs, unsigned int level) : Gate(gid, gs, GateType::MUX_2, level) {}
    ~Mux2Gate() {}
    void evaluate();
    void faultEvaluate() {}
    virtual Mux2Gate* clone() {
        return new Mux2Gate(*this);
    }
};

class TristateGate : public Gate {
public:
    TristateGate(unsigned int gid, GateStore* gs, unsigned int level) : Gate(gid, gs, GateType::TRISTATE, level) {}
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

