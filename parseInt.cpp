/* Implementation of Recursive-Descent Parser
 * parse.cpp
 * Programming Assignment 2
 * Spring 2022
*/

#include "parseInt.h"
#include <queue>

map<string, bool> defVar;
map<string, Token> SymTable;
map<string, Value> TempsResults;
queue<Value> *ValQue;
bool univ;


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



bool Prog(istream& in, int& line)
{
	univ = true;
	LexItem t = Parser::GetNextToken(in, line);
	bool status;
	bool check;
		
	if (t.GetToken() == PROGRAM) {
		t = Parser::GetNextToken(in, line);
		if (t.GetToken() == IDENT) {
			
			t = Parser::GetNextToken(in, line);
			if (t.GetToken() == SEMICOL) {
				status = DeclBlock(in, line); 
			
				if(status) {
					check = ProgBody(in, line);
					if(!check)
					{
						ParseError(line, "Incorrect Program Body.");
						return false;
					}
					
					return true;
				}
				else
				{
					ParseError(line, "Incorrect Declaration Section.");
					return false;
				}
			}
			else
			{
				ParseError(line-1, "Missing Semicolon.");
				return false;
			}
		}
		else
		{
			ParseError(line, "Missing Program Name.");
			return false;
		}
	}
	else if(t.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << t.GetLexeme() << ")" << endl;
		return false;
	}
	else if(t.GetToken() == DONE && t.GetLinenum() <= 1){
		ParseError(line, "Empty File");
		return true;
	}
	ParseError(line, "Missing PROGRAM.");
	return false;
}

bool ProgBody(istream& in, int& line){
	bool check;
	LexItem t = Parser::GetNextToken(in, line);
	
	if (t.GetToken() == BEGIN) {
		check = Stmt(in, line);
		while(check)
		{
			t = Parser::GetNextToken(in, line);
			if(t != SEMICOL)
			{
				line--;
				ParseError(line, "Missing semicolon in Statement.");
				return false;
			}
			check = Stmt(in, line);
		}
		int status = line;			
		t = Parser::GetNextToken(in, line);
		if(t == END )
		{
			return true;
		}
		else 
		{
			line = status;
			ParseError(line, "Syntactic error in Program Body.");
			return false;
		}
	}
	else
	{
		ParseError(line, "Non-recognizable Program Body.");
		return false;
	}	
}


bool DeclBlock(istream& in, int& line) {
	bool check = false;
	LexItem tok = Parser::GetNextToken(in, line);
	if(tok == VAR)
	{
		check = DeclStmt(in, line);
		
		while(check)
		{
			tok = Parser::GetNextToken(in, line);
			if(tok != SEMICOL)
			{
				line--;
				ParseError(line, "Missing semicolon in Declaration Statement.");
				return false;
			}
			check = DeclStmt(in, line);
		}
		
		tok = Parser::GetNextToken(in, line);
		if(tok == BEGIN )
		{
			Parser::PushBackToken(tok);
			return true;
		}
		else 
		{
			ParseError(line, "Syntactic error in Declaration Block.");
			return false;
		}
	}
	else
	{
		ParseError(line, "Non-recognizable Declaration Block.");
		return false;
	}
	
}

