#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <string>
#include <list>
#include <cmath>

enum StatemetTypes{
  WhileStm,
  ForStm,
  IfStm,
  AssiStm,
  ArrAssiStm,
  VarDeclStm,
  ArrDeclStm,
  FuncDeclStm,
  ExprStm,
  PrintStm,

  StmListStm,
  PassStm,
  ContStm,
  BrkStm,
  RetStm,

  ScVarDeclStm,
  ArVarDeclStm
};

enum TypeNames{
  IntType,
  BoolType,
  ArrayIntType,
  ArrayBoolType
};

enum ExpressionTypes{
  NumExp,
  IdExp,
  ArrAccExp,
  FCallExp,
  BoolExp,

  DivExp,
  ModExp,
  MulExp,
  BandExp,

  LorExp,
  LandExp,

  AddExp,
  SubExp,
  ShlExp,
  ShrExp,
  BorExp,
  XorExp,

  PowExp,
  NotExp,
  BitNotExp,
  StrExp,

  LthExp,
  GthExp,
  LeqExp,
  GeqExp,
  NeqExp,
  EquExp
};

using namespace std;

class Expression {
public:
  virtual int getType() = 0;
};

class ExpressionList {
public:
  ExpressionList(){ }
  void AddNew(Expression* exp){
    expressions.push_front(exp);
  }

  std::list<Expression*> expressions;
};

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

class Statement {
public:
  virtual int getType() = 0;
};

class StatementList : public Statement{
public:
  StatementList(){ }

  void AddNew(Statement* stm){
    statements.push_front(stm);
  }

  void Print(string indent);

  int getType(){ return StmListStm; }

  std::list<Statement*> statements;
};

class AssignStatement: public Statement {
public:
	AssignStatement(char* varName, Expression *expr) {
		this->varName = this->varName.append(varName);
		this->valueExpression = expr;
	}

  int getType() { return AssiStm; };

	string varName;
	Expression *valueExpression;
};

class ArrayItemAssignStatement: public Statement {
public:
	ArrayItemAssignStatement(char* varName, Expression* index, Expression *expr) {
    this->indexExpression = indexExpression;
		this->varName = this->varName.append(varName);
		this->valueExpression = expr;
	}

  int getType() { return ArrAssiStm; };

	string varName;
  Expression* indexExpression;
	Expression *valueExpression;
};

class PrintStatement: public Statement {
public:
	PrintStatement(ExpressionList *expr, bool isLn) {
		this->expressionList = expr;
    withNewLine = isLn;
	}

  int getType() { return PrintStm; };

	ExpressionList* expressionList;
  bool withNewLine = false;
};

class IfStatement: public Statement {
public:
	IfStatement(Expression* expr, StatementList* st_true, StatementList* st_false) {
		this->condition = expr;
		this->trueStatements = st_true;
		this->falseStatements = st_false;
	}

  void Print(string indent);

  int getType() { return IfStm; };

	Expression* condition;
	StatementList* trueStatements;
	StatementList* falseStatements;
};

class ForStatement: public Statement {
public:
	ForStatement(char* varName, Expression* minExpr, Expression* maxExpr, StatementList* st) {
    this->varName = this->varName.append(varName);
		this->minExpression = minExpr;
    this->maxExpression = maxExpr;
		this->statements = st;
	}

  void Print(string indent);
  int getType() { return ForStm; };

  string varName;

	Expression* minExpression;
  Expression* maxExpression;

	StatementList* statements;
};

class WhileStatement: public Statement {
public:
	WhileStatement(Expression* expr, StatementList* st) {
		this->condition = expr;
		this->statements = st;
	}

  void Print(string indent);

  int getType() { return WhileStm; };

	Expression* condition;
	StatementList* statements;
};


class ObjectType{
public:
  ObjectType(int code){
    typeCode = code;
  }

  int typeCode;
};

class VarDeclStatement: public Statement {
public:
  VarDeclStatement(char* name, ObjectType* type) {
    varName = varName.append(name);
    varType = type;
  }

  void SetVarName(char* newName){
    varName = "";
    varName = varName.append(newName);
  }

  ObjectType* varType;
  string varName;
};

class ScalarVarDeclStatement: public VarDeclStatement{
public:
  ScalarVarDeclStatement(char* name, ObjectType* type, Expression* value) : VarDeclStatement(name, type){
    valueExpression = value;
  }

  int getType(){ return ScVarDeclStm; }

  Expression* valueExpression;
};

class ArrayVarDeclStatement: public VarDeclStatement{
public:
  ArrayVarDeclStatement(char* name, ObjectType* type, ExpressionList* value): VarDeclStatement(name, type){
    values = value;
  }

  int getType(){ return ArVarDeclStm; }

  ExpressionList* values;
};


class FuncParam{
public:
  FuncParam(char* name, ObjectType* type){
    paramName = paramName.append(name);
    paramType = type;
  }

  string paramName;
  ObjectType* paramType;
};

class ParamList {
public:
  ParamList(){}

  void AddNew(FuncParam* param){
    paramList.push_front(param);
  }

  list<FuncParam*> paramList;
};

class FuncDeclStatement : public Statement {
public:
  FuncDeclStatement(char* name, ParamList* params, ObjectType* type, StatementList* statements){
    funcName = funcName.append(name);
    this->params = params;
    this->returnType = type;
    this->statements = statements;
  }

  void Print(string indent);

  int getType(){ return FuncDeclStm; }

  string funcName;
  ObjectType* returnType;
  ParamList* params;
  StatementList* statements;
};

class PassStatement : public Statement {
public:
  PassStatement(){ }

  int getType(){ return PassStm;}
};

class ContinueStatement : public Statement {
public:
  ContinueStatement(){ }

  int getType(){ return ContStm;}
};

class BreakStatement : public Statement {
public:
  BreakStatement(){ }

  int getType(){ return BrkStm;}
};

class ReturnStatement : public Statement {
public:
  ReturnStatement(Expression* expr){
    expression = expr;
  }

  int getType(){ return RetStm;}

  Expression* expression;
};

class JuliaDocument {
public:
  JuliaDocument(){}

  void Print();

  StatementList* statements;
};

#endif
