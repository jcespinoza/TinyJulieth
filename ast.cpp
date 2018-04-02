#include <cstdio>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include "ast.h"

int whetever = 0;

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

void JuliaDocument::FirstPass() {
  InitLabels();
  //attemp tp register variables anywhere they're found
}

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
