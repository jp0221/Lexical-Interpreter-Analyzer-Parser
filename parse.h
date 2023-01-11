/* 
 * parse.h
 * Programming Assignment 2
 * Spring 2022
*/

#ifndef PARSE_H_
#define PARSE_H_

#include <iostream>

using namespace std;

#include "lex.h"



extern bool Prog(istream& in, int& line);//done
extern bool DeclBlock(istream& in, int& line);//done
extern bool DeclStmt(istream& in, int& line);//done
extern bool ProgBody(istream& in, int& line);//done
extern bool Stmt(istream& in, int& line);//done
extern bool WriteLnStmt(istream& in, int& line);//done
extern bool IfStmt(istream& in, int& line);//done
extern bool ForStmt(istream& in, int& line);//done
extern bool AssignStmt(istream& in, int& line);//done
extern bool Var(istream& in, int& line);//done
extern bool ExprList(istream& in, int& line);//done
extern bool LogicExpr(istream& in, int& line);//done
extern bool Expr(istream& in, int& line);//done
extern bool Term(istream& in, int& line);//done
extern bool SFactor(istream& in, int& line);//done
extern bool Factor(istream& in, int& line, int sign);//done
extern int ErrCount();

#endif /* PARSE_H_ */
