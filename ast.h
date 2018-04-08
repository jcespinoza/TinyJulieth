#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <string>
#include <list>
#include <map>
#include <cmath>
#include <sstream>
#define MALLOCK_CHECK 1

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

enum AsmLocationType{
  AddressLocationType,
  RegisterLocationType,
  LabelLocationType,
  LiteralLocationType
};

using namespace std;

class JuliaDocument;

class VarDescriptor {
public:
  VarDescriptor(string name, int type, int items, bool isParam, bool global, bool loop){
    varName = name;
    typeCode = type;
    this->items = items;
    isParameter = isParam;
    isGlobal = global;
    isLoop = loop;
  }

  string varName;
  bool isGlobal = false;
  bool isLoop = false;
  int typeCode = 0;
  int items = 1;
  int offset = 0;
  bool isParameter = false;;
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
  string code;
  string location;
  int locationType;

  string GetValue32(){
    return (locationType == LabelLocationType || locationType == AddressLocationType) ? ("[" + location + "]"):
    location;
  }

  void PutIntoLabel(string value){
    location = value;
    locationType = LabelLocationType;
  }

  void PutIntoRegister(string value){
    location = value;
    locationType = RegisterLocationType;
  }

  void PutIntoAddress(string value){
    location = value;
    locationType = AddressLocationType;
  }

  void PutLiteral(string value){
    location = value;
    locationType = LiteralLocationType;
  }
} AsmCode;

typedef struct ArrayAllocation{
  string name;
  int size;
  int offset;
} ArrayAllocation;

class ScopeStack {
public:
  ScopeStack(){

  }

  void FreeUpOffset(string name){
    auto item = namedOffsets.find(name);
    if(item == namedOffsets.end()){
      throw runtime_error("No such named offset " + name + "\n");
    }
    offsets[item->second] = false;
  }

  void FreeUpOffset(int offset){
    offsets[offset] = false;
  }

  int AllocateArrayOffset(string name, int size){
    int max = 4;
    for(auto& as: arrayOffsets){
      ArrayAllocation allo = as.second;
      max += allo.size*4;
    }

    ArrayAllocation arlo;
    arlo.name = name;
    arlo.offset = max;
    arlo.size = size;
    arrayOffsets[name] = arlo;
    return max;
  }

  int AllocateOffset(){
    int max = 4;
    for(auto& os: offsets){
      if(os.second == false){
        os.second = true;
        return os.first;
      }
      max += 4;
    }
    //Allocating a new one and set it to taken
    offsets[max] = true;
    return max;
  }

  int AllocateOffset(string name){
    int offset = AllocateOffset();
    namedOffsets[name] = offset;
  }

  map<int, bool> offsets;
  map<string, ArrayAllocation> arrayOffsets;
  map<string, int> namedOffsets;
  int maxArrayOffset = 256;
};

class Scope {
public:
  Scope(Scope* parent, int type ){
    parentScope = parent;
    if(parent != NULL){
      document = parent->document;

      if(type != FunctionScopeT && type != GlobalScopeT){
        stack = parent->stack;
      }
    }
    if(type == FunctionScopeT || type == GlobalScopeT){
      stack = new ScopeStack();
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

  VarDescriptor* GetVariable(string name){
    if(IsGlobal()){ AssertVariableExists(name); }
    if(variables.find(name) != variables.end()){
      VarDescriptor* found = variables[name];
      return found;
    }
    return parentScope->GetVariable(name);;
  }

  void RegisterVariable(VarDescriptor* desc){
    variables[desc->varName] = desc;
  }

  string GetStackAllocation(){
    stringstream ss;
    int reqBytes = stack->offsets.size();
    int missingBytes = (16 - (reqBytes % 16) );
    int bytesForAllocation = reqBytes + missingBytes;
    ss << "sub esp, " << bytesForAllocation << endl;

    return ss.str();
  }

  string GetLoopBegin(){
    if(scopeType == ForScopeT || scopeType == WhileScopeT){
      return label_begin;
    }
    if(parentScope != NULL) return parentScope->GetLoopBegin();
    throw runtime_error("Cant find loop start label.");
  }

  string GetLoopEnd(){
    if(scopeType == ForScopeT || scopeType == WhileScopeT){
      return label_end;
    }
    if(parentScope != NULL) return parentScope->GetLoopEnd();
    throw runtime_error("Cant find loop start label.");
  }

  JuliaDocument* document;
  string label_begin;
  string label_end;
  Scope* parentScope;
  ScopeStack* stack;
  map<string, VarDescriptor*> variables;
  int scopeType = GlobalScopeT;
};

class Expression {
public:
  virtual int getType() = 0;
  //virtual int getExpType() = 0;
  Scope* currentScope;
  bool IsBoolean(){ return false; }
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

  int getType(){ return StmListStm; }

  std::list<Statement*> statements;
};

class JuliaDocument {
public:
  JuliaDocument(){}

  void Process();

  string GetCodeForStatements();
  string GetCodeForFunctions();
  void RegisterFunctions();
  void RegisterGlobals();
  string RegisterString(string stringContent);
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
  void InitRegisters();
  string RequestRegister();
  void FreeUpRegister(string);

  string GetAsm();

  string GetDataSegmentCode();

  string GetLabelFor(string kind, bool includeDot = false);

  Scope* globalScope;

  map<string, int> labels;
  map<string, string> strings;
  map<string, bool> registers;

  map<string, FuncDescriptor*> functions;
  StatementList* statements;
};

#endif