bool DeclStmt(istream& in, int& line)
{
	bool check = IdentList(in, line);
	
	if (!check)
	{
		ParseError(line, "Incorrect variable in Declaration Statement.");
		return check;
	}
	
	LexItem tok = Parser::GetNextToken(in, line);
	if(tok == COLON)
	{
		tok = Parser::GetNextToken(in, line);
		if(tok == STRING || tok == REAL || tok == INTEGER)
		{
			for ( auto it = defVar.begin(); it != defVar.end(); it++) {
				if ( it->second && SymTable.find(it->first) == SymTable.end()) {
					SymTable[it->first] = tok.GetToken();
				}
			}
			return true;
		}
		else
		{
			ParseError(line, "Incorrect Declaration Type.");
			return false;
		}
	}
	else
	{
		Parser::PushBackToken(tok);
		return false;
	}
	
}
bool ForStmt(istream& in, int& line)
{
	LexItem tok;
	bool status = Var(in, line, tok);
	
	if(!status) {
		ParseError(line, "Missing For Statement Control Variable");
		return false;
	}
	
	tok = Parser::GetNextToken(in, line);
	if(tok != ASSOP) {
		ParseError(line, "For Statement Syntax Error");
		return false;
	}
	
	tok = Parser::GetNextToken(in, line);
	if(tok != ICONST) {
		ParseError(line, "Missing Initialization Value in For Statement.");
		return false;
	}
	
	tok = Parser::GetNextToken(in, line);
	if(tok == DOWNTO || tok == TO) {
		tok = Parser::GetNextToken(in, line);
		if(tok != ICONST) {
			ParseError(line, "Missing Termination Value in For Statement.");
			return false;
		}
		tok = Parser::GetNextToken(in, line);
		if(tok != DO) {
			ParseError(line, "For Statement Syntax Error");
			return false;
		}
		
		status = Stmt(in, line);
	} else {
		ParseError(line, "For Statement Syntax Error");
		return false;
	}
	
	return status;
}
bool IdentList(istream& in, int& line) {
	bool check = false;
	string str;
	
	LexItem tok = Parser::GetNextToken(in, line);
	if(tok == IDENT) {
		str = tok.GetLexeme();
		if (!(defVar.find(str)->second)) {
			defVar[str] = true;
		} else {
			ParseError(line, "Variable Redefinition");
			return false;
		}
		
	} else {
		Parser::PushBackToken(tok);
		return true;
	}
	
	tok = Parser::GetNextToken(in, line);
	
	if (tok == COMMA) {
		check = IdentList(in, line);
	} else if(tok == COLON) {
		Parser::PushBackToken(tok);
		return true;
	} else {
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	return check;
	
}

bool Var(istream& in, int& line, LexItem & idtok) {
	string str;
	
	LexItem tok = Parser::GetNextToken(in, line);
	idtok = tok;
	
	if (tok == IDENT){
		str = tok.GetLexeme();
		
		if (!(defVar.find(str)->second))
		{
			ParseError(line, "Undeclared Variable");
			return false;
		}	
		return true;
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	return false;
}
bool Stmt(istream& in, int& line) {
	bool status;
	
	LexItem t = Parser::GetNextToken(in, line);
	
	switch( t.GetToken() ) {

	case WRITELN:
		status = WriteLnStmt(in, line);
		
		break;

	case IF:
		status = IfStmt(in, line);
		break;

	case IDENT:
		Parser::PushBackToken(t);
        status = AssignStmt(in, line);
		
		break;
		
	case FOR:
		status = ForStmt(in, line);
		
		break;
		
		
	default:
		Parser::PushBackToken(t);
		return false;
	}

	return status;
}
bool WriteLnStmt(istream& in, int& line) {
	LexItem tok = Parser::GetNextToken(in, line);
	
	if( tok != LPAREN ) {
		
		ParseError(line, "Missing Left Parenthesis");
		return false;
	}
	ValQue = new queue<Value>();
	bool status = ExprList(in, line);
	
	if( !status ) {
		ParseError(line, "Missing expression after WriteLn");
		return false;
	}
	
	tok = Parser::GetNextToken(in, line);
	if(tok != RPAREN ) {
		ParseError(line, "Missing Right Parenthesis");
		return false;
	} 
	while (!(*ValQue).empty()) {
		cout << (*ValQue).front();
		ValQue->pop();
	}
	cout<<endl;
	
	return status;
}
bool IfStmt(istream& in, int& line) {
	Value temp;
	bool st; 
	LexItem tok = Parser::GetNextToken(in, line);
	if( tok != LPAREN ) {
		
		ParseError(line, "Missing Left Parenthesis");
		return false;
	}
	
	st = LogicExpr(in, line, temp);
	if( !st ) {
		ParseError(line, "Missing if statement Logic Expression");
		return false;
	}
	
	tok = Parser::GetNextToken(in, line);
	if(tok != RPAREN ) {
		
		ParseError(line, "Missing Right Parenthesis");
		return false;
	}
	
	tok = Parser::GetNextToken(in, line);
	if(tok != THEN) {
		ParseError(line, "If-Stmt Syntax Error");
		return false;
	}
	univ = temp.GetBool();
	st = Stmt(in, line);
	if(!st) {
		ParseError(line, "Missing Statement for If-Stmt Then-Part");
		return false;
	}
	tok = Parser::GetNextToken(in, line);
	univ = !temp.GetBool();
	if( tok == ELSE ) {
		st = Stmt(in, line);
		if(!st)
		{
			ParseError(line, "Missing Statement for If-Stmt Else-Part");
			return false;
		}
		univ = true;
		return true;
	}
	univ = true;
	Parser::PushBackToken(tok);
	return true;
}
bool AssignStmt(istream& in, int& line) {
	LexItem tok;
	string tokstr;
	Value temp;
	bool vstat = false;
	bool status = false;
	LexItem t;
	
	vstat = Var( in, line, tok);
	
	if (vstat){
		t = Parser::GetNextToken(in, line);
		
		if (t == ASSOP){
			status = Expr(in, line, temp);
			if(!status) {
				ParseError(line, "Missing Expression in Assignment Statment");
				return status;
			}
			if ( temp.IsErr()) {
				ParseError(line, "Illegal Assignment Operation");
				return false;
			}
			tokstr = tok.GetLexeme();
			if (univ) TempsResults[tokstr] = temp;

			if ( SymTable[tokstr] == STRING) {
				if ( !temp.IsString()) {
					ParseError(line, "Illegal Assignment Operation");
					return false;
				}
			} else {
				if ( temp.IsString() || temp.IsBool()) {
					ParseError(line, "Illegal Assignment Operation");
					return false;
				}
				if ( SymTable[tokstr] == INTEGER) {
					if ( temp.IsReal()) {
						if (univ) {
							TempsResults[tokstr].SetType(VINT);
							TempsResults[tokstr].SetInt ( (int)temp.GetReal());
						}
					}
				} else {
					if ( temp.IsInt()) {
						if ( univ ) {
							TempsResults[tokstr].SetType(VREAL);
							TempsResults[tokstr].SetReal ( temp.GetInt());
						}
					}
				}
			}

		} else if(t.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << t.GetLexeme() << ")" << endl;
			return false;
		} else {
			ParseError(line, "Missing Assignment Operator");
			return false;
		}
	} else {
		ParseError(line, "Missing Left-Hand Side Variable in Assignment statement");
		return false;
	}
	return status;	
}
bool LogicExpr(istream& in, int& line, Value & retVal)
{
	Value temp1, temp2;
	bool stat = Expr(in, line, temp1);
	
	if( !stat ) {
		return false;
	}
	
	LexItem tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	if ( tok == EQUAL  || tok == GTHAN  || tok == LTHAN)
	{
		stat = Expr(in, line, temp2);
		if( !stat ) 
		{
			ParseError(line, "Missing expression after relational operator");
			return false;
		}
		if ( tok == GTHAN ){
			retVal = (temp1 > temp2);	
		} else if ( tok == EQUAL ) {
			retVal = (temp1 == temp2);	
		} else{
			 retVal = ( temp1 < temp2);	
		}
		

		if ( !retVal.IsBool()) {
			ParseError(line, "Run-Time Error-Illegal Mixed Type Operands for a Logic Expression");
			return false;
		}
		return true;
	}
	retVal = temp1;
	if ( !retVal.IsBool()) {
		ParseError(line, "Run-Time Error-Illegal Mixed Type Operands for a Logic Expression");
		return false;
	}
	Parser::PushBackToken(tok);
	return true;
}


bool ExprList(istream& in, int& line) {
	Value temp;
	bool status = Expr(in, line, temp);
	if(!status){
		ParseError(line, "Missing Expression");
		return false;
	}
	ValQue->push(temp);
	
	LexItem tok = Parser::GetNextToken(in, line);
	
	if (tok == COMMA) {
		status = ExprList(in, line);
	} else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	} else{
		Parser::PushBackToken(tok);
		return true;
	}
	return status;
}

bool Expr(istream& in, int& line, Value & retVal) {
	Value temp;
	bool stat = Term(in, line, temp);
	
	if( !stat ) {
		return false;
	}
	retVal = temp;
	LexItem tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while ( tok == PLUS || tok == MINUS ) 
	{
		stat = Term(in, line, temp);
		if( !stat ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}
		if ( tok == PLUS ) {
			retVal = retVal + temp;
		}
		else {
			retVal = retVal - temp;
		}

		tok = Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}
		if ( retVal.IsErr()) {
			ParseError ( line, "Illegal addition or subtraction operation.");
			return false;
		}
	}
	Parser::PushBackToken(tok);
	return true;
}

