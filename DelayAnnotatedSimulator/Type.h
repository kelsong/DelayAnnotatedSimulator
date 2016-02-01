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

#ifndef DelayAnnotatedSimulator_Type_h
#define DelayAnnotatedSimulator_Type_h

#define NUM_FAULT_INJECT 64
#define ALL_ONES 0xFFFFFFFFFFFFFFFF
#define ALL_ZERO 0

#include <algorithm>
#include <iostream>

class LogicValue {
public:
    enum VALUES {
        ZERO = 0,
        X,
        Z,
        ONE
    };

    const char* ascii() const {
        static const char* names[] = {"0", "X", "Z", "1"};
        return names[val];
    }

    enum VALUES val;
    
    LogicValue() : val(X) {}
    LogicValue(VALUES val): val(val) { } //don't need IDs until scheduled;
    operator VALUES () {
        return val;
    }
    LogicValue& operator= (LogicValue::VALUES rhs) {
        this->val = rhs;
        return *this;
    }
    LogicValue& operator= (LogicValue rhs) {
        this->val = rhs.val;
        return *this;
    }
    
    inline bool operator== (LogicValue::VALUES rhs) {
        return (val == rhs);
    }
    inline bool operator== (LogicValue rhs) {
        return val == rhs.val;
    }
    inline bool operator!= (LogicValue::VALUES rhs) {
        return (val != rhs);
    }
    inline bool operator!= (LogicValue rhs){
        return (val != rhs.val);
    }
    LogicValue& operator&= (LogicValue rhs);
    LogicValue& operator|= (LogicValue rhs);
    LogicValue& operator^= (LogicValue rhs);

    static LogicValue fromChar(char c) {
        if (c == '0')  return LogicValue(ZERO);
        if (c == '1')  return LogicValue(ONE);
        if (c == 'x' || c == 'X') return LogicValue(X);
        return LogicValue(X);
    }
};

inline LogicValue operator& (LogicValue lhs, LogicValue rhs) {
    return LogicValue(LogicValue::VALUES(lhs.val & rhs.val));
}

inline LogicValue operator| (LogicValue lhs, LogicValue rhs) {
    return LogicValue(LogicValue::VALUES(lhs.val | rhs.val));
}

inline LogicValue operator^ (LogicValue lhs, LogicValue rhs) {
    return LogicValue(LogicValue::VALUES((lhs.val == LogicValue::X || rhs.val == LogicValue::X) ? LogicValue::X : lhs.val ^ rhs.val));
}

inline LogicValue operator~ (LogicValue lhs) {
    return LogicValue((lhs.val == LogicValue::X) ? LogicValue::X : LogicValue::VALUES(LogicValue::ONE - lhs.val));
}

inline LogicValue& LogicValue::operator&= (LogicValue rhs) {
    *this = *this & rhs;
    return *this;
}
inline LogicValue& LogicValue::operator|= (LogicValue rhs) {
    *this = *this | rhs;
    return *this;
}
inline LogicValue& LogicValue::operator^= (LogicValue rhs) {
    *this = *this ^ rhs;
    return *this;
}

class FaultValue {
private:
    unsigned long fval1; //packed 2 bit representation up to 64 injections
    unsigned long fval2;
public:
    FaultValue() : fval1(ALL_ZERO), fval2(ALL_ZERO) {}
    FaultValue(unsigned long fval1, unsigned long fval2) : fval1(fval1), fval2(fval2) {}
    FaultValue(const FaultValue& copy){
        fval2 = copy.fval2;
        fval1 = copy.fval1;
    }

    FaultValue& operator= (FaultValue rhs){
        this->fval1 = rhs.fval1;
        return *this;
    }
    
    inline void setAllLogicValue(LogicValue value) {
        fval1 = (value == LogicValue::X || value == LogicValue::ZERO ) ? ALL_ZERO : ALL_ONES;
        fval2 = (value == LogicValue::Z || value == LogicValue::ZERO ) ? ALL_ZERO : ALL_ONES;
    }
    
    inline bool compareLogicVal(LogicValue value) { //true if
        switch(value) {
            case LogicValue::ZERO:
                return (fval1 == ALL_ZERO && fval2 == ALL_ZERO);
                break;
            case LogicValue::X:
                return (fval1 == ALL_ZERO && fval2 == ALL_ONES);
                break;
            case LogicValue::Z:
                return (fval1 == ALL_ONES && fval2 == ALL_ZERO);
                break;
            case LogicValue::ONE:
                return (fval1 == ALL_ONES && fval2 == ALL_ONES);
                break;
        }
    }
    
    LogicValue getFaultyVal(unsigned int idx) {
        if(idx >= NUM_FAULT_INJECT){ std::cerr << "ERROR: invalid fault set\n"; exit(700); }
        
        unsigned long mask = 0x1 << idx;
        unsigned long val1 = fval1 & mask;
        unsigned long val2 = fval2 & mask;
        if(val1 == val2) {
            return (val1 == 0) ? LogicValue::ZERO: LogicValue::ONE;
        } else {
            return (val1 == 0) ? LogicValue::X: LogicValue::Z;
        }
    }
    
