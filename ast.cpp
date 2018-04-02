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

void JuliaDocument::FirstPass() {
  InitLabels();
  globalScope = new Scope(NULL, GlobalScopeT);
  for(auto& stm : statements->statements){
    int stmType = stm->getType();
    switch(stmType){
      case FuncDeclStm: {
        FuncDeclStatement* funcDecl = (FuncDeclStatement*)stm;
      }
      case ScVarDeclStm: {
        ScalarVarDeclStatement* varDecl = (ScalarVarDeclStatement*)stm;
      }
      case ArVarDeclStm: {
        ArrayVarDeclStatement* varDecl = (ArrayVarDeclStatement*)stm;
      }
      case AssiStm: {
        AssignStatement* assStm = (AssignStatement*)stm;
      }
      case ArrAssiStm: {
        ArrayItemAssignStatement* assStm = (ArrayItemAssignStatement*)stm;
      }
      case IfStm: {
        IfStatement* ifStm = (IfStatement*)stm;
      }
      case ForStm: {
        ForStatement* forStm = (ForStatement*)stm;
      }
      case WhileStm: {
        WhileStatement* whileStm = (WhileStatement*)stm;
      }
      default: {
        //Well, no such thing is possible to occur so...
        cout << "YOU FORGOT ON KIND OF EXPRESSION\n";
      }
    }
  }
  //Check which statements are variable declarations
  //When a variable is assigned a value, complain it doesn't exist if it hasn't been declared
  //Begin asking for expression type
  //Associate a type to each expression when found, they can be integer or boolean
  //attemp tp register variables anywhere they're found
}
