#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <string>
#include <list>
#include <map>
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

enum ScopeType{
  GlobalScopeT,
  FunctionScopeT,
  WhileScopeT,
  IfScopeT,
  ForScopeT
};

using namespace std;

class JuliaDocument;

class VarDescriptor {
public:
  VarDescriptor(string name, int type, int bytes){
    varName = name;
    typeCode = type;
    byteSize = bytes;
  }

  string varName;
  int typeCode;
  int byteSize;
};

class FuncDescriptor{
public:
  FuncDescriptor(string name, int type){
    funcName = name;
    returnType = type;
  }

  string funcName;
  int returnType;
  list<VarDescriptor*> parameters;
};

class Scope {
public:
  Scope(Scope* parent, int type ){
    parentScope = parent;
    if(parent != NULL){
      document = parent->document;
    }
    scopeType = type;
  }

  bool IsGlobal(){ return scopeType == GlobalScopeT; }

  bool VariableExists(string varName){
    return variables.find(varName) != variables.end();
  }

  JuliaDocument* document;
  Scope* parentScope;
  map<string, VarDescriptor*> variables;
  int scopeType = GlobalScopeT;
};

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

class JuliaDocument {
public:
  JuliaDocument(){}

  void Print();

  void Process();
  void PrintDoc();

  void FirstPass();
  void RegisterFunctions();
  void RegisterGlobalVariables();

  void InitLabels();

  string GetLabelFor(string kind, bool includeDot);

  Scope* globalScope;

  map<string, int> labels;
  list<FuncDescriptor*> functions;
  StatementList* statements;
};

#endif
