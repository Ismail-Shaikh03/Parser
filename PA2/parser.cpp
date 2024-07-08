
#include "parser.h"
#include "lex.h"

map<string, bool> defVar;
map<string, Token> SymTable;

namespace Parser {
	bool pushed_back = false;
	LexItem	pushed_token;

	static LexItem GetNextToken(istream& in, int& line) {
		if (pushed_back) {
			pushed_back = false;
			return pushed_token;
		}
		return getNextToken(in, line);
	}

	static void PushBackToken(LexItem& t) {
		if (pushed_back) {
			abort();
		}
		pushed_back = true;
		pushed_token = t;
	}
}

static int error_count = 0;
int nestedBlockIF = 0;
bool varlist = false;
string len = "";
string initialVar = "";

int ErrCount()
{
	return error_count;
}

void ParseError(int line, string msg)
{
	++error_count;
	cout << line << ": " << msg << endl;
}

bool IdentList(istream& in, int& line);

//PrintStmt:= PRINT *, ExpreList 
bool PrintStmt(istream& in, int& line) {
	LexItem t;

	t = Parser::GetNextToken(in, line);

	if (t != DEF) {

		ParseError(line, "Print statement syntax error.");
		return false;
	}
	t = Parser::GetNextToken(in, line);

	if (t != COMMA) {

		ParseError(line, "Missing Comma.");
		return false;
	}
	bool ex = ExprList(in, line);

	if (!ex) {
		ParseError(line, "Missing expression after Print Statement");
		return false;
	}
	return ex;
}//End of PrintStmt

