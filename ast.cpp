#include <cstdio>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <cstdlib>
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
  //set all label kinds to zero
  labels["for"] = 0;
  labels["if"] = 0;
  labels["while"] = 0;
  labels["str"] = 0;
}

void JuliaDocument::Process(){
  FirstPass();
}

void JuliaDocument::RegisterFunctions(){
  for(auto& stm : statements->statements){
    int stmType = stm->getType();
    if(stmType == FuncDeclStm){
      FuncDeclStatement* funcDecl = (FuncDeclStatement*)stm;
      FuncDescriptor* func =
        new FuncDescriptor(funcDecl->funcName, funcDecl->returnType->typeCode);
      for(auto& param: funcDecl->params->paramList){
        VarDescriptor* parameter =
          new VarDescriptor(param->paramName, param->paramType->typeCode, sizeof(int));
        func->parameters.push_back(parameter);
      }
      functions.push_back(func);
    }
  }
}

void JuliaDocument::RegisterGlobalVariables(){
  for(auto& stm : statements->statements){
    int stmType = stm->getType();
    if(stmType == ScVarDeclStm || stmType == ArVarDeclStm){
      VarDeclStatement* var = (VarDeclStatement*)stm;
      VarDescriptor* newVar = new VarDescriptor(var->varName, var->varType->typeCode, sizeof(int));
      //Check if doesn't exist first
      globalScope->variables[var->varName] = newVar;
    }
  }
}

void JuliaDocument::PrintDoc(){
  return;
  for(auto& f: functions){
    cout << f->funcName << "(" << ")::"<< f->returnType <<"\n";
  }
  for(auto& v: globalScope->variables){
    cout << v.first << "::"<< v.second->typeCode <<"\n";
  }
}

void JuliaDocument::FirstPass() {
  InitLabels();
  globalScope = new Scope(NULL, GlobalScopeT);
  RegisterFunctions();
  RegisterGlobalVariables();

  for(auto& stm : statements->statements){
    int stmType = stm->getType();
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
      default: {
        //Well, no such thing is possible to occur so...
        cout << "YOU FORGOT ON KIND OF STATEMENT: " << stmType << "\n";
      }
    }
  }
  //Check which statements are variable declarations
  //When a variable is assigned a value, complain it doesn't exist if it hasn't been declared
  //Begin asking for expression type
  //Associate a type to each expression when found, they can be integer or boolean
  //attemp tp register variables anywhere they're found
}
