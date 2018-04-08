#include <cstdio>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <cstdlib>
#include <stdexcept>
#include "statements.h"
#include "expressions.h"

AsmCode StatementList::GetAsm(Scope* scope){
  stringstream ss;
  for(auto& stm : statements){
    if(stm != NULL){
      stm->currentScope = scope;
      AsmCode stmCode = stm->GetAsm(scope);
      ss << stmCode.code;
    }
  }
  AsmCode sts;
  sts.code = ss.str();
  return sts;
}

AsmCode AssignStatement::GetAsm(Scope* scope){
  scope->AssertVariableExists(varName);
  stringstream ss;
  AsmCode asmCode;
  AsmCode expCode = valueExpression->GetAsm(scope);
  ss << expCode.code;

  if(expCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(expCode.location);
  }

  VarDescriptor* desc = scope->GetVariable(varName);
  if(desc->isGlobal){
    ss << "  mov dword  [global_" << desc->varName << "], " << expCode.location << endl;
  }else if(desc->isParameter){
    ss << "  mov dword  [ebp+" << desc->offset << "], " << expCode.location << endl;
  }
  asmCode.code = ss.str();
  return asmCode;
}

AsmCode ArrayItemAssignStatement::GetAsm(Scope* scope){
  scope->AssertVariableExists(varName);
  stringstream ss;
  AsmCode asmCode;
  AsmCode expCode = valueExpression->GetAsm(scope);
  ss << expCode.code;

  VarDescriptor* desc = scope->GetVariable(varName);
  string tReg = scope->document->RequestRegister();
  if(expCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(expCode.location);
  }
  int valueOffset = scope->stack->AllocateOffset();
  ss << "  mov " << tReg << ", " << expCode.GetValue32() << endl;
  ss << "  mov dword [ebp-" << valueOffset <<"], " << tReg << endl;

  AsmCode indexCode = indexExpression->GetAsm(scope);
  ss << indexCode.code;
  ss << "  mov " << tReg << ", dword " << indexCode.GetValue32() << endl;
  ss << "  sub " << tReg << ", 1" << endl;
  ss << "  shl " << tReg << ", 2" << endl; //multiply by 4

  if(desc->isGlobal){
    string sReg = scope->document->RequestRegister();
    ss << "  add " << tReg << ", global_" << varName << endl;
    ss << "  mov " << sReg << ", dword [ebp-" << valueOffset << "]" << endl;
    ss << "  mov dword [" << tReg << "], " << sReg << endl;
    scope->document->FreeUpRegister(sReg);
  }
  scope->stack->FreeUpOffset(valueOffset);
  scope->document->FreeUpRegister(tReg);
  asmCode.code = ss.str();
  return asmCode;
}

AsmCode PrintStatement::GetAsm(Scope* scope){
  stringstream ss;

  for(auto& exp: expressionList->expressions){
    int expType = exp->getType();

    if(expType == StrExp){
      AsmCode expCode = exp->GetAsm(scope);
      ss << expCode.code;
      ss << "  push dword " << expCode.location << endl;
        ss << "  push dword str_format" << endl;
      ss << "  call printf" << endl;
      ss << "  add esp, 8" << endl;
    }
    if(expType == BoolExp
      || expType == EquExp
      || expType == NeqExp
      || expType == LthExp
      || expType == GthExp
      || expType == GeqExp
      || expType == LeqExp
      || expType == LorExp
      || expType == LandExp
      || expType == NotExp){
        AsmCode expCode = exp->GetAsm(scope);
        if(expCode.locationType == RegisterLocationType){
          scope->document->FreeUpRegister(expCode.location);
        }
        ss << expCode.code;
        ss << "  push dword " << expCode.GetValue32() << endl;
        ss << "  call printbool" << endl;
        ss << "  add esp, 4" << endl;
    }
    if(expType == NumExp){
      AsmCode expCode = exp->GetAsm(scope);
      ss << expCode.code;
      ss << "  push dword " << expCode.location << endl;
      ss << "  push dword dec_format" << endl;
      ss << "  call printf" << endl;
      ss << "  add esp, 8" << endl;
    }
    if(expType == IdExp ){
      AsmCode expCode = exp->GetAsm(scope);

      VarDescriptor* desc = scope->GetVariable(((IdExpression*)exp)->varName);
      if(desc->typeCode == BoolType){
        ss << expCode.code;
        ss << "  push dword " << expCode.GetValue32() << endl;
        ss << "  call printbool" << endl;
        ss << "  add esp, 4" << endl;
      }else{
        ss << expCode.code;
        ss << "  push dword " << expCode.GetValue32() << endl;
        ss << "  push dword dec_format" << endl;
        ss << "  call printf" << endl;
        ss << "  add esp, 8" << endl;
      }
      if(expCode.IsRegister()){
        scope->document->FreeUpRegister(expCode.location);
      }
    }
    if((expType == AddExp
      || expType == SubExp
      || expType == DivExp
      || expType == ModExp
      || expType == MulExp
      || expType == PowExp
      || expType == BitNotExp
      || expType == XorExp
      || expType == BorExp
      || expType == BandExp
      || expType == ShlExp
      || expType == ShrExp
      || expType == ArrAccExp
      || expType == FCallExp)){
      AsmCode expCode = exp->GetAsm(scope);
      if(expCode.IsRegister()){
        scope->document->FreeUpRegister(expCode.location);
      }
      ss << expCode.code;
      ss << "  push dword " << expCode.GetValue32() << endl;
      ss << "  push dword dec_format" << endl;
      ss << "  call printf" << endl;
      ss << "  add esp, 8" << endl;
    }
  }
  if(withNewLine){
    ss << "  push dword newline_format" << endl;
    ss << "  call printf" << endl;
    ss << "  add esp, 4" << endl;
  }

  AsmCode printCode;
  printCode.code = ss.str();
  return printCode;
}

