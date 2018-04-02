#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

#include <stdio.h>
#include <string>
#include <list>
#include <map>
#include <cmath>
#include "ast.h"

using namespace std;

class NumExpression : public Expression{
public:
  NumExpression(int val){
    value = val;
  }

  int getType(){ return NumExp; }
  int value = 0;
};

class BoolExpression : public Expression{
public:
  BoolExpression(bool val){
    value = val;
  }

  int getType(){ return BoolExp; }
  bool value = false;
};

class IdExpression : public Expression {
public:
  IdExpression(char* idName){
    varName = varName.append( idName );
  }

  int getType(){ return IdExp; }

  std::string varName;
};

class ArrayAccessExpression : public Expression {
public:
  ArrayAccessExpression(char* arrName, Expression* index){
    varName = varName.append( arrName );
    indexExpression = index;
  }

  int getType(){ return ArrAccExp; }

  Expression* indexExpression;
  std::string varName;
};

class FuncCallExpression : public Expression {
public:
  FuncCallExpression(char* fName, ExpressionList* args){
    funcName = funcName.append( fName );
    arguments = args;
  }

  int getType(){ return FCallExp; }

  ExpressionList* arguments;
  std::string funcName;
};

class StrExpression : public Expression{
public:
  StrExpression(char* val){
    strValue = strValue.append( val );
  }

  int getType(){ return StrExp; }

  std::string strValue;
};

class BitNotExpression : public Expression{
public:
  BitNotExpression(Expression* exp){
    targetExpression = exp;
  }

  int getType(){ return BitNotExp; }

  Expression* targetExpression;
};

class LogNotExpression : public Expression{
public:
  LogNotExpression(Expression* exp){
    targetExpression = exp;
  }

  int getType(){ return NotExp; }

  Expression* targetExpression;
};

class BinaryExpression : public Expression{
public:
  BinaryExpression(Expression* left, Expression* right){
    leftSide = left;
    rightSide = right;
  }
  Expression* leftSide;
  Expression* rightSide;
};

class AddExpression : public BinaryExpression {
public:
  AddExpression(Expression* left, Expression* right): BinaryExpression(left, right){}
  int getType(){ return AddExp; }
};

class SubExpression : public BinaryExpression {
public:
  SubExpression(Expression* left, Expression* right): BinaryExpression(left, right){}
  int getType(){ return SubExp; }
};


class ShiftLeftExpression : public BinaryExpression {
public:
  ShiftLeftExpression(Expression* left, Expression* right): BinaryExpression(left, right){}
  int getType(){ return ShlExp; }
};

class ShiftRightExpression : public BinaryExpression {
public:
  ShiftRightExpression(Expression* left, Expression* right): BinaryExpression(left, right){}
  int getType(){ return ShrExp; }
};

class BorExpression : public BinaryExpression {
public:
  BorExpression(Expression* left, Expression* right): BinaryExpression(left, right){}
  int getType(){ return BorExp; }
};

class XorExpression : public BinaryExpression {
public:
  XorExpression(Expression* left, Expression* right): BinaryExpression(left, right){}
  int getType(){ return XorExp; }
};

class DivExpression : public BinaryExpression {
public:
  DivExpression(Expression* left, Expression* right): BinaryExpression(left, right){}
  int getType(){ return DivExp; }
};

class ModExpression : public BinaryExpression {
public:
  ModExpression(Expression* left, Expression* right): BinaryExpression(left, right){}
  int getType(){ return ModExp; }
};

class MulExpression : public BinaryExpression {
public:
  MulExpression(Expression* left, Expression* right): BinaryExpression(left, right){}
  int getType(){ return MulExp; }
};

class BandExpression : public BinaryExpression {
public:
  BandExpression(Expression* left, Expression* right): BinaryExpression(left, right){}
  int getType(){ return BandExp; }
};

class PowExpression : public BinaryExpression {
public:
  PowExpression(Expression* left, Expression* right): BinaryExpression(left, right){}
  int getType(){ return PowExp; }
};

class EquExpression: public BinaryExpression {
public:
  EquExpression(Expression* left, Expression* right): BinaryExpression(left, right){}
  int getType(){ return EquExp; }
};

class NequExpression: public BinaryExpression {
public:
  NequExpression(Expression* left, Expression* right): BinaryExpression(left, right){}
  int getType(){ return NeqExp; }
};

class LthanExpression: public BinaryExpression {
public:
  LthanExpression(Expression* left, Expression* right): BinaryExpression(left, right){}
  int getType(){ return LthExp; }
};

class GthanExpression: public BinaryExpression {
public:
  GthanExpression(Expression* left, Expression* right): BinaryExpression(left, right){}
  int getType(){ return GthExp; }
};

class GeqExpression: public BinaryExpression {
public:
  GeqExpression(Expression* left, Expression* right): BinaryExpression(left, right){}
  int getType(){ return GeqExp; }
};

class LeqExpression: public BinaryExpression {
public:
  LeqExpression(Expression* left, Expression* right): BinaryExpression(left, right){}
  int getType(){ return LeqExp; }
};

class LorExpression: public BinaryExpression {
public:
  LorExpression(Expression* left, Expression* right): BinaryExpression(left, right){}
  int getType(){ return LorExp; }
};

class LandExpression: public BinaryExpression {
public:
  LandExpression(Expression* left, Expression* right): BinaryExpression(left, right){}
  int getType(){ return LandExp; }
};

#endif
