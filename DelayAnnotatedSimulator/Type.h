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


class LogicIDGenerator {
private:
    static unsigned int ids;
public:
    static unsigned int getNewID() {int ret = ids; ids += 2; return ret;}
    static void reset() {ids = 0;}
};

class LogicValue{
public:
    enum VALUES{
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
    unsigned int x_id;
    
    LogicValue(VALUES val): val(val) {} //don't need IDs until scheduled;
    operator VALUES () { return val; }
    LogicValue operator= (LogicValue::VALUES rhs) { this->val = rhs; return *this; }
    bool operator== (LogicValue::VALUES rhs) {return (val == rhs);}
    bool operator== (LogicValue rhs) {return val == rhs.val;}
    bool operator!= (LogicValue::VALUES rhs) {return (val != rhs);}

    inline unsigned int getID(){ return ( val == X ) ? x_id : -1; }
    inline void setID(int ident) { x_id = ident; }
    inline void newID() { x_id = LogicIDGenerator::getNewID(); }
    inline void invertID() { x_id = ((x_id % 2) == 0) ? x_id + 1  : x_id - 1; }
    
    

    static LogicValue fromChar(char c) {
        if (c == '0')  return LogicValue(ZERO);
        if (c == '1')  return LogicValue(ONE);
        if (c == 'x' || c == 'X') return LogicValue(X);
        if (c == 'z' || c == 'Z') return LogicValue(X);
        return LogicValue(X); }
};

inline LogicValue operator& (LogicValue lhs, LogicValue rhs) {
    bool rhs_z = (rhs.val == LogicValue::Z);
    bool lhs_z = (lhs.val == LogicValue::Z);
    
    if(rhs_z && lhs_z) return LogicValue(LogicValue::X);
    if(rhs_z) return lhs;
    if(lhs_z) return rhs;
    
    return LogicValue(LogicValue::VALUES(lhs.val & rhs.val));
}

inline LogicValue operator| (LogicValue lhs, LogicValue rhs) {
    bool rhs_z = (rhs.val == LogicValue::Z);
    bool lhs_z = (lhs.val == LogicValue::Z);
    
    if(rhs_z && lhs_z) return LogicValue(LogicValue::X);
    if(rhs_z) return lhs;
    if(lhs_z) return rhs;
    
    return LogicValue(LogicValue::VALUES(lhs.val | rhs.val));
}

inline LogicValue operator^ (LogicValue lhs, LogicValue rhs) {
    bool rhs_z = (rhs.val == LogicValue::Z);
    bool lhs_z = (lhs.val == LogicValue::Z);
    
    if(rhs_z && lhs_z) return LogicValue(LogicValue::X);
    if(rhs_z) return lhs;
    if(lhs_z) return rhs;
    
    return LogicValue(LogicValue::VALUES((lhs.val == LogicValue::X || rhs.val == LogicValue::X) ? LogicValue::X : lhs.val ^ rhs.val));
}

inline LogicValue operator~ (LogicValue lhs) {
    if(lhs.val == LogicValue::Z) return LogicValue(LogicValue::Z);
    return LogicValue((lhs.val == LogicValue::X) ? LogicValue::X : LogicValue::VALUES(LogicValue::ONE - lhs.val));
}

#endif
