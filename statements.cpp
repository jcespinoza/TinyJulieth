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

void StatementList::Print(string indent) {
  for (auto& var : statements) {
    if(var != NULL){
      int stmType = var->getType();
      if(stmType == StmListStm){
        StatementList* stms = ((StatementList*)var);
        stms->Print(indent + indent);
      }else if(stmType == FuncDeclStm){
        FuncDeclStatement* func = ((FuncDeclStatement*)var);
        func->Print(indent);
      }else if( stmType == IfStm) {
        IfStatement* func = ((IfStatement*)var);
        func->Print(indent);
      }else if( stmType == ForStm) {
        ForStatement* func = ((ForStatement*)var);
        func->Print(indent);
      }else if( stmType == WhileStm) {
        WhileStatement* func = ((WhileStatement*)var);
        func->Print(indent);
      }else{
  		  cout << indent << "Statement of Type: " << var->getType() << endl;
      }
    }else{
      cout << "Statement is null" << endl;
    }
	}
}

void FuncDeclStatement::Print(string indent) {
  statements->Print(indent + "_");
}

void ForStatement::Print(string indent) {
  statements->Print(indent + "_");
}

void WhileStatement::Print(string indent) {
  statements->Print(indent + "_");
}

void IfStatement::Print(string indent) {
  trueStatements->Print(indent + "^");
  trueStatements->Print(indent + "v");
}

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
  return AsmCode();
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
      ss << "pushad" << endl;
      ss << "push dword " << expCode.location << endl;
      //printf("Compare '%s' and '%s'\n",((StrExpression*)exp)->strValue,  ((StrExpression*)expressionList->expressions.back())->strValue);
      if(withNewLine){
        ss << "push dword str_formatln" << endl;
      }else{
        ss << "push dword str_format" << endl;
      }
      ss << "call printf" << endl;
      ss << "add esp, 8" << endl;
      ss << "popad" << endl;
    }
    if(expType == 1993){
      AsmCode expCode = exp->GetAsm(scope);
      ss << expCode.code;
      ss << "pushad" << endl;
      ss << "push dword " << expCode.location << endl;
      if(withNewLine && exp == expressionList->expressions.back()){
        ss << "push dword str_formatln" << endl;
      }else{
        ss << "push dword str_format" << endl;
      }
      ss << "call printf" << endl;
      ss << "add esp, 8" << endl;
      ss << "popad" << endl;
    }
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

  VarDescriptor* newVar = new VarDescriptor(varName, varType->typeCode, 1);

  scope->variables[varName] = newVar;
  return AsmCode();
}

AsmCode ArrayVarDeclStatement::GetAsm(Scope* scope){
  scope->AssertVariableDoesntExist(varName);

  VarDescriptor* newVar = new VarDescriptor(varName, varType->typeCode, values->getCount());

  scope->variables[varName] = newVar;

  return AsmCode();
}

AsmCode FuncDeclStatement::GetAsm(Scope* scope){
  functionScope = new Scope(scope, FunctionScopeT);
  //register pareters as if they were normal variables
  for(auto& param : params->paramList){
    VarDescriptor* newVar = new VarDescriptor(param->paramName, param->paramType->typeCode, 1);
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
