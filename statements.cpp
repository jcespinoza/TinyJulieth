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

}

void WhileStatement::CheckSemantics(Scope* scope){

}

void ScalarVarDeclStatement::CheckSemantics(Scope* scope){

}

void ArrayVarDeclStatement::CheckSemantics(Scope* scope){

}

void FuncDeclStatement::CheckSemantics(Scope* scope){

}

void PassStatement::CheckSemantics(Scope* scope){

}

void ContinueStatement::CheckSemantics(Scope* scope){

}

void BreakStatement::CheckSemantics(Scope* scope){

}

void ReturnStatement::CheckSemantics(Scope* scope){

}