bool Term(istream& in, int& line, Value &retVal) {
	Value temp;
	bool stat = SFactor(in, line, temp);
	
	if( !stat ) {
		return false;
	}
	retVal = temp;

	LexItem t	= Parser::GetNextToken(in, line);
	if(t.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << t.GetLexeme() << ")" << endl;
			return false;
	}
	while ( t == MULT || t == DIV  )
	{
		stat = SFactor(in, line, temp);
		
		if( !stat ) {
			ParseError(line, "Missing operand after operator");
			return false;
		}
		if ( t == MULT) {
			retVal = retVal*temp;
		} else if ( t == DIV) {
			if ( temp.IsReal()) {
				if ( temp.GetReal() == 0.0 ) {
					ParseError ( line, "Run-Time Error-Illegal Division By Zero");
					return false;
				}
			} else if ( temp.IsInt()) {
				if ( temp.GetInt() == 0 ) {
					ParseError ( line, "Run-Time Error-Illegal Division By Zero");
					return false;
				}
			}
			retVal = retVal/temp;
		}
		if ( retVal.IsErr()) {
			ParseError ( line, "Run-Time Error-Illegal Mixed Type Operands");
			return false;
		}


		t	= Parser::GetNextToken(in, line);
		if(t.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << t.GetLexeme() << ")" << endl;
			return false;
		}
	}
	Parser::PushBackToken(t);
	return true;
}

