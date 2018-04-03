#include <cstdio>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <cstdlib>
#include <stdexcept>
#include "statements.h"

void JuliaDocument::Print() {
  string indent = ">";
  statements->Print(indent);
}

string JuliaDocument::GetLabelFor(string kind, bool includeDot){
  //check current count for kind, generate
  if(includeDot){
    kind.insert(0, ".");
  }

  kind.append("_");
  kind.append(to_string(labels[kind]++) );

  return kind;
}

void JuliaDocument::InitLabels(){
  labels["for"] = 0;
  labels["if"] = 0;
  labels["while"] = 0;
  labels["str"] = 0;
}

void JuliaDocument::Process(){
  FirstPass();
}

string JuliaDocument::GetAsm(){
  string funcDecsCode = "";
  string statementsCode = "";
  string dataSection = GetDataSegmentCode();

  stringstream ss;
  ss << "extern printf" << endl;
  ss << "global main" << endl;
  ss << dataSection << endl;
  ss << "section .text" << endl;
  ss << "main:" << endl;
  ss << statementsCode << endl;
  ss << "move eax, 0" << endl;
  ss << "ret 0" << endl;

  ss << funcDecsCode << endl;
}

string JuliaDocument::GetDataSegmentCode(){
  stringstream ss;
  ss << "section .data" << endl;
  ss << "sample_text db \"This is the content:!\", 10, 0" << endl;
  ss << "decimal_format db \"%d\", 10, 0" << endl;

  for (auto& str: strings) {
		ss << str.first << " db " << str.second << endl;
	}

  return ss.str();
}

void JuliaDocument::RegisterFunctions(){
  for(auto& stm : statements->statements){
    int stmType = stm->getType();
    if(stmType == FuncDeclStm){
      FuncDeclStatement* funcDecl = (FuncDeclStatement*)stm;
      FuncDescriptor* func =
        new FuncDescriptor(funcDecl->funcName, funcDecl->returnType->typeCode);

      AsssertFunctionDoesnExist(funcDecl->funcName);

      for(auto& param: funcDecl->params->paramList){
        VarDescriptor* parameter =
          new VarDescriptor(param->paramName, param->paramType->typeCode, sizeof(int));
        func->parameters.push_back(parameter);
      }
      functions[funcDecl->funcName] = func;
    }
  }
}

void JuliaDocument::PrintDoc(){
  return;
  for(auto& f: functions){
    cout << f.first << "(" << ")::"<< f.second->returnType <<"\n";
  }
  for(auto& v: globalScope->variables){
    cout << v.first << "::"<< v.second->typeCode << " [" << v.second->items <<"]\n";
  }
}

void JuliaDocument::FirstPass() {
  InitLabels();
  globalScope = new Scope(NULL, GlobalScopeT);
  globalScope->document = this;
  RegisterFunctions();

  for(auto& stm : statements->statements){
    int stmType = stm->getType();
    stm->currentScope = globalScope;
    stm->GetAsm(globalScope);

    switch(stmType){
      case FuncDeclStm: {
        FuncDeclStatement* funcDecl = (FuncDeclStatement*)stm;
        break;
      }
      case ScVarDeclStm: {
        ScalarVarDeclStatement* varDecl = (ScalarVarDeclStatement*)stm;
        break;
      }
      case ArVarDeclStm: {
        ArrayVarDeclStatement* varDecl = (ArrayVarDeclStatement*)stm;
        break;
      }
      case AssiStm: {
        AssignStatement* assStm = (AssignStatement*)stm;
        break;
      }
      case ArrAssiStm: {
        ArrayItemAssignStatement* assStm = (ArrayItemAssignStatement*)stm;
        break;
      }
      case IfStm: {
        IfStatement* ifStm = (IfStatement*)stm;
        break;
      }
      case ForStm: {
        ForStatement* forStm = (ForStatement*)stm;
        break;
      }
      case WhileStm: {
        WhileStatement* whileStm = (WhileStatement*)stm;
        break;
      }
      case PrintStm: {
        break;
      }
      case InvokeStm: {
        break;
      }
      default: {
        //Well, no such thing is possible to occur so...
        cout << "YOU FORGOT ON KIND OF STATEMENT: " << stmType << "\n";
      }
    }
  }

  //When a variable is assigned a value, complain it doesn't exist if it hasn't been declared
  //Begin asking for expression type
  //Associate a type to each expression when found, they can be integer or boolean
  //attemp tp register variables anywhere they're found
}
