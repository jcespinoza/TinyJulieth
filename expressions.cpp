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
  AsmCode asmCode; currentScope = scope;

  asmCode.location = to_string(value);
  asmCode.locationType = LiteralLocationType;

  return asmCode;
}

AsmCode BoolExpression::GetAsm(Scope* scope){
  AsmCode asmCode; currentScope = scope;

  asmCode.location = to_string(value);
  asmCode.locationType = LiteralLocationType;

  return asmCode;
}

AsmCode IdExpression::GetAsm(Scope* scope){
  AsmCode asmCode; currentScope = scope;
  stringstream ss;
  VarDescriptor* desc = scope->GetVariable(varName);

  if(desc->isGlobal){
    asmCode.PutIntoLabel("global_" + varName);
  }else if(desc->isParameter){
    string tReg = scope->document->RequestRegister();
    ss << "  mov " << tReg << ", dword [ebp+" << desc->offset << "]" << endl;
    asmCode.PutIntoRegister(tReg);
  } else{
    string tReg = scope->document->RequestRegister();
    ss << "  mov " << tReg << ", dword [ebp-" << desc->offset << "]" << endl;
    asmCode.PutIntoRegister(tReg);
  }

  asmCode.code = ss.str();
  return asmCode;
}

AsmCode ArrayAccessExpression::GetAsm(Scope* scope){
  AsmCode asmCode; currentScope = scope;
  stringstream ss;
  VarDescriptor* desc = scope->GetVariable(varName);
  AsmCode expCode = indexExpression->GetAsm(scope);

  string tReg = scope->document->RequestRegister();
  if(expCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(expCode.location);
  }
  ss << expCode.code;
  ss << "  mov " << tReg << ", dword " << expCode.GetValue32() << endl;
  ss << "  sub " << tReg << ", 1" << endl;
  ss << "  shl " << tReg << ", 2" << endl; //multiply by 4

  if(desc->isGlobal){
    ss << "  add " << tReg << ", global_" << varName << endl;
    ss << "  mov " << tReg << ", dword [" << tReg << "]" << endl;

    asmCode.PutIntoRegister(tReg);
  }else{

  }

  asmCode.code = ss.str();
  return asmCode;
}

AsmCode FuncCallExpression::GetAsm(Scope* scope){
  AsmCode asmCode; currentScope = scope;
  stringstream ss;
  auto iter = arguments->expressions.rbegin();
  for(; iter != arguments->expressions.rend(); ++iter){
    AsmCode expCode = (*iter)->GetAsm(scope);
    ss << expCode.code;
    if(expCode.locationType == RegisterLocationType){
      scope->document->FreeUpRegister(expCode.location);
    }
    ss << "  push dword " << expCode.GetValue32() << endl;
  }
  ss << "  call " << funcName << endl;
  ss << "  add esp, " << (arguments->getCount()*4) << endl;
  string tReg = scope->document->RequestRegister();
  ss << "  mov " << tReg << ", eax" << endl;

  asmCode.PutIntoRegister(tReg);
  asmCode.code = ss.str();
  return asmCode;
}

AsmCode StrExpression::GetAsm(Scope* scope){
  stringstream ss;
  AsmCode asmCode; currentScope = scope;

  string label = scope->document->RegisterString(strValue);
  asmCode.PutIntoLabel(label);

  return asmCode;
}

AsmCode BitNotExpression::GetAsm(Scope* scope){
  AsmCode asmCode; currentScope = scope;
  stringstream ss;

  AsmCode leftCode = targetExpression->GetAsm(scope);

  string tReg = scope->document->RequestRegister();
  if(leftCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(leftCode.location);
  }

  ss << leftCode.code;

  if(targetExpression->getExpType() == BoolType){
    string label = scope->document->GetLabelFor("bwnot");
    ss << "  mov eax, dword " << leftCode.GetValue32() << endl;
    ss << "  cmp eax, 0" << endl;
    ss << "  jne " << label << "_true" << endl;
    ss << "  nop" << endl;
    ss << "  mov " << tReg << ", dword 0" << endl;
    ss << "  jmp " << label << "_end" << endl;
    ss << "  nop" << endl;
    ss << label << "_true:" << endl;
    ss << "  mov " << tReg << ", dword 1" << endl;
    ss << label << "_end:" << endl;
  }else{
    ss << "  mov eax, dword " << leftCode.GetValue32() << endl;
    ss << "  not eax" << endl;
    ss << "  mov " << tReg << ", eax" << endl;
  }

  asmCode.PutIntoRegister( tReg );
  asmCode.code = ss.str();
  return asmCode;
}

