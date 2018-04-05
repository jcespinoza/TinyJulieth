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

  asmCode.location = to_string(value);
  asmCode.locationType = LiteralLocationType;

  return asmCode;
}

AsmCode IdExpression::GetAsm(Scope* scope){
  AsmCode asmCode;
  stringstream ss;
  if(scope->IsGlobal()){
    asmCode.locationType = LabelLocationType;
    asmCode.location = "global_" + varName;
  }

  asmCode.code = ss.str();
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
  stringstream ss;

  AsmCode leftCode = leftSide->GetAsm(scope);
  AsmCode rightCode = rightSide->GetAsm(scope);

  if(leftCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(leftCode.location);
  }
  if(rightCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(rightCode.location);
  }
  string tReg = scope->document->RequestRegister();
  ss << "  mov " << tReg << ", "  << leftCode.GetValue32() << endl;
  ss << "  add " << tReg << ", " << rightCode.GetValue32() << endl;
  asmCode.location = tReg;
  asmCode.locationType = RegisterLocationType;

  asmCode.code = ss.str();
  return asmCode;
}

AsmCode SubExpression::GetAsm(Scope* scope){
  AsmCode asmCode;
  stringstream ss;

  AsmCode leftCode = leftSide->GetAsm(scope);
  AsmCode rightCode = rightSide->GetAsm(scope);

  if(leftCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(leftCode.location);
  }
  if(rightCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(rightCode.location);
  }
  string tReg = scope->document->RequestRegister();
  ss << "  mov " << tReg << ", "  << leftCode.GetValue32() << endl;
  ss << "  sub " << tReg << ", " << rightCode.GetValue32() << endl;
  asmCode.location = tReg;
  asmCode.locationType = RegisterLocationType;

  asmCode.code = ss.str();
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
  stringstream ss;

  AsmCode leftCode = leftSide->GetAsm(scope);
  AsmCode rightCode = rightSide->GetAsm(scope);

  if(leftCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(leftCode.location);
  }
  if(rightCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(rightCode.location);
  }
  string tReg = scope->document->RequestRegister();
  ss << leftCode.code;
  ss << rightCode.code;
  ss << "  mov eax, dword " << leftCode.GetValue32() << endl;
  ss << "  mov " << tReg << ", dword " << rightCode.GetValue32() << endl;
  ss << "  cdq" << endl;
  ss << "  div " << tReg << endl;
  ss << "  mov eax, " << tReg << endl;
  asmCode.location = tReg;
  asmCode.locationType = RegisterLocationType;
  asmCode.code = ss.str();

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
