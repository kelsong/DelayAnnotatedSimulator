//these are the unit tests for the logic simulator

#include <iostream>
#include "Gates.h"
#include "Type.h"
#include "Circuit.h"

#define TEST_FAIL 0
#define TEST_PASS 1
//////////////////////////////////////////////////
// Gate Unit Tests
//////////////////////////////////////////////////
std::vector<LogicValue> two_bit_init_arr1 {LogicValue::ZERO, LogicValue::ZERO, LogicValue::ZERO, 
                                           LogicValue::ONE,  LogicValue::ONE, LogicValue::ONE, 
                                           LogicValue::X, LogicValue::X, LogicValue::X};

std::vector<LogicValue> two_bit_init_arr2 {LogicValue::ZERO, LogicValue::ONE, LogicValue::X, 
                                           LogicValue::ZERO, LogicValue::ONE, LogicValue::X,
                                           LogicValue::ZERO, LogicValue::ONE, LogicValue::X};

unsigned int TestAnd(){
  //setup
  Gate * andGate = new AndGate(3, 5);
  InputGate * inputA = new InputGate(1, 0);
  InputGate * inputB = new InputGate(2, 0);
  
  andGate->addFanin(inputA);
  andGate->addFanin(inputB);
  
  std::vector<LogicValue> exp_results {LogicValue::ZERO, LogicValue::ZERO, LogicValue::ZERO, 
                                       LogicValue::ZERO, LogicValue::ONE, LogicValue::X,
                                       LogicValue::ZERO, LogicValue::X, LogicValue::X};

  //test
  for(int i = 0; i<9; i++){
    inputA->setInput(two_bit_init_arr1[i]);
    inputB->setInput(two_bit_init_arr2[i]);
    inputA->evaluate();
    inputB->evaluate();
    
    andGate->evaluate();
    if(andGate->getOut() != exp_results[i] ){
      std::cerr << "INPUT A: " << inputA->getOut().ascii() << " INPUT B: " << inputB->getOut().ascii() << " ";
      std::cerr << "FAIL. Observed: " << andGate->getOut().ascii() << " Expected: " << exp_results[i].ascii() << std::endl;
      return TEST_FAIL;
    }
  }
  
  //teardown
  delete andGate;
  delete inputA;
  delete inputB;
  return TEST_PASS;
}

unsigned int TestNand(){
  Gate * nandGate = new NandGate(3, 5);
  InputGate * inputA = new InputGate(1, 0);
  InputGate * inputB = new InputGate(2, 0);
  
  nandGate->addFanin(inputA);
  nandGate->addFanin(inputB);
  
  std::vector<LogicValue> exp_results {LogicValue::ONE, LogicValue::ONE, LogicValue::ONE, 
                                       LogicValue::ONE, LogicValue::ZERO, LogicValue::X,
                                       LogicValue::ONE, LogicValue::X, LogicValue::X};

  //test
  for(int i = 0; i<9; i++){
    inputA->setInput(two_bit_init_arr1[i]);
    inputB->setInput(two_bit_init_arr2[i]);
    inputA->evaluate();
    inputB->evaluate();
    
    nandGate->evaluate();
    if(nandGate->getOut() != exp_results[i] ){
      std::cerr << "INPUT A: " << inputA->getOut().ascii() << " INPUT B: " << inputB->getOut().ascii() << " ";
      std::cerr << "FAIL. Observed: " << nandGate->getOut().ascii() << " Expected: " << exp_results[i].ascii() << std::endl;
      return TEST_FAIL;
    }
  }
  
  //teardown
  delete nandGate;
  delete inputA;
  delete inputB;
  return TEST_PASS;
}

unsigned int TestOr(){
  Gate * orGate = new OrGate(3, 5);
  InputGate * inputA = new InputGate(1, 0);
  InputGate * inputB = new InputGate(2, 0);
  
  orGate->addFanin(inputA);
  orGate->addFanin(inputB);
  
  std::vector<LogicValue> exp_results {LogicValue::ZERO, LogicValue::ONE, LogicValue::X, 
                                       LogicValue::ONE, LogicValue::ONE, LogicValue::ONE,
                                       LogicValue::X, LogicValue::ONE, LogicValue::X};

  //test
  for(int i = 0; i<9; i++){
    inputA->setInput(two_bit_init_arr1[i]);
    inputB->setInput(two_bit_init_arr2[i]);
    inputA->evaluate();
    inputB->evaluate();
    
    orGate->evaluate();
    if(orGate->getOut() != exp_results[i] ){
      std::cerr << "INPUT A: " << inputA->getOut() << " INPUT B: " << inputB->getOut() << " ";
      std::cerr << "FAIL. Observed: " << orGate->getOut().ascii() << " Expected: " << exp_results[i].ascii() << std::endl;
      return TEST_FAIL;
    }
  }
  
  //teardown
  delete orGate;
  delete inputA;
  delete inputB;
  return TEST_PASS;
}

