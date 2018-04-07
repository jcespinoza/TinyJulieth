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
  valueExpression->currentScope = scope;
  AsmCode expCode = valueExpression->GetAsm(scope);
  ss << expCode.code;
  string valueLocation;
  if(expCode.locationType == LiteralLocationType){

    VarDescriptor* desc = scope->GetVariable(varName);
    if(desc->isGlobal){
      ss << "  mov dword global_" << desc->varName << ", " << expCode.location << endl;
    }else if(desc->isParameter){

    }
  }
  return asmCode;
}

AsmCode ArrayItemAssignStatement::GetAsm(Scope* scope){
  return AsmCode();
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
      || expType == NeqExp){
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
    if((expType == IdExp
      || expType == AddExp
      || expType == SubExp
      || expType == DivExp
      || expType == ModExp
      || expType == MulExp
      || expType == PowExp)
      && scope->IsGlobal()){
      AsmCode expCode = exp->GetAsm(scope);
      if(expCode.locationType == RegisterLocationType){
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
  label_begin = scope->document->GetLabelFor("if");
  Scope* localScope = new Scope(scope, IfScopeT);
  trueStatements->GetAsm(localScope);

  localScope = new Scope(scope, IfScopeT);
  falseStatements->GetAsm(localScope);

  return AsmCode();
}

AsmCode ForStatement::GetAsm(Scope* scope){
  // Register counter variable
  label_begin = scope->document->GetLabelFor("for");
  localScope = new Scope(scope, ForScopeT);
  statements->GetAsm(localScope);
  return AsmCode();
}

AsmCode WhileStatement::GetAsm(Scope* scope){
  label_begin = scope->document->GetLabelFor("while");
  localScope = new Scope(scope, WhileScopeT);
  statements->GetAsm(localScope);
  return AsmCode();
}

AsmCode ScalarVarDeclStatement::GetAsm(Scope* scope){
  scope->AssertVariableDoesntExist(varName);

  VarDescriptor* newVar = new VarDescriptor(varName, varType->typeCode, 1, false, scope->IsGlobal());

  stringstream ss;
  AsmCode asmCode;
  AsmCode expCode = valueExpression->GetAsm(scope);
  ss << expCode.code;
  if(scope->IsGlobal()){
    if(expCode.locationType == LiteralLocationType){
      ss << "  mov dword [global_" << varName  << "], " << expCode.location << endl;
    }
  }
  scope->variables[varName] = newVar;
  asmCode.code = ss.str();

  return asmCode;
}

AsmCode ArrayVarDeclStatement::GetAsm(Scope* scope){
  scope->AssertVariableDoesntExist(varName);

  VarDescriptor* newVar = new VarDescriptor(varName, varType->typeCode, values->getCount(), false, scope->IsGlobal());
  newVar->offset = scope->stack->AllocateOffset(varName);

  scope->variables[varName] = newVar;

  return AsmCode();
}

AsmCode FuncDeclStatement::GetAsm(Scope* scope){
  functionScope = new Scope(scope, FunctionScopeT);
  //register pareters as if they were normal variables
  int order = 0;
  for(auto& param : params->paramList){
    VarDescriptor* newVar = new VarDescriptor(param->paramName, param->paramType->typeCode, 1, true, false);
    newVar->offset = 2*sizeof(int) + order*sizeof(int);
    functionScope->variables[param->paramName] = newVar;
  }
  statements->GetAsm(functionScope);
  return AsmCode();
}

AsmCode InvokeStatement::GetAsm(Scope* scope){
  return AsmCode();
  scope->document->AsssertFunctionExists(funcName);
}

AsmCode ContinueStatement::GetAsm(Scope* scope){
  scope->AssertIsInLoop("Continue statement not valid in this context.\n");
  return AsmCode();
}

AsmCode BreakStatement::GetAsm(Scope* scope){
  scope->AssertIsInLoop("Break statement not valid in this context.\n");
  return AsmCode();
}

AsmCode ReturnStatement::GetAsm(Scope* scope){
  //Check the current scope is not the global scope
  if(currentScope->scopeType == GlobalScopeT){
    throw runtime_error("Return statement is not valid in the global scope.\n");
  }
  scope->AssertIsInFunction("Return statements are only valid inside a function.\n");
  return AsmCode();
}
