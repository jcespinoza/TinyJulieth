#include <cstdio>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <cstdlib>
#include <stdexcept>
#include "statements.h"

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

void StatementList::CheckSemantics(Scope* scope){
  for(auto& stm : statements){
    if(stm != NULL){
      stm->currentScope = scope;
      stm->CheckSemantics(scope);
    }
  }
}

void AssignStatement::CheckSemantics(Scope* scope){

}

void ArrayItemAssignStatement::CheckSemantics(Scope* scope){

}

void PrintStatement::CheckSemantics(Scope* scope){

}

void IfStatement::CheckSemantics(Scope* scope){

}

void ForStatement::CheckSemantics(Scope* scope){
  // Register counter variable
  label_begin = scope->document->GetLabelFor("for");
  localScope = new Scope(scope, ForScopeT);
  statements->CheckSemantics(localScope);
}

void WhileStatement::CheckSemantics(Scope* scope){
  label_begin = scope->document->GetLabelFor("while");
  localScope = new Scope(scope, ForScopeT);
  statements->CheckSemantics(localScope);
}

void ScalarVarDeclStatement::CheckSemantics(Scope* scope){
  scope->AssertVariableDoesntExist(varName);

  VarDescriptor* newVar = new VarDescriptor(varName, varType->typeCode, sizeof(int));

  scope->variables[varName] = newVar;
}

void ArrayVarDeclStatement::CheckSemantics(Scope* scope){
  scope->AssertVariableDoesntExist(varName);

  VarDescriptor* newVar = new VarDescriptor(varName, varType->typeCode, values->getCount()*sizeof(int));

  scope->variables[varName] = newVar;
}

void FuncDeclStatement::CheckSemantics(Scope* scope){
  functionScope = new Scope(scope, FunctionScopeT);
  statements->CheckSemantics(functionScope);
}

void InvokeStatement::CheckSemantics(Scope* scope){
  scope->document->AsssertFunctionExists(funcName);
}

void ContinueStatement::CheckSemantics(Scope* scope){
  scope->AssertIsInLoop("Continue statement not valid in this context.\n");
}

void BreakStatement::CheckSemantics(Scope* scope){
  scope->AssertIsInLoop("Break statement not valid in this context.\n");
}

void ReturnStatement::CheckSemantics(Scope* scope){
  //Check the current scope is not the global scope
  if(currentScope->scopeType == GlobalScopeT){
    throw runtime_error("Return statement is not valid in the global scope.\n");
  }
  scope->AssertIsInFunction("Return statements are only valid inside a function.\n");
}