bool Factor(istream& in, int& line, int sign, Value & retVal) {
	Value temp;
	LexItem t = Parser::GetNextToken(in, line);
	
	if( t == IDENT ) {
		string lexeme = t.GetLexeme();
		if (!(defVar.find(lexeme)->second))
		{
			ParseError(line, "Using Undefined Variable");
			return false;	
		}
		if ( TempsResults.find(t.GetLexeme()) == TempsResults.end()) {
			ParseError(line, "Using Undeclared Variable");
			return false;
		}
		retVal = TempsResults[t.GetLexeme()];
		return true;
	}
	else if( t == ICONST ) {
		retVal.SetType(VINT);
		retVal.SetInt(stoi(t.GetLexeme()));
		return true;
	}
	else if( t == SCONST ) {
		retVal.SetType(VSTRING);
		retVal.SetString(t.GetLexeme());
		return true;
	}
	else if( t == RCONST ) {
		retVal.SetType(VREAL);
		retVal.SetReal ( stof(t.GetLexeme()));
		return true;
	}
	else if( t == LPAREN ) {
		bool stat = Expr(in, line, temp);
		if( !stat ) {
			ParseError(line, "Missing expression after (");
			return false;
		}
		if( Parser::GetNextToken(in, line) == RPAREN ) {
			retVal = temp;
			return stat;
		} else {
			Parser::PushBackToken(t);
			ParseError(line, "Missing ) after expression");
			return false;
		}
	}
	else if(t.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << t.GetLexeme() << ")" << endl;
		return false;
	}

	return false;
}

bool SFactor(istream& in, int& line, Value &retVal)
{
	Value temp;
	LexItem tok = Parser::GetNextToken(in, line);
	int status = 0;
	if(tok == MINUS )
	{
		status = -1;
	} else if(tok == PLUS) {
		status = 1;
	}
	else{
		Parser::PushBackToken(tok);	
	}

		
	bool check = Factor(in, line, status, temp);
	if ( check) {
		retVal = temp;
		if ( status != 0 ) {
			if ( temp.IsString() || temp.IsBool()) {
				ParseError ( line, "Illegal Operand Type for Sign Operator");
				return false;
			}
			if ( temp.IsInt()) {
				retVal.SetInt ( status*temp.GetInt());
			}
			else if ( temp.IsReal()) {
				retVal.SetReal ( status*temp.GetReal());
			}
		}
	}
	return check;
}

