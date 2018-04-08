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

void JuliaDocument::InitRegisters(){
  registers["ebx"] = false;
  registers["ecx"] = false;
  registers["esi"] = false;
  registers["edi"] = false;
}

string JuliaDocument::RequestRegister(){
  for(auto& r : registers){
    if(r.second == false){
      r.second = true;
      return r.first;
    }
  }
  throw runtime_error("Ran out of registers!\n");
}

void JuliaDocument::FreeUpRegister(string name){
  if(registers.find(name) == registers.end()) throw runtime_error("No such register: " + name);

  registers[name] = false;
}

void JuliaDocument::Process(){
  InitLabels();
  InitRegisters();
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
  ss << "global cpower" << endl;
  ss << dataSection << endl;
  ss << "section .text" << endl;
  ss << "main:" << endl;
  ss << "  push ebp" << endl;
  ss << "  mov ebp, esp" << endl;
  ss << "  sub esp, 256" << endl << endl;
  ss << statementsCode << endl;
  ss << "  leave" << endl;
  ss << "  mov eax, 0" << endl;
  ss << "  ret 0" << endl;

  ss << "cpower:" << endl;
  ss << "  push ebp" << endl;
  ss << "  mov ebp, esp" << endl;
  ss << "  sub esp, 16" << endl;
  ss << "  mov [ebp-4], dword 1" << endl << endl;
  ss << "cpower_while_begin:" << endl;
  ss << "  cmp dword [ebp+12], 0" << endl;
  ss << "  je cpower_while_end" << endl;
  ss << "  nop" << endl;
  ss << "  mov eax, dword [ebp-4]" << endl;
  ss << "  mov ebx, dword [ebp+8]" << endl;
  ss << "  mul ebx" << endl;
  ss << "  mov dword [ebp-4], eax" << endl;
  ss << "  mov ebx, dword [ebp+12]" << endl;
  ss << "  sub ebx, 1" << endl;
  ss << "  mov dword [ebp+12], ebx" << endl;
  ss << "  jmp cpower_while_begin" << endl;
  ss << "  nop" << endl;
  ss << "cpower_while_end:" << endl <<endl;
  ss << "  mov eax, dword [ebp-4]" << endl;
  ss << "  leave" << endl;
  ss << "  ret" << endl;

  ss << "printbool:" << endl;
  ss << "  push ebp" << endl;
  ss << "  mov ebp, esp" << endl << endl;
  ss << "printbool_if_begin:" << endl;
  ss << "  cmp dword [ebp+8], 1" << endl;
  ss << "  jne printbool_if_else" << endl;
  ss << "  nop" << endl << endl;
  ss << "  push dword true_format" << endl;
  ss << "  call printf" << endl;
  ss << "  add esp, 4" << endl << endl;
  ss << "  jmp printbool_if_end" << endl;
  ss << "  nop" << endl;
  ss << "printbool_if_else:" << endl;
  ss << "  push dword false_format" << endl;
  ss << "  call printf" << endl;
  ss << "  add esp, 4" << endl << endl;
  ss << "printbool_if_end:" << endl;
  ss << "  leave" << endl;
  ss << "  ret" << endl;

  ss << funcDecsCode << endl;

  return ss.str();
}

string JuliaDocument::GetDataSegmentCode(){
  stringstream ss;
  ss << "section .data" << endl;
  ss << "  sample_text db \"This is the content:!\", 10, 0" << endl;
  ss << "  dec_format db \"%d\", 0" << endl;
  ss << "  str_format db \"%s\", 0" << endl;
  ss << "  newline_format db 10, 0" << endl;
  ss << "  false_format db \"false\", 0" << endl;
  ss << "  true_format db \"true\", 0" << endl;

  for (auto& str: strings) {
    string woQuotes = regex_replace(str.second, regex("\\\\\""), "\", 34, \"");
    string woSlashes = regex_replace(woQuotes, regex("\\\\\\\\"), "\", 92, \"");
    string woTabs = regex_replace(woSlashes, regex("\\\\t"), "\", 9, \"");
    string woLines = regex_replace(woTabs, regex("\\\\n"), "\", 10, \"");
		ss << "  " << str.first << " db \"" << woLines << "\", 0" << endl;
	}

  for(auto& var: globalScope->variables){
    ss << "  global_" << var.first << " dd";
    for(int i = 1; i <= var.second->items; i++){
      ss << " 0";
      if(i < var.second->items){
        ss << ",";
      }
    }
    ss << endl;
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
          new VarDescriptor(param->paramName, param->paramType->typeCode, sizeof(int), true, false, false);
        func->parameters.push_back(parameter);
      }
      functions[funcDecl->funcName] = func;
    }
  }
}

void JuliaDocument::RegisterGlobals(){
  for(auto& stm : statements->statements){
    int stmType = stm->getType();
    if(stmType == ScVarDeclStm){
      VarDeclStatement* varDecl = (VarDeclStatement*)stm;
      VarDescriptor* varDes =
        new VarDescriptor(varDecl->varName, varDecl->varType->typeCode, 1, false, true, false);

      globalScope->variables[varDecl->varName] = varDes;
    }else if(stmType == ArVarDeclStm){
      ArrayVarDeclStatement* varDecl = (ArrayVarDeclStatement*)stm;
      VarDescriptor* varDes =
        new VarDescriptor(varDecl->varName, varDecl->varType->typeCode, varDecl->values->getCount(), false, true, false);

      globalScope->variables[varDecl->varName] = varDes;
    }
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
