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
  AsmCode asmCode;

  return asmCode;
}

AsmCode IdExpression::GetAsm(Scope* scope){
  AsmCode asmCode;

  return asmCode;
}

AsmCode ArrayAccessExpression::GetAsm(Scope* scope){
  AsmCode asmCode;

  return asmCode;
}

AsmCode FuncCallExpression::GetAsm(Scope* scope){
  AsmCode asmCode;

  return asmCode;
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
  AsmCode asmCode;

  return asmCode;
}

AsmCode LogNotExpression::GetAsm(Scope* scope){
  AsmCode asmCode;

  return asmCode;
}

AsmCode AddExpression::GetAsm(Scope* scope){
  AsmCode asmCode;

  return asmCode;
}

AsmCode SubExpression::GetAsm(Scope* scope){
  AsmCode asmCode;

  return asmCode;
}

AsmCode ShiftLeftExpression::GetAsm(Scope* scope){
  AsmCode asmCode;

  return asmCode;
}

AsmCode ShiftRightExpression::GetAsm(Scope* scope){
  AsmCode asmCode;

  return asmCode;
}

AsmCode BorExpression::GetAsm(Scope* scope){
  AsmCode asmCode;

  return asmCode;
}

AsmCode XorExpression::GetAsm(Scope* scope){
  AsmCode asmCode;

  return asmCode;
}

AsmCode DivExpression::GetAsm(Scope* scope){
  AsmCode asmCode;

  return asmCode;
}

AsmCode ModExpression::GetAsm(Scope* scope){
  AsmCode asmCode;

  return asmCode;
}

AsmCode MulExpression::GetAsm(Scope* scope){
  AsmCode asmCode;

  return asmCode;
}

AsmCode BandExpression::GetAsm(Scope* scope){
  AsmCode asmCode;

  return asmCode;
}

AsmCode PowExpression::GetAsm(Scope* scope){
  AsmCode asmCode;

  return asmCode;
}

AsmCode EquExpression::GetAsm(Scope* scope){
  AsmCode asmCode;

  return asmCode;
}

AsmCode NequExpression::GetAsm(Scope* scope){
  AsmCode asmCode;

  return asmCode;
}

AsmCode LthanExpression::GetAsm(Scope* scope){
  AsmCode asmCode;

  return asmCode;
}

AsmCode GthanExpression::GetAsm(Scope* scope){
  AsmCode asmCode;

  return asmCode;
}

AsmCode LeqExpression::GetAsm(Scope* scope){
  AsmCode asmCode;

  return asmCode;
}

AsmCode GeqExpression::GetAsm(Scope* scope){
  AsmCode asmCode;

  return asmCode;
}

AsmCode LorExpression::GetAsm(Scope* scope){
  AsmCode asmCode;

  return asmCode;
}

AsmCode LandExpression::GetAsm(Scope* scope){
  AsmCode asmCode;

  return asmCode;
}
