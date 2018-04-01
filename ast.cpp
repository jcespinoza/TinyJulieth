#include <cstdio>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include "ast.h"

int whetever = 0;

void JuliaDocument::Print() {
  for (auto& var : statements->statements) {
    if(var != NULL){
  		cout << "Statement of Type: " << var->getType() << endl;
    }else{
      cout << "Statement is null" << endl;
    }
	}
}
