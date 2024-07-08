//#include "parser.h"

map<string, bool> defVar;
map<string, Token> SymTable;

namespace Parser {
	bool pushed_back = false;
	LexItem	pushed_token;

	static LexItem GetNextToken(istream& in, int& line) {
		if( pushed_back ) {
			pushed_back = false;
			return pushed_token;
		}
		return getNextToken(in, line);
	}

	static void PushBackToken(LexItem & t) {
		if( pushed_back ) {
			abort();
		}
		pushed_back = true;
		pushed_token = t;	
	}

}

static int error_count = 0;

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
	
 	if( t != DEF ) {
		
		ParseError(line, "Print statement syntax error.");
		return false;
	}
	t = Parser::GetNextToken(in, line);
	
	if( t != COMMA ) {
		
		ParseError(line, "Missing Comma.");
		return false;
	}
	bool ex = ExprList(in, line);
	
	if( !ex ) {
		ParseError(line, "Missing expression after Print Statement");
		return false;
	}
	return ex;
}//End of PrintStmt


//ExprList:= Expr {,Expr}
bool ExprList(istream& in, int& line) {
	bool status = false;
	
	status = Expr(in, line);
	if(!status){
		ParseError(line, "Error Pattern UnRecognized");
		return false;
	}
	
	LexItem tok = Parser::GetNextToken(in, line);
	
	if (tok == COMMA) {
		status = ExprList(in, line);
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else{
		Parser::PushBackToken(tok);
		return true;
	}
	return status;
}//End of ExprList
//Expr::=MultExpr {(+ | - | //) MultExpr}
bool Expr(istream& in, int& line){
	LexItem token;
	if(MultExpr(in,line)){
			while(true){
				token=Parser::GetNextToken(in,line);
				if(token==PLUS || token== MINUS || token==CAT){
					if(MultExpr(in,line)){
						continue;
					}else{
						return false;
					}
				}else{
					Parser::PushBackToken(token);
					break;
				}
			}
	}else{
		ParseError(line,"Missing MultExpr");
	}
	return true;
}
//Prog::=PROGRAM IDENT {Decl} {Stmt} END PROGRAM IDENT
bool Prog(istream& in, int& line){
	LexItem token;
	token=Parser::GetNextToken(in, line);
	if(token==PROGRAM){
		token=Parser::GetNextToken(in, line);
		if(token==IDENT){
			while(true){
				token=Parser::GetNextToken(in, line);
				if(token==INTEGER || token==REAL || token==CHARACTER){
					Parser::PushBackToken(token);
					if(!Decl(in,line)){
						ParseError(line,"DECLARE MUS BE STATED");
						return false;
					}
				}else{
					Parser::PushBackToken(token);
					break;
				}
			}
			while(true){
				token=Parser::GetNextToken(in,line);
				if(token==END){
					Parser::PushBackToken(token);
					break;
				}else{
					Parser::PushBackToken(token);
					if(!Stmt(in,line)){
						ParseError(line,"InProg STMT");
						return false;
					}
				}
			}
			token=Parser::GetNextToken(in, line);
			if(token==END){
				token=Parser::GetNextToken(in, line);
				if(token==PROGRAM){
				 token=Parser::GetNextToken(in, line);
					if(token==IDENT){
						cout<<"(DONE)"<<endl;
						return true;
					}else{
						ParseError(line,"Missing IDENT statement");
					}
				}else{
					ParseError(line,"Missing Program statement");
				}
			}else{
				ParseError(line,"Missing END statement");
			}
		}else{
			ParseError(line,"Missing IDENT statement");
		}
	}else{
		ParseError(line,"Missing PROGRAM statement");
	}
	return false;
}//end of Prog
//Decl::= Type :: VarList
bool Decl(istream& in, int& line){
	LexItem token;
	if(Type(in,line)){
		token=Parser::GetNextToken(in, line);
		if(token==DCOLON){
			//token=Parser::GetNextToken(in, line);
			if(VarList(in,line)){
				return true;
			}
		}else{
			ParseError(line,"Missing DCOLON");
			return false;
		}
	}else{
		ParseError(line,"Missing TYPE in Decl");
		return false;
	}
	return false;
}
//Type::= INTEGER || REAL || CHARACTER[(Len=ICONST)]
bool Type(istream& in, int& line){
	LexItem token; 
	token=Parser::GetNextToken(in, line);
	if(token==INTEGER || token== REAL){
		return true;
	}else if(token==CHARACTER){
		token=Parser::GetNextToken(in, line);
		if(token==LPAREN){
			token=Parser::GetNextToken(in, line);
			if(token==LEN){
				token=Parser::GetNextToken(in, line);
				if(token==ASSOP){
					token=Parser::GetNextToken(in, line);
					if(token==ICONST){
						return true;
					}else{
						ParseError(line,"MISSING ICONST");
					}
				}else{
					ParseError(line,"MISSING ASSOP");
				}
			}else{
				ParseError(line,"Missing LEN");
			}
		}else{
			Parser::PushBackToken(token);
		}
	}else{
		ParseError(line,"Missing TYPE in Type");
		return false;
	}
return false;
}

//VarList::= Var[=Expr] {, Var[=Expr]}
bool VarList(istream& in, int& line) {
	bool status = false;
	
	status = Var(in, line);
	if(!status){
		ParseError(line, "Missing Expression");
		return false;
	}
	
	LexItem tok = Parser::GetNextToken(in, line);
	
	if (tok == COMMA) {
		status = VarList(in, line);
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else{
		Parser::PushBackToken(tok);
		return true;
	}
	return status;
}//End of VarList
/*bool VarList(istream& in, int& line){
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
bool Var(istream& in, int& line){
	LexItem token; 
	token=Parser::GetNextToken(in, line);
	if(token==IDENT){
		return true;
	}
	return false;
}
//Stmt::=AssignStmt || BlockIfStmt || Printstmt || SimpleIfStmt
bool Stmt(istream& in, int& line){
	LexItem token; 
	token=Parser::GetNextToken(in, line);
	if(token==IDENT){
		Parser::PushBackToken(token);
		if(AssignStmt(in,line)){
			return true;
		}else{
			return false;
		}
	}else if(token==IF){
		token=Parser::GetNextToken(in,line);
		if(token==LPAREN){
			token=Parser::GetNextToken(in,line);
			if(RelExpr(in,line)){
				token=Parser::GetNextToken(in,line);
				if(token==RPAREN){
					token=Parser::GetNextToken(in,line);
					if(token==THEN){
						if(BlockIfStmt(in,line)){
							return true;
						}else{
							return false;
						}
					}else{
						Parser::PushBackToken(token);
						if(SimpleIfStmt(in,line)){
							return true;
						}else{
							return false;
						}
					}
				}else{
					ParseError(line,"Missing RPAREN IN STMT CHECKING");
				}
			}else{
				ParseError(line,"MISSING RELEXPR IN STMT CHECKING");
			}
		}else{
			ParseError(line,"MISSING LPAREN IN STMT CHECKING");
		}

	}else if(token==PRINT){
		if(PrintStmt(in,line)){
			return true;
		}else{
			return false;
		}
	}else{
		return false;
	}
	return false;
}
//AssignStmt::= Var= Expr
bool AssignStmt(istream& in, int& line){
	LexItem token; 
	//token=Parser::GetNextToken(in, line);
	if(Var(in,line)){
		token=Parser::GetNextToken(in, line);
		if(token==ASSOP){
			//token=Parser::GetNextToken(in, line);
			if(Expr(in,line)){
				return true;
			}else{
				ParseError(line,"Missing Expr in AssignStmt");
				return false;
			}
		}else{
			ParseError(line,"Missing ASSOP");
		}
	}else{
		ParseError(line,"Missing VAR");
	}
    return false;
}
//Factor::=IDENT | ICONST | RCONST | SCONST | (Expr)
bool Factor(istream& in, int& line, int sign){
	LexItem token;
	token=Parser::GetNextToken(in,line);
	if(token==IDENT || token==ICONST || token==RCONST || token==SCONST){
		return true;
	}else if(token==LPAREN){
		//token=Parser::GetNextToken(in,line);
		if(Expr(in,line)){
			token=Parser::GetNextToken(in,line);
			if(token==RPAREN){
				return true;
			}else{
				ParseError(line,"Missing RPAREN");
			}
		}else{
			ParseError(line,"Missing Expr");
		}
	}else{
		return false;
	}
	return false;
}
bool SFactor(istream& in, int& line){
	LexItem token;
	token=Parser::GetNextToken(in,line);
	if(token== PLUS || token== MINUS){
		if(Factor(in, line, 1)){
			return true;
		}else{
			 ParseError(line,"Missing Factor");
		}
	}else{
		Parser::PushBackToken(token);
		if(Factor(in, line, 1)){
			return true;
		}else{
			 ParseError(line,"Missing Factor");
			 return false;
		}
	}
	return false;
}
bool TermExpr(istream& in, int& line){
	LexItem token;
	if(SFactor(in,line)){
		token=Parser::GetNextToken(in,line);
		while(true){
			if(token==POW){
				if(!SFactor(in,line)){
					return false;
				}
				
			}else{
				Parser::PushBackToken(token);
				break;
			}
		}
			

	}else{
		ParseError(line,"SFACTOR BROKEN  IN TERMEXPR");
		return false;
	}
	return true;
}
bool MultExpr(istream& in, int& line){
	LexItem token;
	if(TermExpr(in,line)){
		token=Parser::GetNextToken(in,line);
		while(true){
			if(token==MULT || token==DIV){
				if(!TermExpr(in,line)){
					return false;
				}
			}else{
				Parser::PushBackToken(token);
				break;
			}
		}
			

	}else{
		ParseError(line,"TERMEXPR BROKEN  IN MULTEXPR");
		return false;
	}
	return true;
}
bool RelExpr(istream& in, int& line){
	LexItem token;
	if(Expr(in,line)){
		token=Parser::GetNextToken(in,line);
		if(token== EQ || token==LTHAN || token==GTHAN){
			if(Expr(in,line)){
				return true;
				}else{
					ParseError(line,"Missing Expr in RealExpr");
					}
			}else{
				Parser::PushBackToken(token);
			}
		}
	return false;
}
bool SimpleStmt(istream& in, int& line){
	LexItem token;
	//token=Parser::GetNextToken(in,line);
	if(AssignStmt(in,line) || PrintStmt(in,line)){
		return true;
	}else{
		ParseError(line,"No Simple Stmt");
	}
	return false;
}
bool SimpleIfStmt(istream& in, int& line){
	LexItem token;
	if(SimpleStmt(in,line)){
		return true;
	}else{
		return false;
	}
	return false;
}
bool BlockIfStmt(istream& in, int& line){
	LexItem token;
	while(true){
		token=Parser::GetNextToken(in,line);
		if(token==END || token==ELSE){
			Parser::PushBackToken(token);
			break;
		}else{
			Parser::PushBackToken(token);
			if(!Stmt(in,line)){
				return false;
				ParseError(line,"Missing STMT BLOCK IF STATEMENT");
			}
		}
	}
	token=Parser::GetNextToken(in,line);
	if(token==ELSE){
		while(true){
			token=Parser::GetNextToken(in,line);
			if(token==END){
				Parser::PushBackToken(token);
				break;
				}else{
					Parser::PushBackToken(token);
					if(!Stmt(in,line)){
						return false;
						ParseError(line,"Missing STMT BLOCK IF STATEMENT");
			}
		}
		}

	}else{
		Parser::PushBackToken(token);
	}
	token=Parser::GetNextToken(in,line);
	if(token==END){
		token=Parser::GetNextToken(in,line);
		if(token==IF){
			return true;
		}else{
			ParseError(line,"Missing IF in BlockIF");
		}
	}else{
		ParseError(line,"Missing END in BlockIF");
	}
	return false;
}