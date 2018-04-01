#ifndef AST_H
#define AST_H

enum StatemetTypes{
  WhileStm,
  ForStm,
  IfStm,
  AssiStm,
  VarDeclStm,
  ArrDeclStm,
  FuncDeclStm,
  ExprStm,
  PrintStm,

  StmListStm,
};

enum ExpressionTypes{
  NumExp,
  IdExp,
  ArrAccExp,
  FCallExp,
  BoolExp,

  DivExp,
  ModExp,
  MulExp,
  LorExp,
  LndExp,

  AddExp,
  SubExp,
  ShlExp,
  ShrExp,

  PowExp,
  NotExp,
  BitNotExp,
  StrExp,

  LthExp,
  GthExp,
  LeqExp,
  GeqExp,
  NeqExp,
  EquExp
};

class Expression {
public:
  virtual int getType() = 0;
};

class ExpressionList {
public:
  ExpressionList(){ }
  void AddNew(Expression* exp){
    exps.push_front(exp);
  }

  std::list<Expression*> exps;
};

class Statement {
public:
  virtual int getType() = 0;
};

class StatementList : public Statement{
public:
  StatementList(){ }

  void AddNew(Statement* stm){
    stms.push_front(stm);
  }

  int getType(){ return StmListStm; }

  std::list<Statement*> stms;
};

#endif