AsmCode LogNotExpression::GetAsm(Scope* scope){
  AsmCode asmCode; currentScope = scope;
  stringstream ss;

  AsmCode leftCode = targetExpression->GetAsm(scope);

  string tReg = scope->document->RequestRegister();
  if(leftCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(leftCode.location);
  }

  ss << leftCode.code;
  ss << "  mov ax, 1111111111111111b" << endl;
  ss << "  cwd" << endl << "  cdq" << endl;
  ss << "  xor eax, dword " << leftCode.GetValue32() << endl;
  ss << "  mov " << tReg << ", eax" << endl;

  asmCode.PutIntoRegister( tReg );
  asmCode.code = ss.str();
  return asmCode;
}

AsmCode AddExpression::GetAsm(Scope* scope){
  AsmCode asmCode; currentScope = scope;
  stringstream ss;

  AsmCode leftCode = leftSide->GetAsm(scope);
  AsmCode rightCode = rightSide->GetAsm(scope);
  ss << leftCode.code;
  ss << rightCode.code;

  if(leftCode.IsLiteral() && rightCode.IsLiteral()){
    asmCode.PutLiteral( "("+ leftCode.GetValue32()+ "+"+ rightCode.GetValue32()+")" );
  }else if (leftCode.IsRegister() && rightCode.IsRegister()){
    ss << "  add " << leftCode.location << ", " << rightCode.location << endl;
    asmCode.PutIntoRegister(leftCode.location);
    scope->document->FreeUpRegister(rightCode.location);
  }else if(leftCode.IsRegister() ){
    ss << "  add " << leftCode.location << ", " << rightCode.location << endl;
    asmCode.PutIntoRegister(leftCode.location);
  }else if(rightCode.IsRegister()){
    ss << "  add " << rightCode.location << ", " << leftCode.location << endl;
    asmCode.PutIntoRegister(rightCode.location);
  }else{
    string tReg = scope->document->RequestRegister();

    ss << "  mov " << tReg << ", "  << leftCode.GetValue32() << endl;
    ss << "  add " << tReg << ", " << rightCode.GetValue32() << endl;
    asmCode.PutIntoRegister(tReg);
  }

  asmCode.code = ss.str();
  return asmCode;
}

AsmCode SubExpression::GetAsm(Scope* scope){
  AsmCode asmCode; currentScope = scope;
  stringstream ss;

  AsmCode leftCode = leftSide->GetAsm(scope);
  AsmCode rightCode = rightSide->GetAsm(scope);

  string tReg = scope->document->RequestRegister();
  if(leftCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(leftCode.location);
  }
  if(rightCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(rightCode.location);
  }

  ss << leftCode.code;
  ss << rightCode.code;
  ss << "  mov " << tReg << ", "  << leftCode.GetValue32() << endl;
  ss << "  sub " << tReg << ", " << rightCode.GetValue32() << endl;
  asmCode.PutIntoRegister( tReg );

  asmCode.code = ss.str();
  return asmCode;
}

AsmCode ShiftLeftExpression::GetAsm(Scope* scope){
  AsmCode asmCode; currentScope = scope;
  stringstream ss;

  AsmCode leftCode = leftSide->GetAsm(scope);
  AsmCode rightCode = rightSide->GetAsm(scope);

  string tReg = scope->document->RequestRegister();
  if(leftCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(leftCode.location);
  }
  if(rightCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(rightCode.location);
  }

  ss << leftCode.code;
  ss << rightCode.code;
  ss << "  mov " << tReg <<", " << leftCode.GetValue32() << endl;
  ss << "  shl " << tReg << ", " << rightCode.GetValue32() << endl;

  asmCode.PutIntoRegister( tReg );
  asmCode.code = ss.str();
  return asmCode;
}

AsmCode ShiftRightExpression::GetAsm(Scope* scope){
  AsmCode asmCode; currentScope = scope;
  stringstream ss;

  AsmCode leftCode = leftSide->GetAsm(scope);
  AsmCode rightCode = rightSide->GetAsm(scope);

  string tReg = scope->document->RequestRegister();
  if(leftCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(leftCode.location);
  }
  if(rightCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(rightCode.location);
  }

  ss << leftCode.code;
  ss << rightCode.code;
  ss << "  mov " << tReg <<", " << leftCode.GetValue32() << endl;
  ss << "  sar " << tReg << ", " << rightCode.GetValue32() << endl;

  asmCode.PutIntoRegister( tReg );
  asmCode.code = ss.str();
  return asmCode;
}

