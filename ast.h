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
  ArVarDeclStm,

  InvokeStm
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
  VarDescriptor(string name, int type, int items){
    varName = name;
    typeCode = type;
    this->items = items;
  }

  string varName;
  int typeCode;
  int items;
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

typedef struct AsmCode{
  
} AsmCode;

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
    bool existsInCurrentContext = variables.find(varName) != variables.end();

    if(existsInCurrentContext) return true;

    if(parentScope == NULL) return false;

    return parentScope->VariableExists(varName);
  }

  void AssertVariableDoesntExist(string name){
    if(VariableExists(name)){
      throw runtime_error("Variable " + name + " has already been declared\n");
    }
  }

  void AssertVariableExists(string name){
    if(!VariableExists(name)){
      throw runtime_error("Variable " + name + " is not accessible in the current context or it hasn't been declared.\n");
    }
  }

  bool IsInLoop(){
    if(scopeType == ForScopeT || scopeType == WhileScopeT){
      return true;
    }
    if(parentScope == NULL) return false;
    return parentScope->IsInLoop();
  }

  bool IsInFunction(){
    if(scopeType == FunctionScopeT){
      return true;
    }
    if(parentScope == NULL) return false;
    return parentScope->IsInFunction();
  }

  void AssertIsInFunction(string message){
    if(!IsInFunction()){
      throw runtime_error(message);
    }
  }

  void AssertIsInLoop(string message){
    if(!IsInLoop()){
      throw runtime_error(message);
    }
  }

  JuliaDocument* document;
  Scope* parentScope;
  map<string, VarDescriptor*> variables;
  int scopeType = GlobalScopeT;
};

class Expression {
public:
  virtual int getType() = 0;
  //virtual int getExpType() = 0;
  Scope* currentScope;
  virtual AsmCode GetAsm(Scope* scope) = 0;
};

class ExpressionList {
public:
  ExpressionList(){ }
  void AddNew(Expression* exp){
    expressions.push_front(exp);
  }

  std::list<Expression*> expressions;
  int getCount(){ return expressions.size(); }
};

class Statement {
public:
  virtual int getType() = 0;
  virtual AsmCode GetAsm(Scope* scope) = 0;

  Scope* currentScope;
};

class StatementList : public Statement{
public:
  StatementList(){ }

  void AddNew(Statement* stm){
    statements.push_front(stm);
  }

  AsmCode GetAsm(Scope* scope);

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
  bool FunctionExists(string name){
    return functions.find(name) != functions.end();
  }

  void AsssertFunctionDoesnExist(string name){
    if(FunctionExists(name)){
      throw runtime_error("Function " + name + " has already been defined.\n");
    }
  }

  void AsssertFunctionExists(string name){
    if(!FunctionExists(name)){
      throw runtime_error("Function " + name + " does not exist.\n");
    }
  }

  void InitLabels();

  string GetLabelFor(string kind, bool includeDot = false);

  Scope* globalScope;

  map<string, int> labels;
  map<string, FuncDescriptor*> functions;
  StatementList* statements;
};

#endif