//ExprList:= Expr {,Expr}
bool ExprList(istream& in, int& line) {
	bool status = false;

	status = Expr(in, line);
	if (!status) {
		ParseError(line, "Missing Expression");
		return false;
	}

	LexItem tok = Parser::GetNextToken(in, line);

	if (tok == COMMA) {
		status = ExprList(in, line);
	}
	else if (tok.GetToken() == ERR) {
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else {
		Parser::PushBackToken(tok);
		return true;
	}
	return status;
}//End of ExprList

//Expr::=MultExpr {(+ | - | //) MultExpr}
bool Expr(istream& in, int& line) {
	//cout << "Expr" << endl; Used to debug
	LexItem token;
	if (MultExpr(in, line)) {
		while (true) {
			token = Parser::GetNextToken(in, line);
			if (token == PLUS || token == MINUS || token == CAT) {
				if (MultExpr(in, line)) {
					continue;
				}
				else {
					return false;
				}
			}
			else {
				Parser::PushBackToken(token);
				break;
			}
		}
	}
	else {
		ParseError(line, "Missing MultExpr");
		return false;
	}
	return true;
}
//Prog::=PROGRAM IDENT {Decl} {Stmt} END PROGRAM IDENT
bool Prog(istream& in, int& line) {
	//cout << "Prog" << endl; Used to debug
	LexItem token;
	token = Parser::GetNextToken(in, line);
	if (token == PROGRAM) {
		token = Parser::GetNextToken(in, line);
		if (token == IDENT) {
			//Decl while loop
			while (true) {
				token = Parser::GetNextToken(in, line);
				if (token == INTEGER || token == REAL || token == CHARACTER) {
					Parser::PushBackToken(token);
					if (!Decl(in, line)) {
						ParseError(line, "Declare fails, fix");
						return false;
					}
				}
				else {
					Parser::PushBackToken(token);
					break;
				}
			}

			//Stmt while loop 
			while (true) {
				token = Parser::GetNextToken(in, line);
				if (token == END) {
					Parser::PushBackToken(token);
					break;
				}
				else {
					Parser::PushBackToken(token);
					if (!Stmt(in, line)) {
						ParseError(line, "InProg STMT something is wrong");
						return false;
					}
				}
			}

			token = Parser::GetNextToken(in, line);
			if (token == END) {
				token = Parser::GetNextToken(in, line);
				if (token == PROGRAM) {
					token = Parser::GetNextToken(in, line);
					if (token == IDENT) {
						cout << "(DONE)" << endl;
						return true;
					}
					else {
						ParseError(line, "Missing IDENT statement");
					}
				}
				else {
					ParseError(line, "Missing Program statement");
				}
			}
			else {
				ParseError(line, "Missing END statement");
			}
		}
		else {
			ParseError(line, "Missing IDENT statement");
		}
	}
	else {
		ParseError(line, "Missing PROGRAM statement");
	}
	return false;
}//end of Prog
//Decl::= Type :: VarList
bool Decl(istream& in, int& line) {
	//cout << "Decl" << endl; Used to debug
	LexItem token;
	if (Type(in, line)) {
		//cout << token << endl; Used to debug
		token = Parser::GetNextToken(in, line);
		//cout << token << endl; Used to debug
		if (token == DCOLON) {
			//token=Parser::GetNextToken(in, line);
			if (VarList(in, line)) {
				if (!len.empty()) {
					cout << "Definition of Strings with length of " << len << " in declaration statement." << endl;
					len = "";

				}
				return true;
			}
		}
		else {
			ParseError(line, "Missing DCOLON");
			return false;
		}
	}
	else {
		ParseError(line, "Missing TYPE in Decl");
		return false;
	}
	return false;
}
//Type::= INTEGER || REAL || CHARACTER[(Len=ICONST)]
bool Type(istream& in, int& line) {
	//cout << "Type" << endl; Used to debug
	LexItem token;
	token = Parser::GetNextToken(in, line);
	//cout << token << endl; Used to debug
	if (token == INTEGER || token == REAL) {
		return true;
	}
	else if (token == CHARACTER) {
		token = Parser::GetNextToken(in, line);
		if (token == LPAREN) {
			token = Parser::GetNextToken(in, line);
			if (token == LEN) {
				token = Parser::GetNextToken(in, line);
				if (token == ASSOP) {
					token = Parser::GetNextToken(in, line);
					if (token == ICONST) {
						len = token.GetLexeme();
						token = Parser::GetNextToken(in, line);
						if (token == RPAREN) {
							return true;
						}
					}
					else {
						ParseError(line, "MISSING ICONST");
					}
				}
				else {
					ParseError(line, "MISSING ASSOP");
				}
			}
			else {
				ParseError(line, "Missing LEN");
			}
		}
		else {
			Parser::PushBackToken(token);
			return true;
		}
	}
	else {
		ParseError(line, "Missing TYPE in Type");
		return false;
	}
	return false;
}

//VarList::= Var[=Expr] {, Var[=Expr]}
bool VarList(istream& in, int& line) {
	varlist = true;
	//cout << "VarList" << endl; Used to debug
	if (!Var(in, line)) {
		ParseError(line, "Missing Expression");
		return false;
	}

	LexItem token = Parser::GetNextToken(in, line);

	if (token == ASSOP) {
		if (!Expr(in, line)) {
			ParseError(line, "EJERWNJNI");
			return false;
		}
		else {
			cout << "Initialization of the variable " << initialVar << " in the declaration statement." << endl;
		}

	}
	else {
		Parser::PushBackToken(token);
	}

	while (true) {
		token = Parser::GetNextToken(in, line);
		if (token == COMMA) {
			if (Var(in, line)) {
				token = Parser::GetNextToken(in, line);
				if (token == ASSOP) {
					if (!Expr(in, line)) {
						ParseError(line, "uh oh");
						return false;
					}
					else {
						cout << "Initialization of the variable " << initialVar << " in the declaration statement." << endl;
					}
				}
				else {
					Parser::PushBackToken(token); // is not ASSOP push the Token back
				}
			}
			else {
				ParseError(line, "In VarList expected Var to work after COMMA so fix");
				return false;
			}
		}
		else {
			Parser::PushBackToken(token);
			break;
		}
	}
	varlist = false;
	return true;
}//End of VarList
/* Original VarList that has issues. Decided to redo this part.
bool VarList(istream& in, int& line){
	LexItem token;
	if(Var(in,line)){
		token=Parser::GetNextToken(in, line);
		if(token==ASSOP){
			if(Expr(in,line)){
				token=Parser::GetNextToken(in, line);
				if(token==COMMA){
					while(Var(in,line)){
						return true;
					}
				}
			}else{
				ParseError(line,"Missing Expression");
			}
		}else{
			ParseError(line,"Missing ASSOP");
		}
	}
	return false;
}*/
//Var::=IDENT
bool Var(istream& in, int& line) {
	//cout << "Var" << endl; Used to debug
	LexItem token;
	token = Parser::GetNextToken(in, line);
	if (token == IDENT) {
		if (defVar.find(token.GetLexeme()) != defVar.end() && varlist == true) {
			ParseError(line, "Variable Redefinition");
			return false;
		}
		else {
			initialVar = token.GetLexeme();
			defVar[token.GetLexeme()] = false;
		}
		return true;
	}
	return false;
}
//Stmt::=AssignStmt || BlockIfStmt || Printstmt || SimpleIfStmt
bool Stmt(istream& in, int& line) {
	//cout << "Stmt" << endl; Used to debug
	LexItem token;
	token = Parser::GetNextToken(in, line);
	if (token == IDENT) {
		Parser::PushBackToken(token);
		if (AssignStmt(in, line)) {
			return true;
		}
		else {
			ParseError(line, "AssignStmt failed fix");
			return false;
		}
	}
	else if (token == IF) {
		token = Parser::GetNextToken(in, line);
		if (token == LPAREN) {
			//token = Parser::GetNextToken(in, line);
			if (RelExpr(in, line)) {
				token = Parser::GetNextToken(in, line);
				if (token == RPAREN) {
					token = Parser::GetNextToken(in, line);
					if (token == THEN) {
						nestedBlockIF = nestedBlockIF + 1;
						if (BlockIfStmt(in, line)) {
							return true;
						}
						else {
							return false;
						}
					}
					else {
						Parser::PushBackToken(token);
						if (SimpleIfStmt(in, line)) {
							return true;
						}
						else {
							ParseError(line, "SIMPLE IF STATMENT DID NOT PASSSSSSS");
							return false;
						}
					}
				}
				else {
					ParseError(line, "Missing RPAREN IN STMT CHECKING, so FIX");
				}
			}
			else {
				ParseError(line, "MISSING RELEXPR IN STMT CHECKING, so FIX");
			}
		}
		else {
			ParseError(line, "MISSING LPAREN IN STMT CHECKING, so FIX");
		}

	}
	else if (token == PRINT) {
		if (PrintStmt(in, line)) {
			return true;
		}
		else {
			ParseError(line, "PRINT STMT did not work, fix");
			return false;
		}
	}
	else {
		ParseError(line, "Something is wrong in Stmt");
		return false;
	}
	return false;
}
//AssignStmt::= Var= Expr
bool AssignStmt(istream& in, int& line) {
	//cout << "AssignStmt" << endl; Used to debug
	LexItem token;
	//token=Parser::GetNextToken(in, line);
	if (Var(in, line)) {
		token = Parser::GetNextToken(in, line);
		if (token == ASSOP) {
			//token=Parser::GetNextToken(in, line);
			if (Expr(in, line)) {
				return true;
			}
			else {
				ParseError(line, "Missing Expr in AssignStmt");
				return false;
			}
		}
		else {
			ParseError(line, "Missing ASSOP");
		}
	}
	else {
		ParseError(line, "Missing VAR");
	}
	return false;
}
//Factor::=IDENT | ICONST | RCONST | SCONST | (Expr)
bool Factor(istream& in, int& line, int sign) {
	//cout << "Factor" << endl; Used to debug
	LexItem token;
	token = Parser::GetNextToken(in, line);
	if (token == IDENT || token == ICONST || token == RCONST || token == SCONST) {
		if (token == IDENT && defVar.find(token.GetLexeme()) == defVar.end()) {
			ParseError(line, "Using Undefined Variable");
			return false;
		}
		return true;
	}
	else if (token == LPAREN) {
		//token=Parser::GetNextToken(in,line);
		if (Expr(in, line)) {
			token = Parser::GetNextToken(in, line);
			if (token == RPAREN) {
				return true;
			}
			else {
				ParseError(line, "Missing RPAREN");
			}
		}
		else {
			ParseError(line, "Missing Expr");
		}
	}
	else {
		return false;
	}
	return false;
}
bool SFactor(istream& in, int& line) {
	//cout << "SFactor" << endl; Used to debug
	LexItem token;
	token = Parser::GetNextToken(in, line);
	if (token == PLUS || token == MINUS) {
		if (Factor(in, line, 1)) {
			return true;
		}
		else {
			ParseError(line, "Missing Factor");
			return false;
		}
	}
	else {
		Parser::PushBackToken(token);
		if (Factor(in, line, 1)) {
			return true;
		}
		else {
			ParseError(line, "Missing Factor");
			return false;
		}
	}
	return false;
}
bool TermExpr(istream& in, int& line) {
	//cout << "TermExpr" << endl; Used to debug
	LexItem token;
	if (SFactor(in, line)) {
		while (true) {
			token = Parser::GetNextToken(in, line);
			if (token == POW) {
				if (!SFactor(in, line)) {
					return false;
				}

			}
			else {
				Parser::PushBackToken(token);
				break;
			}
		}


	}
	else {
		ParseError(line, "SFACTOR BROKEN  IN TERMEXPR");
		return false;
	}
	return true;
}
bool MultExpr(istream& in, int& line) {
	//cout << "MultExpr" << endl; Used to debug
	LexItem token;
	if (TermExpr(in, line)) {
		while (true) {
			token = Parser::GetNextToken(in, line);
			if (token == MULT || token == DIV) {
				if (!TermExpr(in, line)) {
					return false;
				}
			}
			else {
				Parser::PushBackToken(token);
				break;
			}
		}


	}
	else {
		ParseError(line, "TERMEXPR BROKEN  IN MULTEXPR");
		return false;
	}
	return true;
}
bool RelExpr(istream& in, int& line) {
	//cout << "RelExpr" << endl; Used to debug
	LexItem token;
	if (Expr(in, line)) {
		token = Parser::GetNextToken(in, line);
		if (token == EQ || token == LTHAN || token == GTHAN) {
			if (Expr(in, line)) {
				return true;
			}
			else {
				ParseError(line, "Missing Expr in RealExpr");
				return false;
			}
		}
		else {
			Parser::PushBackToken(token);
		}
	}
	return false;
}
bool SimpleStmt(istream& in, int& line) {
	//cout << "SimpleStmt" << endl; Used to debug

	LexItem token  = Parser::GetNextToken(in, line);
	if (token == PRINT) 
	{
		cout << "Print statement in a Simple If statement." << endl;

		if (PrintStmt(in, line)) {
			return true;
		}
		else {
			ParseError(line, "IN SimpleStmt Expected PrintStmt To Work so fix");
			return false;
		}
	}
	//else check AssignStmt passes'
	else
	{
		cout << "Assignment statement in a Simple If statement." << endl;
		//cout << nextToken << endl;
		Parser::PushBackToken(token);
		if (AssignStmt(in, line)) {
			return true;
		}
		else {
			ParseError(line, "IN SimpleStmt Expected AssignStmt To Work so fix");
			return false;
		}
	}
	return true;
}
bool SimpleIfStmt(istream& in, int& line) {
	//cout << "Simple IF" << endl; Used to debug
	if (SimpleStmt(in, line)) {
		return true;
	}
	else {
		return false;
	}
	return true;
}
bool BlockIfStmt(istream& in, int& line) {
	//cout << "BlockIfStmt" << endl; Used to debug
	LexItem token;
	while (true) {
		token = Parser::GetNextToken(in, line);
		if (token == END || token == ELSE) {
			Parser::PushBackToken(token);
			break;
		}
		else {
			Parser::PushBackToken(token);
			if (!Stmt(in, line)) {
				return false;
				ParseError(line, "Missing STMT BLOCK IF STATEMENT");
			}
		}
	}
	token = Parser::GetNextToken(in, line);
	if (token == ELSE) {
		while (true) {
			token = Parser::GetNextToken(in, line);
			if (token == END) {
				Parser::PushBackToken(token);
				break;
			}
			else {
				Parser::PushBackToken(token);
				if (!Stmt(in, line)) {
					return false;
					ParseError(line, "Missing STMT BLOCK IF STATEMENT");
				}
			}
		}

	}
	else {
		Parser::PushBackToken(token);
	}
	token = Parser::GetNextToken(in, line);
	if (token == END) {
		token = Parser::GetNextToken(in, line);
		if (token == IF) {
			cout << "End of Block If statement at nesting level " << nestedBlockIF << endl;
			nestedBlockIF = nestedBlockIF - 1;
			return true;
		}
		else {
			ParseError(line, "Missing IF in BlockIFStmt fix");
		}
	}
	else {
		ParseError(line, "Missing END in BlockIFStmt fix");
	}
	return false;
}