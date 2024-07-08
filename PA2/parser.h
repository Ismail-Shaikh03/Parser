/* 
 * parser(SP24).h
 * Programming Assignment 2
 * Spring 2024
*/

#ifndef PARSE_H_
#define PARSE_H_

#include <iostream>

using namespace std;

#include "lex.h"



extern bool Prog(istream& in, int& line);//done
extern bool Decl(istream& in, int& line);//done
extern bool Type(istream& in, int& line);//done
extern bool VarList(istream& in, int& line);//done
extern bool Stmt(istream& in, int& line);//done
extern bool SimpleStmt(istream& in, int& line);//done
extern bool PrintStmt(istream& in, int& line);//done
extern bool BlockIfStmt(istream& in, int& line);//done
extern bool SimpleIfStmt(istream& in, int& line);//done
extern bool AssignStmt(istream& in, int& line);//done
extern bool Var(istream& in, int& line);//done
extern bool ExprList(istream& in, int& line);//done
extern bool RelExpr(istream& in, int& line);//done
extern bool Expr(istream& in, int& line);//done
extern bool MultExpr(istream& in, int& line);//done
extern bool TermExpr(istream& in, int& line);//done
extern bool SFactor(istream& in, int& line);//done
extern bool Factor(istream& in, int& line, int sign);//done
extern int ErrCount();//done

#endif /* PARSE_H_ */
