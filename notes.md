switch(stmType){
  case FuncDeclStm: {
    FuncDeclStatement* funcDecl = (FuncDeclStatement*)stm;
    break;
  }
  case ScVarDeclStm: {
    ScalarVarDeclStatement* varDecl = (ScalarVarDeclStatement*)stm;
    break;
  }
  case ArVarDeclStm: {
    ArrayVarDeclStatement* varDecl = (ArrayVarDeclStatement*)stm;
    break;
  }
  case AssiStm: {
    AssignStatement* assStm = (AssignStatement*)stm;
    break;
  }
  case ArrAssiStm: {
    ArrayItemAssignStatement* assStm = (ArrayItemAssignStatement*)stm;
    break;
  }
  case IfStm: {
    IfStatement* ifStm = (IfStatement*)stm;
    break;
  }
  case ForStm: {
    ForStatement* forStm = (ForStatement*)stm;
    break;
  }
  case WhileStm: {
    WhileStatement* whileStm = (WhileStatement*)stm;
    break;
  }
  case PrintStm: {
    break;
  }
  case InvokeStm: {
    break;
  }
  default: {
    //Well, no such thing is possible to occur so...
    cout << "YOU FORGOT ON KIND OF STATEMENT: " << stmType << "\n";
  }