    inline void setValue(LogicValue value, unsigned int idx) {
        if(idx > NUM_FAULT_INJECT) { std::cerr << "ERROR: invalid fault set\n"; exit(700); }
        unsigned long mask = 0x1;
        switch (value) {
            case LogicValue::ZERO:
                mask = ~(mask << idx);
                fval1 &= mask; //set individual bits to zero
                fval2 &= mask;
                break;
            case LogicValue::X:
                mask = mask << idx;
                fval2 |= mask;
                mask = ~mask;
                fval1 &= mask;
                break;
            case LogicValue::Z:
                mask = mask << idx;
                fval1 |= mask;
                mask = ~mask;
                fval2 &= mask;
                break;
            case LogicValue::ONE:
                mask = mask << idx;
                fval1 |= mask;
                fval2 |= mask;
                break;
        }
    }
    
    inline FaultValue operator& (FaultValue rhs) {
        return FaultValue((this->fval1 & rhs.fval1), (this->fval2 & rhs.fval2));
    }
    
    inline FaultValue operator| (FaultValue rhs) {
        return FaultValue((this->fval1 | rhs.fval1), (this->fval2 | rhs.fval2));
    }
    
    inline FaultValue operator^ (FaultValue rhs) {
        unsigned long val1 = (rhs.fval1 & this->fval1 & (~rhs.fval2) & (~this->fval2)) |
                             ((~rhs.fval1) & (~this->fval1) & rhs.fval2 & this->fval2);
        unsigned long val2 = ~((rhs.fval1 & this->fval1 & rhs.fval2 & this->fval2) |
                               (~rhs.fval1 & ~this->fval1 & ~rhs.fval2 & ~this->fval2));
        return FaultValue(val1, val2);
    }
    
    inline FaultValue operator~ () {
        unsigned long val1 = ~fval2;
        unsigned long val2 = ~fval1;
        return FaultValue(val1, val2);
    }
    
    FaultValue& operator&= (FaultValue rhs);
    FaultValue& operator|= (FaultValue rhs);
    FaultValue& operator^= (FaultValue rhs);
};

inline FaultValue& FaultValue::operator&= (FaultValue rhs) {
    *this = *this & rhs;
    return *this;
}
inline FaultValue& FaultValue::operator|= (FaultValue rhs) {
    *this = *this | rhs;
    return *this;
}
inline FaultValue& FaultValue::operator^= (FaultValue rhs) {
    *this = *this ^ rhs;
    return *this;
}

inline unsigned int pow2uint(unsigned int pow){
    if(pow >= 32) return 0;
    unsigned int operand = 0x01;
    for(unsigned int i = 0; i < pow; i++){
        operand = operand << 1;
    }    
    return operand;
}

//Some useful data structures for optimization

//Linked list for fast faulty state lookup
template <class T, class W>
class FastDataPairStack{
private:
    T* typeA = NULL;
    W* typeB = NULL;
    int top;
    unsigned int alloc_size;
public:
    FastDataPairStack() {
        //std::cerr << "create data pair stack " << this << "\n";
        typeA = new T[10];
        typeB = new W[10];
        top = -1;
        alloc_size = 10;
    }
    FastDataPairStack(unsigned int size) {
        typeA = new T[size];
        typeB = new W[size];
        top = -1;
        alloc_size = size;
    }
    
    FastDataPairStack(const FastDataPairStack<T,W>& copy){
        typeA = new T[copy.alloc_size];
        typeB = new W[copy.alloc_size];
        std::copy(copy.typeA, copy.typeA + copy.top+1, typeA);
        std::copy(copy.typeB, copy.typeB + copy.top+1, typeB);
        top = copy.top;
        alloc_size = copy.alloc_size;
    }
    
    inline bool isEmpty(){
        return (top == -1);
    }
    
    inline void pop(T& valueA, W& valueB){
        if(isEmpty()){
            return;
        }
        valueA = typeA[top];
        valueB = typeB[top];
        top--;
    }
    
    inline void push(T gateIdx, W value){
        if(top == (alloc_size - 1) ){
            //std::cerr << "Doubling stored space was size " << alloc_size << "\n";
            T * tmp_A = new T[alloc_size * 2];
            W * tmp_B = new W[alloc_size * 2];
            std::copy(typeA, typeA + alloc_size, tmp_A);
            std::copy(typeB, typeB + alloc_size, tmp_B);
            delete typeA;
            delete typeB;
            
            typeA = tmp_A;
            typeB = tmp_B;
            alloc_size = alloc_size * 2;
        }
        
        top++;
        typeA[top] = gateIdx;
        typeB[top] = value;
    }
    ~FastDataPairStack(){
        //std::cerr << "delete data pair stack " << this << "\n";
        if(typeA)
            delete [] typeA;
        if(typeB)
            delete [] typeB;
    }
};

#endif