unsigned int TestNor(){
  Gate * norGate = new NorGate(3, 5);
  InputGate * inputA = new InputGate(1, 0);
  InputGate * inputB = new InputGate(2, 0);
  
  norGate->addFanin(inputA);
  norGate->addFanin(inputB);
  
  std::vector<LogicValue> exp_results {LogicValue::ONE, LogicValue::ZERO, LogicValue::X, 
                                       LogicValue::ZERO, LogicValue::ZERO, LogicValue::ZERO,
                                       LogicValue::X, LogicValue::ZERO, LogicValue::X};

  //test
  for(int i = 0; i<9; i++){
    inputA->setInput(two_bit_init_arr1[i]);
    inputB->setInput(two_bit_init_arr2[i]);
    inputA->evaluate();
    inputB->evaluate();
    
    norGate->evaluate();
    if(norGate->getOut() != exp_results[i] ){
      std::cerr << "INPUT A: " << inputA->getOut() << " INPUT B: " << inputB->getOut() << " ";
      std::cerr << "FAIL. Observed: " << norGate->getOut().ascii() << " Expected: " << exp_results[i].ascii() << std::endl;
      return TEST_FAIL;
    }
  }
  
  //teardown
  delete norGate;
  delete inputA;
  delete inputB;
  return TEST_PASS;
}

unsigned int TestXor(){
  Gate * xorGate = new XorGate(3, 5);
  InputGate * inputA = new InputGate(1, 0);
  InputGate * inputB = new InputGate(2, 0);
  
  xorGate->addFanin(inputA);
  xorGate->addFanin(inputB);
  
  std::vector<LogicValue> exp_results {LogicValue::ZERO, LogicValue::ONE, LogicValue::X, 
                                       LogicValue::ONE, LogicValue::ZERO, LogicValue::X,
                                       LogicValue::X, LogicValue::X, LogicValue::X};

  //test
  for(int i = 0; i<9; i++){
    inputA->setInput(two_bit_init_arr1[i]);
    inputB->setInput(two_bit_init_arr2[i]);
    inputA->evaluate();
    inputB->evaluate();
    
    xorGate->evaluate();
    if(xorGate->getOut() != exp_results[i] ){
      std::cerr << "INPUT A: " << inputA->getOut() << " INPUT B: " << inputB->getOut() << " ";
      std::cerr << "FAIL. Observed: " << xorGate->getOut().ascii() << " Expected: " << exp_results[i].ascii() << std::endl;
      return TEST_FAIL;
    }
  }
  
  //teardown
  delete xorGate;
  delete inputA;
  delete inputB;
  return TEST_PASS;
}

unsigned int TestXNor(){
  Gate * xnorGate = new XnorGate(3, 5);
  InputGate * inputA = new InputGate(1, 0);
  InputGate * inputB = new InputGate(2, 0);
  
  xnorGate->addFanin(inputA);
  xnorGate->addFanin(inputB);
  
  std::vector<LogicValue> exp_results {LogicValue::ONE, LogicValue::ZERO, LogicValue::X, 
                                       LogicValue::ZERO, LogicValue::ONE, LogicValue::X,
                                       LogicValue::X, LogicValue::X, LogicValue::X};

  //test
  for(int i = 0; i<9; i++){
    inputA->setInput(two_bit_init_arr1[i]);
    inputB->setInput(two_bit_init_arr2[i]);
    inputA->evaluate();
    inputB->evaluate();
    
    xnorGate->evaluate();
    if(xnorGate->getOut() != exp_results[i] ){
      std::cerr << "INPUT A: " << inputA->getOut() << " INPUT B: " << inputB->getOut() << " ";
      std::cerr << "FAIL. Observed: " << xnorGate->getOut().ascii() << " Expected: " << exp_results[i].ascii() << std::endl;
      return TEST_FAIL;
    }
  }
  
  //teardown
  delete xnorGate;
  delete inputA;
  delete inputB;
  return TEST_PASS;
}