AsmCode IfStatement::GetAsm(Scope* scope){
  AsmCode asmCode;
  stringstream ss;
  label_begin = scope->document->GetLabelFor("if");
  Scope* localScope = new Scope(scope, IfScopeT);

  AsmCode expCode = condition->GetAsm(localScope);
  ss << label_begin << "_begin:" << endl;
  ss << expCode.code;

  if(expCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(expCode.location);
  }
  ss << "  xor eax, eax" << endl;
  ss << "  cmp " << expCode.GetValue32() << ", 0" << endl;
  ss << "  je " << label_begin << "_else" << endl << "  nop" << endl;

  AsmCode tCode = trueStatements->GetAsm(localScope);
  ss << tCode.code;
  ss << "  jmp " << label_begin << "_end" << endl << "  nop" << endl;

  localScope = new Scope(scope, IfScopeT);
  AsmCode fCode = falseStatements->GetAsm(localScope);

  ss << label_begin << "_else:" << endl;
  ss << fCode.code;
  ss << label_begin << "_end:" << endl;

  asmCode.code = ss.str();
  return asmCode;
}

AsmCode ForStatement::GetAsm(Scope* scope){
  AsmCode asmCode;
  stringstream ss;
  // Register counter variable
  label_begin = scope->document->GetLabelFor("for");
  localScope = new Scope(scope, ForScopeT);
  scope->label_begin = label_begin + "_begin";
  scope->label_end = label_begin + "_end";

  VarDescriptor* newVar = new VarDescriptor(varName, IntType, 1, false, false, true);
  int offset = scope->stack->AllocateOffset(varName);
  newVar->offset = offset;
  localScope->RegisterVariable(newVar);

  AsmCode minCode = minExpression->GetAsm(scope);
  if(minCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(minCode.location);
  }
  AsmCode maxCode = maxExpression->GetAsm(scope);
  ss << "  mov dword [ebp-" << offset << "], " << minCode.GetValue32() << endl;
  ss << label_begin << "_begin:" << endl;

  string tReg = scope->document->RequestRegister();
  if(maxCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(maxCode.location);
  }
  ss << maxCode.code;
  ss << "  mov " << tReg << ", " << maxCode.GetValue32() << endl;
  ss << "  cmp dword [ebp-" << offset << "], " << tReg << endl;
  ss << "  jg " << label_begin << "_end" << endl << "  nop" << endl;

  scope->document->FreeUpRegister(tReg);

  AsmCode stmCode = statements->GetAsm(localScope);

  ss << stmCode.code;
  ss << "  add dword [ebp-" << offset << "], 1" << endl;
  ss << "  jmp " << label_begin << "_begin" << endl;
  ss << "  nop" << endl;
  ss << label_begin << "_end:" << endl;
  scope->stack->FreeUpOffset(varName);

  asmCode.code = ss.str();
  return asmCode;
}

AsmCode WhileStatement::GetAsm(Scope* scope){
  AsmCode asmCode;
  stringstream ss;

  label_begin = scope->document->GetLabelFor("while");
  localScope = new Scope(scope, WhileScopeT);
  localScope->label_begin = label_begin +"_begin";
  localScope->label_end = label_begin +"_end";

  ss << label_begin <<"_begin:" << endl;

  AsmCode expCode = condition->GetAsm(scope);

  if(expCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(expCode.location);
  }
  ss << expCode.code;
  ss << "  xor eax, eax" << endl;
  ss << "  cmp " << expCode.GetValue32() << ", 0" << endl;
  ss << "  je " << label_begin << "_end" << endl << "  nop" << endl;

  AsmCode stmCode = statements->GetAsm(localScope);
  ss << stmCode.code;
  ss << "  jmp " << label_begin << "_begin" << endl;
  ss << "  nop" << endl;
  ss << label_begin << "_end:" << endl;

  asmCode.code = ss.str();

  return asmCode;
}

