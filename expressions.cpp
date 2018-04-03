#include <cstdio>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <cstdlib>
#include <stdexcept>
#include "expressions.h"

AsmCode NumExpression::GetAsm(Scope* scope){
  AsmCode asmCode;

  asmCode.location = to_string(value);
  asmCode.locationType = LiteralLocationType;

  return asmCode;
}

AsmCode BoolExpression::GetAsm(Scope* scope){

}

AsmCode IdExpression::GetAsm(Scope* scope){

}

AsmCode ArrayAccessExpression::GetAsm(Scope* scope){

}

AsmCode FuncCallExpression::GetAsm(Scope* scope){

}

AsmCode StrExpression::GetAsm(Scope* scope){
  stringstream ss;
  AsmCode asmCode;

  string label = scope->document->RegisterString(strValue);
  asmCode.location = label;
  asmCode.locationType = LabelLocationType;

  return asmCode;
}

AsmCode BitNotExpression::GetAsm(Scope* scope){

}

AsmCode LogNotExpression::GetAsm(Scope* scope){

}

AsmCode AddExpression::GetAsm(Scope* scope){

}

AsmCode SubExpression::GetAsm(Scope* scope){

}

AsmCode ShiftLeftExpression::GetAsm(Scope* scope){

}

AsmCode ShiftRightExpression::GetAsm(Scope* scope){

}

AsmCode BorExpression::GetAsm(Scope* scope){

}

AsmCode XorExpression::GetAsm(Scope* scope){

}

AsmCode DivExpression::GetAsm(Scope* scope){

}

AsmCode ModExpression::GetAsm(Scope* scope){

}

AsmCode MulExpression::GetAsm(Scope* scope){

}

AsmCode BandExpression::GetAsm(Scope* scope){

}

AsmCode PowExpression::GetAsm(Scope* scope){

}

AsmCode EquExpression::GetAsm(Scope* scope){

}

AsmCode NequExpression::GetAsm(Scope* scope){

}

AsmCode LthanExpression::GetAsm(Scope* scope){

}

AsmCode GthanExpression::GetAsm(Scope* scope){

}

AsmCode LeqExpression::GetAsm(Scope* scope){

}

AsmCode GeqExpression::GetAsm(Scope* scope){

}

AsmCode LorExpression::GetAsm(Scope* scope){

}

AsmCode LandExpression::GetAsm(Scope* scope){

}
