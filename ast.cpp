#include <cstdio>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <cstdlib>
#include <stdexcept>
#include "statements.h"
#include <regex>

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

string JuliaDocument::RegisterString(string content){
  string label = GetLabelFor("str");

  strings[label] = content;

  return label;
}

void JuliaDocument::InitLabels(){
  labels["for"] = 0;
  labels["if"] = 0;
  labels["while"] = 0;
  labels["str"] = 0;
}

void JuliaDocument::Process(){
  InitLabels();
  globalScope = new Scope(NULL, GlobalScopeT);
  globalScope->document = this;
  RegisterFunctions();

  string documentAsm = GetAsm();

  cout << documentAsm << endl << endl;
}

string JuliaDocument::GetAsm(){
  string funcDecsCode = GetCodeForFunctions();
  string statementsCode = GetCodeForStatements();
  string dataSection = GetDataSegmentCode();

  stringstream ss;
  ss << "extern printf" << endl;
  ss << "global main" << endl;
  ss << dataSection << endl;
  ss << "section .text" << endl;
  ss << "main:" << endl;
  ss << statementsCode << endl;
  ss << "mov eax, 0" << endl;
  ss << "ret 0" << endl;

  ss << funcDecsCode << endl;

  return ss.str();
}

string JuliaDocument::GetDataSegmentCode(){
  stringstream ss;
  ss << "section .data" << endl;
  ss << "sample_text db \"This is the content:!\", 10, 0" << endl;
  ss << "dec_format db \"%d\", 0" << endl;
  ss << "str_format db \"%s\", 0" << endl;
  ss << "dec_formatln db \"%d\", 10, 0" << endl;
  ss << "str_formatln db \"%s\", 10, 0" << endl;

  for (auto& str: strings) {
    string woQuotes = regex_replace(str.second, regex("\\\\\""), "\", 34, \"");
    string woLines = regex_replace(woQuotes, regex("\\\\n"), "\", 10, \"");
		ss << str.first << " db \"" << woLines << "\", 0" << endl;
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

string JuliaDocument::GetCodeForFunctions() {
  stringstream ss;

  for(auto& stm : statements->statements){
    int stmType = stm->getType();

    if(stmType != FuncDeclStm) continue;

    stm->currentScope = globalScope;
    AsmCode code = stm->GetAsm(globalScope);

    ss << code.code << endl;
  }
  return ss.str();
}

string JuliaDocument::GetCodeForStatements() {
  stringstream ss;

  for(auto& stm : statements->statements){
    int stmType = stm->getType();

    if(stmType == FuncDeclStm) continue;

    stm->currentScope = globalScope;
    AsmCode code = stm->GetAsm(globalScope);

    ss << code.code << endl;
  }
  return ss.str();
}