AsmCode BorExpression::GetAsm(Scope* scope){
  AsmCode asmCode; currentScope = scope;
  stringstream ss;

  AsmCode leftCode = leftSide->GetAsm(scope);
  AsmCode rightCode = rightSide->GetAsm(scope);

  string tReg = scope->document->RequestRegister();
  if(leftCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(leftCode.location);
  }
  if(rightCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(rightCode.location);
  }

  ss << leftCode.code;
  ss << rightCode.code;
  ss << "  mov " << tReg <<", " << leftCode.GetValue32() << endl;
  ss << "  or " << tReg << ", " << rightCode.GetValue32() << endl;

  asmCode.PutIntoRegister( tReg );
  asmCode.code = ss.str();
  return asmCode;
}

AsmCode XorExpression::GetAsm(Scope* scope){
  AsmCode asmCode; currentScope = scope;
  stringstream ss;

  AsmCode leftCode = leftSide->GetAsm(scope);
  AsmCode rightCode = rightSide->GetAsm(scope);

  string tReg = scope->document->RequestRegister();
  if(leftCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(leftCode.location);
  }
  if(rightCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(rightCode.location);
  }
  ss << leftCode.code;
  ss << rightCode.code;
  ss << "  mov " << tReg <<", " << leftCode.GetValue32() << endl;
  ss << "  xor " << tReg << ", " << rightCode.GetValue32() << endl;

  asmCode.PutIntoRegister( tReg );
  asmCode.code = ss.str();
  return asmCode;
}

AsmCode DivExpression::GetAsm(Scope* scope){
  AsmCode asmCode; currentScope = scope;
  stringstream ss;

  AsmCode leftCode = leftSide->GetAsm(scope);
  AsmCode rightCode = rightSide->GetAsm(scope);

  string tReg = scope->document->RequestRegister();
  if(leftCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(leftCode.location);
  }
  if(rightCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(rightCode.location);
  }

  ss << leftCode.code;
  ss << rightCode.code;
  ss << "  mov eax, dword " << leftCode.GetValue32() << endl;
  ss << "  mov " << tReg << ", dword " << rightCode.GetValue32() << endl;
  ss << "  cdq" << endl;
  ss << "  div " << tReg << endl;
  ss << "  mov " << tReg << ", eax" << endl;
  asmCode.PutIntoRegister( tReg );

  asmCode.code = ss.str();

  return asmCode;
}

AsmCode ModExpression::GetAsm(Scope* scope){
  AsmCode asmCode; currentScope = scope;
  stringstream ss;

  AsmCode leftCode = leftSide->GetAsm(scope);
  AsmCode rightCode = rightSide->GetAsm(scope);

  string tReg = scope->document->RequestRegister();
  if(leftCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(leftCode.location);
  }
  if(rightCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(rightCode.location);
  }
  ss << leftCode.code;
  ss << rightCode.code;
  ss << "  mov eax, dword " << leftCode.GetValue32() << endl;
  ss << "  mov " << tReg << ", dword " << rightCode.GetValue32() << endl;
  ss << "  cdq" << endl;
  ss << "  div " << tReg << endl;
  ss << "  mov " << tReg << ", edx" << endl;
  asmCode.PutIntoRegister( tReg );

  asmCode.code = ss.str();

  return asmCode;
}

AsmCode MulExpression::GetAsm(Scope* scope){
  AsmCode asmCode; currentScope = scope;
  stringstream ss;

  AsmCode leftCode = leftSide->GetAsm(scope);
  AsmCode rightCode = rightSide->GetAsm(scope);

  string tReg = scope->document->RequestRegister();
  if(leftCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(leftCode.location);
  }
  if(rightCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(rightCode.location);
  }
  ss << leftCode.code;
  ss << rightCode.code;
  ss << "  mov eax, dword " << leftCode.GetValue32() << endl;
  ss << "  mov " << tReg << ", dword " << rightCode.GetValue32() << endl;
  ss << "  cdq" << endl;
  ss << "  mul " << tReg << endl;
  ss << "  mov " << tReg << ", eax" << endl;
  asmCode.PutIntoRegister( tReg );

  asmCode.code = ss.str();

  return asmCode;
}

AsmCode BandExpression::GetAsm(Scope* scope){
  AsmCode asmCode; currentScope = scope;
  stringstream ss;

  AsmCode leftCode = leftSide->GetAsm(scope);
  AsmCode rightCode = rightSide->GetAsm(scope);

  string tReg = scope->document->RequestRegister();
  if(leftCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(leftCode.location);
  }
  if(rightCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(rightCode.location);
  }

  ss << leftCode.code;
  ss << rightCode.code;
  ss << "  mov " << tReg <<", " << leftCode.GetValue32() << endl;
  ss << "  and " << tReg << ", " << rightCode.GetValue32() << endl;

  asmCode.PutIntoRegister( tReg );
  asmCode.code = ss.str();
  return asmCode;
}

