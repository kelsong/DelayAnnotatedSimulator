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

#define NUM_FAULT_INJECT 16

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
