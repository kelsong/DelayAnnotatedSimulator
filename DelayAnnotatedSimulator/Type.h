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

//want logic to be analogous

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
    
    LogicValue(VALUES val): val(val) {}
    operator VALUES () { return val; }
    LogicValue operator= (LogicValue::VALUES rhs) { this->val = rhs; return *this; }
    bool operator== (LogicValue::VALUES rhs) {return (val == rhs);}
    bool operator== (LogicValue rhs) {return val == rhs.val;}
    
    static LogicValue fromChar(char c) {
        if (c == '0')  return LogicValue(ZERO);
        if (c == '1')  return LogicValue(ONE);
        if (c == 'x' || c == 'X') return LogicValue(X);
        if (c == 'z' || c == 'Z') return LogicValue(X);
        return LogicValue(X); }
};

#endif