AsmCode PowExpression::GetAsm(Scope* scope){
  AsmCode asmCode; currentScope = scope;
  stringstream ss;

  AsmCode leftCode = leftSide->GetAsm(scope);
  AsmCode rightCode = rightSide->GetAsm(scope);

  string tReg = scope->document->RequestRegister();
  if(leftCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(leftCode.location);
  }
  if(rightCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(rightCode.location);
  }
  ss << leftCode.code;
  ss << rightCode.code;
  ss << "  push dword " << rightCode.GetValue32() << endl;
  ss << "  push dword " << leftCode.GetValue32() << endl;
  ss << "  call cpower" << endl;
  ss << "  add esp, 8" << endl;
  ss << "  mov " << tReg << ", eax" << endl;

  asmCode.PutIntoRegister( tReg );

  asmCode.code = ss.str();
  return asmCode;
}

AsmCode EquExpression::GetAsm(Scope* scope){
  AsmCode asmCode; currentScope = scope;
  stringstream ss;

  AsmCode leftCode = leftSide->GetAsm(scope);
  AsmCode rightCode = rightSide->GetAsm(scope);

  string tReg = scope->document->RequestRegister();
  if(leftCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(leftCode.location);
  }
  if(rightCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(rightCode.location);
  }

  ss << leftCode.code;
  ss << rightCode.code;
  ss << "  mov " << tReg <<", " << leftCode.GetValue32() << endl;
  ss << "  xor eax, eax" << endl;
  ss << "  cmp " << tReg
     << ", " << rightCode.GetValue32() << endl;
  ss << "  sete al" << endl;
  ss << "  mov " << tReg << ", eax" << endl;

  asmCode.PutIntoRegister( tReg );
  asmCode.code = ss.str();
  return asmCode;
}

AsmCode NequExpression::GetAsm(Scope* scope){
  AsmCode asmCode; currentScope = scope;
  stringstream ss;

  AsmCode leftCode = leftSide->GetAsm(scope);
  AsmCode rightCode = rightSide->GetAsm(scope);

  string tReg = scope->document->RequestRegister();
  if(leftCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(leftCode.location);
  }
  if(rightCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(rightCode.location);
  }

  ss << leftCode.code;
  ss << rightCode.code;
  ss << "  mov " << tReg <<", " << leftCode.GetValue32() << endl;
  ss << "  xor eax, eax" << endl;
  ss << "  cmp " << tReg
     << ", " << rightCode.GetValue32() << endl;
  ss << "  setne al" << endl;
  ss << "  mov " << tReg << ", eax" << endl;

  asmCode.PutIntoRegister( tReg );
  asmCode.code = ss.str();
  return asmCode;
}

AsmCode LthanExpression::GetAsm(Scope* scope){
  AsmCode asmCode; currentScope = scope;
  stringstream ss;

  AsmCode leftCode = leftSide->GetAsm(scope);
  AsmCode rightCode = rightSide->GetAsm(scope);

  string tReg = scope->document->RequestRegister();
  if(leftCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(leftCode.location);
  }
  if(rightCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(rightCode.location);
  }

  ss << leftCode.code;
  ss << rightCode.code;
  ss << "  mov " << tReg <<", " << leftCode.GetValue32() << endl;
  ss << "  xor eax, eax" << endl;
  ss << "  cmp " << tReg
     << ", " << rightCode.GetValue32() << endl;
  ss << "  setl al" << endl;
  ss << "  mov " << tReg << ", eax" << endl;

  asmCode.PutIntoRegister( tReg );
  asmCode.code = ss.str();
  return asmCode;
}

AsmCode GthanExpression::GetAsm(Scope* scope){
  AsmCode asmCode; currentScope = scope;
  stringstream ss;

  AsmCode leftCode = leftSide->GetAsm(scope);
  AsmCode rightCode = rightSide->GetAsm(scope);

  string tReg = scope->document->RequestRegister();
  if(leftCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(leftCode.location);
  }
  if(rightCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(rightCode.location);
  }

  ss << leftCode.code;
  ss << rightCode.code;
  ss << "  mov " << tReg <<", " << leftCode.GetValue32() << endl;
  ss << "  xor eax, eax" << endl;
  ss << "  cmp " << tReg
     << ", " << rightCode.GetValue32() << endl;
  ss << "  setg al" << endl;
  ss << "  mov " << tReg << ", eax" << endl;

  asmCode.PutIntoRegister( tReg );
  asmCode.code = ss.str();
  return asmCode;
}

