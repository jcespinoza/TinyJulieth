#ifndef STATEMENTS_H
#define STATEMENTS_H

#include <stdio.h>
#include <string>
#include <list>
#include <map>
#include <cmath>
#include "ast.h"

using namespace std;

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

#endif