unsigned int TestBuf(){
  Gate * bufGate = new BufGate(2, 5);
  InputGate * inputA = new InputGate(1, 0);
  bufGate->addFanin(inputA);

  std::vector<LogicValue> exp_results {LogicValue::ONE, LogicValue::ZERO, LogicValue::X};
  
  //test
  for(int i = 0; i<exp_results.size(); i++){
    inputA->setInput(exp_results[i]);
    inputA->evaluate();
    bufGate->evaluate();
    if(bufGate->getOut() != exp_results[i]){
      return TEST_FAIL;
    }
  }
  
  delete bufGate;
  delete inputA;
  return TEST_PASS;
}

unsigned int TestNot(){
  Gate * notGate = new NotGate(2, 5);
  InputGate * inputA = new InputGate(1, 0);
  notGate->addFanin(inputA);

  std::vector<LogicValue> input_values {LogicValue::ZERO, LogicValue::ONE, LogicValue::X};
  std::vector<LogicValue> exp_results {LogicValue::ONE, LogicValue::ZERO, LogicValue::X};
  
  //test
  for(int i = 0; i<exp_results.size(); i++){
    inputA->setInput(input_values[i]);
    inputA->evaluate();
    notGate->evaluate();
    if(notGate->getOut() != exp_results[i]){
      return TEST_FAIL;
    }
  }
  
  delete notGate;
  delete inputA;
  return TEST_PASS;
}

unsigned int TestInput(){
  InputGate * inputA = new InputGate(1, 0);

  std::vector<LogicValue> exp_results {LogicValue::ONE, LogicValue::ZERO, LogicValue::X};
  
  //test
  for(int i = 0; i<exp_results.size(); i++){
    inputA->setInput(exp_results[i]);
    inputA->evaluate();
    if(inputA->getOut() != exp_results[i]){
      return TEST_FAIL;
    }
  }
  
  delete inputA;
  return TEST_PASS;
}

unsigned int TestOutput(){
  Gate * bufGate = new OutputGate(2, 5);
  InputGate * inputA = new InputGate(1, 0);
  bufGate->addFanin(inputA);

  std::vector<LogicValue> exp_results {LogicValue::ONE, LogicValue::ZERO, LogicValue::X};
  
  //test
  for(int i = 0; i<exp_results.size(); i++){
    inputA->setInput(exp_results[i]);
    inputA->evaluate();
    bufGate->evaluate();
    if(bufGate->getOut() != exp_results[i]){
      return TEST_FAIL;
    }
  }
  
  delete bufGate;
  delete inputA;
  return TEST_PASS;
}

unsigned int TestDFF(){
  Gate * bufGate = new DffGate(2, 5);
  InputGate * inputA = new InputGate(1, 0);
  bufGate->addFanin(inputA);

  std::vector<LogicValue> exp_results {LogicValue::ONE, LogicValue::ZERO, LogicValue::X};
  
  //test
  for(int i = 0; i<exp_results.size(); i++){
    inputA->setInput(exp_results[i]);
    inputA->evaluate();
    bufGate->evaluate();
    if(bufGate->getOut() != exp_results[i]){
      return TEST_FAIL;
    }
  }
  
  delete bufGate;
  delete inputA;
  return TEST_PASS;
}

unsigned int TestCircuit(){
    Circuit * test = new Circuit("b01rst", false, true);
    if(test->numFaults() != 135) {
        std::cerr << "NUM FAULTS: " << test->numFaults() << std::endl;
        test->printFaults();
        return TEST_FAIL;
    }
    return TEST_PASS;
}
/*
int main(){
  std::cerr << TestAnd() << std::endl;
  std::cerr << TestNand() << std::endl;
  std::cerr << TestOr() << std::endl;
  std::cerr << TestNor() << std::endl;
  std::cerr << TestNot() << std::endl;
  std::cerr << TestBuf() << std::endl;
  std::cerr << TestXor() << std::endl;
  std::cerr << TestXNor() << std::endl;
  std::cerr << TestDFF() << std::endl;
  std::cerr << TestInput() << std::endl;
  std::cerr << TestOutput() << std::endl;
  std::cerr << TestCircuit() << std::endl;
    getchar();
}*/
