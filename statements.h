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
	AsmCode GetAsm(Scope* scope);

	string varName;
	Expression *valueExpression;
};

class ArrayItemAssignStatement: public Statement {
public:
	ArrayItemAssignStatement(char* varName, Expression* index, Expression *expr) {
    this->indexExpression = index;
		this->varName = this->varName.append(varName);
		this->valueExpression = expr;
	}

  int getType() { return ArrAssiStm; };
	AsmCode GetAsm(Scope* scope);

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
	AsmCode GetAsm(Scope* scope);

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
	AsmCode GetAsm(Scope* scope);

	Expression* condition;
	StatementList* trueStatements;
	StatementList* falseStatements;
	string label_begin;
	string label_end;
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
	AsmCode GetAsm(Scope* scope);

  string varName;

	Expression* minExpression;
  Expression* maxExpression;

	StatementList* statements;
	Scope* localScope;
	string label_begin;
	string label_end;
};

class WhileStatement: public Statement {
public:
	WhileStatement(Expression* expr, StatementList* st) {
		this->condition = expr;
		this->statements = st;
	}

  void Print(string indent);

  int getType() { return WhileStm; };
	AsmCode GetAsm(Scope* scope);

	Expression* condition;
	StatementList* statements;
	string label_begin;
	string label_end;

	Scope* localScope;
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
	AsmCode GetAsm(Scope* scope);

  Expression* valueExpression;
};

class ArrayVarDeclStatement: public VarDeclStatement{
public:
  ArrayVarDeclStatement(char* name, ObjectType* type, ExpressionList* value): VarDeclStatement(name, type){
    values = value;
  }

  int getType(){ return ArVarDeclStm; }
	AsmCode GetAsm(Scope* scope);

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
	AsmCode GetAsm(Scope* scope);

  string funcName;
  ObjectType* returnType;
  ParamList* params;
  StatementList* statements;
	string label_end;

	Scope* functionScope;
};

class InvokeStatement: public Statement{
public:
	InvokeStatement(char* name, ExpressionList* args){
		funcName = funcName.append( name);
		arguments = args;
	}

	int getType(){ return InvokeStm; }
	AsmCode GetAsm(Scope* scope);

	string funcName;
	ExpressionList* arguments;

};

class PassStatement : public Statement {
public:
  PassStatement(){ }

  int getType(){ return PassStm; }
	inline AsmCode GetAsm(Scope* scope){}
};

class ContinueStatement : public Statement {
public:
  ContinueStatement(){ }

  int getType(){ return ContStm; }
	AsmCode GetAsm(Scope* scope);
};

class BreakStatement : public Statement {
public:
  BreakStatement(){ }

  int getType(){ return BrkStm;}
	AsmCode GetAsm(Scope* scope);
};

class ReturnStatement : public Statement {
public:
  ReturnStatement(Expression* expr){
    expression = expr;
  }

  AsmCode GetAsm(Scope* scope);
  int getType(){ return RetStm;}

  Expression* expression;
};

#endif