AsmCode ScalarVarDeclStatement::GetAsm(Scope* scope){
  if(!scope->IsGlobal()){
    scope->AssertVariableDoesntExist(varName);
  }
  VarDescriptor* newVar = new VarDescriptor(varName, varType->typeCode, 1, false, scope->IsGlobal(), false);

  stringstream ss;
  AsmCode asmCode;
  AsmCode expCode = valueExpression->GetAsm(scope);
  ss << expCode.code;

  if(expCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(expCode.location);
  }

  if(scope->IsGlobal()){
    ss << "  mov dword [global_" << varName  << "], " << expCode.GetValue32() << endl;
  }else{
    int offset = scope->stack->AllocateOffset(varName);
    newVar->offset = offset;
    ss << "  mov dword [ebp-" << offset << "], " << expCode.GetValue32() << endl;
  }
  scope->variables[varName] = newVar;
  asmCode.code = ss.str();

  return asmCode;
}

AsmCode ArrayVarDeclStatement::GetAsm(Scope* scope){
  if(!scope->IsGlobal()){
    scope->AssertVariableDoesntExist(varName);
  }
  AsmCode asmCode;
  stringstream ss;

  VarDescriptor* newVar = new VarDescriptor(varName, varType->typeCode, values->getCount(), false, scope->IsGlobal(), false);
  if(!scope->IsGlobal()){
    newVar->offset = scope->stack->AllocateArrayOffset(varName, values->getCount());
  }
  scope->variables[varName] = newVar;
  int index = 0;
  for(auto& item: values->expressions){
    AsmCode expCode = item->GetAsm(scope);

    string tReg = scope->document->RequestRegister();
    if(expCode.locationType == RegisterLocationType){
      scope->document->FreeUpRegister(expCode.location);
    }
    ss << expCode.code;
    ss << "  mov " << tReg << ", " << expCode.GetValue32() << endl;
    if(scope->IsGlobal()){
      ss << "  mov " << "[global_" << varName << "+"<< (index*4) << "], dword " << tReg << endl;;
    }else{

    }
    scope->document->FreeUpRegister(tReg);
    index++;
  }

  asmCode.code = ss.str();
  return asmCode;
}

AsmCode FuncDeclStatement::GetAsm(Scope* scope){
  AsmCode asmCode;
  stringstream ss;

  functionScope = new Scope(scope, FunctionScopeT);

  int order = 0;
  for(auto& param : params->paramList){
    VarDescriptor* newVar = new VarDescriptor(param->paramName, param->paramType->typeCode, 1, true, false, false);
    newVar->offset = 2*sizeof(int) + order*sizeof(int);
    functionScope->variables[param->paramName] = newVar;
    order++;
  }
  ss << funcName << ":" << endl;
  ss << "  push ebp" << endl;
  ss << "  mov ebp, esp" << endl;
  ss << "  sub esp, 256" << endl << endl;

  AsmCode stmCode = statements->GetAsm(functionScope);
  ss << stmCode.code;

  ss << "  leave" << endl << "  ret" << endl;

  asmCode.code = ss.str();
  return asmCode;
}

AsmCode InvokeStatement::GetAsm(Scope* scope){
  AsmCode asmCode;
  stringstream ss;
  auto iter = arguments->expressions.rbegin();
  for(; iter != arguments->expressions.rend(); ++iter){
    AsmCode expCode = (*iter)->GetAsm(scope);
    ss << expCode.code;
    ss << ";  push " << expCode.GetValue32();
  }
  //ss <<

  asmCode.code = ss.str();
  return asmCode;
}

AsmCode ContinueStatement::GetAsm(Scope* scope){
  scope->AssertIsInLoop("Continue statement not valid in this context.\n");
  AsmCode asmCode;
  stringstream ss;

  string jmpLabel = scope->GetLoopBegin();
  ss << "  jmp " << jmpLabel << endl << "  nop" << endl;

  asmCode.code = ss.str();
  return asmCode;
}

AsmCode BreakStatement::GetAsm(Scope* scope){
  AsmCode asmCode;
  stringstream ss;
  scope->AssertIsInLoop("Break statement not valid in this context.\n");

  string jmpLabel = scope->GetLoopEnd();
  ss << "  jmp " << jmpLabel << endl << "  nop" << endl;

  asmCode.code = ss.str();
  return asmCode;
}

AsmCode ReturnStatement::GetAsm(Scope* scope){
  if(currentScope->scopeType == GlobalScopeT){
    throw runtime_error("Return statement is not valid in the global scope.\n");
  }
  stringstream ss;
  AsmCode asmCode;

  if(expression != NULL){
    AsmCode expCode = expression->GetAsm(scope);
    ss << expCode.code;

    if(expCode.locationType == RegisterLocationType){
      scope->document->FreeUpRegister(expCode.location);
    }

    ss << "  mov eax, " << expCode.GetValue32() << endl;
  }

  ss << "  leave" << endl << "  ret" << endl;
  scope->AssertIsInFunction("Return statements are only valid inside a function.\n");

  asmCode.code = ss.str();
  return asmCode;
}