AsmCode LeqExpression::GetAsm(Scope* scope){
  AsmCode asmCode; currentScope = scope;
  stringstream ss;

  AsmCode leftCode = leftSide->GetAsm(scope);
  AsmCode rightCode = rightSide->GetAsm(scope);

  string tReg = scope->document->RequestRegister();
  if(leftCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(leftCode.location);
  }
  if(rightCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(rightCode.location);
  }
  ss << leftCode.code;
  ss << rightCode.code;
  ss << "  mov " << tReg <<", " << leftCode.GetValue32() << endl;
  ss << "  xor eax, eax" << endl;
  ss << "  cmp " << tReg
     << ", " << rightCode.GetValue32() << endl;
  ss << "  setle al" << endl;
  ss << "  mov " << tReg << ", eax" << endl;

  asmCode.PutIntoRegister( tReg );
  asmCode.code = ss.str();
  return asmCode;
}

AsmCode GeqExpression::GetAsm(Scope* scope){
  AsmCode asmCode; currentScope = scope;
  stringstream ss;

  AsmCode leftCode = leftSide->GetAsm(scope);
  AsmCode rightCode = rightSide->GetAsm(scope);

  string tReg = scope->document->RequestRegister();
  if(leftCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(leftCode.location);
  }
  if(rightCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(rightCode.location);
  }

  ss << leftCode.code;
  ss << rightCode.code;
  ss << "  mov " << tReg <<", " << leftCode.GetValue32() << endl;
  ss << "  xor eax, eax" << endl;
  ss << "  cmp " << tReg
     << ", " << rightCode.GetValue32() << endl;
  ss << "  setge al" << endl;
  ss << "  mov " << tReg << ", eax" << endl;

  asmCode.PutIntoRegister( tReg );
  asmCode.code = ss.str();
  return asmCode;
}

AsmCode LorExpression::GetAsm(Scope* scope){
  AsmCode asmCode; currentScope = scope;
  stringstream ss;

  AsmCode leftCode = leftSide->GetAsm(scope);
  AsmCode rightCode = rightSide->GetAsm(scope);

  string tReg = scope->document->RequestRegister();
  if(leftCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(leftCode.location);
  }
  if(rightCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(rightCode.location);
  }

  ss << leftCode.code;
  ss << rightCode.code;
  ss << "  mov " << tReg <<", " << leftCode.GetValue32() << endl;
  ss << "  or " << tReg << ", " << rightCode.GetValue32() << endl;

  asmCode.PutIntoRegister( tReg );
  asmCode.code = ss.str();
  return asmCode;
}

AsmCode LandExpression::GetAsm(Scope* scope){
  AsmCode asmCode; currentScope = scope;
  stringstream ss;

  AsmCode leftCode = leftSide->GetAsm(scope);
  AsmCode rightCode = rightSide->GetAsm(scope);

  string tReg = scope->document->RequestRegister();
  if(leftCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(leftCode.location);
  }
  if(rightCode.locationType == RegisterLocationType){
    scope->document->FreeUpRegister(rightCode.location);
  }

  ss << leftCode.code;
  ss << rightCode.code;
  ss << "  mov " << tReg <<", " << leftCode.GetValue32() << endl;
  ss << "  and " << tReg << ", " << rightCode.GetValue32() << endl;

  asmCode.PutIntoRegister( tReg );
  asmCode.code = ss.str();
  return asmCode;
}

int ExpressionList::getExpType(){
  for(auto& exp : expressions){
    if(exp->getExpType() == IntType){
      return IntType;
    }
  }
  return BoolType;
}

int IdExpression::getExpType(){
  VarDescriptor* desc = currentScope->GetVariable(varName);
  return desc->typeCode;
}

int ArrayAccessExpression::getExpType(){
  VarDescriptor* desc = currentScope->GetVariable(varName);
  if(desc->typeCode == ArrayIntType) return IntType;
  if(desc->typeCode == ArrayBoolType) return BoolType;
  return desc->typeCode;
}

int FuncCallExpression::getExpType(){
  FuncDescriptor* desc = currentScope->document->functions[funcName];
  return desc->returnType;
}

int BinaryExpression::getExpType(){
  if(leftSide->getExpType() == IntType || rightSide->getExpType() == IntType){
    return IntType;
  }
  return leftSide->getExpType();
}
