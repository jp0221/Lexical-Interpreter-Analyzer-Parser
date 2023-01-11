/* Definitions and some functions implementations
 * parse.cpp to be completed
 * Programming Assignment 2
 * Spring 2022
*/

#include "parse.h"


map<string, bool> defVar;
map<string, Token> SymTable;

namespace Parser {
bool pushed_back = false;
LexItem pushed_token;

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


//Stmt is either a WriteLnStmt, ForepeatStmt, IfStmt, or AssigStmt
//Stmt = AssigStmt | IfStmt | WriteStmt | ForStmt
bool Stmt(istream& in, int& line) {
    bool status;
    //cout << "in ContrlStmt" << endl;
    LexItem t = Parser::GetNextToken(in, line);

    switch( t.GetToken() ) {

        case WRITELN:
            status = WriteLnStmt(in, line);
            //cout << "After WriteStmet status: " << (status? true:false) <<endl;
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
}//End of Stmt


//WriteStmt:= wi, ExpreList
bool WriteLnStmt(istream& in, int& line) {
    LexItem t;
    //cout << "in WriteStmt" << endl;

    t = Parser::GetNextToken(in, line);
    if( t != LPAREN ) {

        ParseError(line, "Missing Left Parenthesis");
        return false;
    }

    bool ex = ExprList(in, line);

    if( !ex ) {
        ParseError(line, "Missing expression after WriteLn");
        return false;
    }

    t = Parser::GetNextToken(in, line);
    if(t != RPAREN ) {

        ParseError(line, "Missing Right Parenthesis");
        return false;
    }
    //Evaluate: print out the list of expressions values

    return ex;
}


//ExprList:= Expr {,Expr}
bool ExprList(istream& in, int& line) {
    bool status = false;
//cout << "in ExprList and before calling Expr" << endl;
status = Expr(in, line);
if(!status){
    ParseError(line, "Missing Expression");
        return false;
    }

    LexItem tok = Parser::GetNextToken(in, line);

    if (tok == COMMA) {
        //cout << "before calling ExprList" << endl;
        status = ExprList(in, line);
        //cout << "after calling ExprList" << endl;
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
}

bool Expr(istream& in, int& line) {
    while(true) {
        bool status = Term(in, line);
        if(!status){
            ParseError(line, "Missing operand after operator");
            return false;
        }
        
        LexItem t = Parser::GetNextToken(in, line);
        if(t.GetToken() == ERR) {
            return false;
        }
        if(t != PLUS && t != MINUS){
            Parser::PushBackToken(t);
            break;
        }
    }
    return true;
}

bool Term(istream&in, int& line){
    while(true) {
        bool status = SFactor(in, line);
        LexItem t;
        if(!status){
            return false;
        }
        
		t = Parser::GetNextToken(in, line);
        if(t.GetToken() == ERR) {
            return false;
        }
        if(t != MULT && t != DIV){
            Parser::PushBackToken(t);
            break;
        }
    }
    return true;
}

bool SFactor(istream&in, int& line){
    LexItem t = Parser::GetNextToken(in, line);
    int sign = 0;

    if(t == PLUS){
        sign = 1;
    } else if(t == MINUS){
        sign = -1;
    } else {
        Parser::PushBackToken(t);
    }
    
    bool status = Factor(in, line, sign);
    if (!status){
        return false;
    }

    return true;
}

bool Var(istream&in, int& line) {
    LexItem t = Parser::GetNextToken(in, line);
    if(t.GetToken() != IDENT) {
        ParseError(line, "Missing IDENT");
        return false;
    }
    if(!(defVar.find(t.GetLexeme()) -> second)) {
        ParseError(line, "Variable already defined");
        return false;
    }
    return true;
}

bool ForStmt(istream&in, int& line){
    bool status=Var(in,line);
    LexItem t;
    if (!status){
        ParseError(line,"Missing Var in For");
        return false;
    }
    t = Parser::GetNextToken(in,line);
    if (t!=ASSOP){
        ParseError(line,"No ASSOP after Var");
        return false;
    }

    t = Parser::GetNextToken(in,line);
    if (t != ICONST){
        ParseError(line,"NO Iconst in For");
        return false;
    }

    t = Parser::GetNextToken(in,line);
    if (t != TO && t != DOWNTO){
        ParseError(line,"Missing to or DownTo");
        return false;
    }

    t = Parser::GetNextToken(in,line);
    if (t != ICONST){
        ParseError(line,"Missing Iconst in For");
        return false;
    }

    t = Parser::GetNextToken(in,line);
    if (t != DO){
        ParseError(line,"Missing DO in For");
        return false;
    }

    status = Stmt(in,line);
    if (!status){
        ParseError(line,"Invalid Stmt in For");
        return false;
    }
    return true;
}

bool AssignStmt(istream&in, int& line){
    bool status = Var(in,line);
    if(!status){
        ParseError(line,"Missing Variable");
        return false;
    }
    LexItem tok=Parser::GetNextToken(in,line);
    if (tok!=ASSOP){
        ParseError(line,"Missing ASSOP");
        return false;
    }

    status= Expr(in,line);
    if (!status){
        ParseError(line,"Missing Expression");
        return false;
    }
    return true;
}

bool LogicExpr(istream&in, int& line){
    bool ex = Expr(in, line);
    if(!ex){
        ParseError(line, "Missing Expression");
        return false;
    }
    LexItem tok = Parser::GetNextToken(in, line);
    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized Input Patter");
        return false;
    }
    if(tok != EQUAL && tok!= LTHAN && tok != GTHAN){
        ParseError(line, "Missing operator for operand type");
        return false;
    }
    ex = Expr(in, line);
    if(!ex){
        ParseError(line, "Missing Expression");
        return false;
    }
    return true;
}

bool Factor(istream&in, int& line, int sign) {
	
    LexItem t = Parser::GetNextToken(in, line);
    if(t.GetToken() == IDENT){
        if(!(defVar.find(t.GetLexeme()) -> second)) {
            ParseError(line, "Undefined Variable");
            return false;
        }
        return true;
    } else if(t.GetToken() == ICONST) {
        return true;
    } else if(t.GetToken() == RCONST) {
        return true;
    } else if(t.GetToken() == SCONST) {
        return true;
    } else if (t.GetToken() == LPAREN) {
        bool ex = Expr(in, line);
        
        if(!ex) {
            ParseError(line, "Missing Expression");
            return false;
        }
        
        t = Parser::GetNextToken(in, line);
        if(t.GetToken() == RPAREN) {
            return ex;
        } else if(t.GetToken() == ERR) {
            ParseError(line, "Unrecognized input pattern");
            return false;
        } else {
            ParseError(line, "Unrecognized input pattern");
            return false;
        }
    }
    
    return false;
}

bool Prog(istream&in, int& line) {
	
    LexItem t = Parser::GetNextToken(in, line);
    if(t.GetToken() != PROGRAM) {
        ParseError(line, "Missing PROGRAM.");
        return false;
    }
    
    t = Parser::GetNextToken(in, line);
    if(t.GetToken() != IDENT) {
        ParseError(line, "Missing Program Name.");
        return false;
    }
    
    t = Parser::GetNextToken(in, line);
    if(t.GetToken() != SEMICOL) {
        ParseError(line, "Missing Semicol");
        return false;
    }

    bool status = DeclBlock(in, line);
    if(!status) {
        ParseError(line, "Incorrect Declaration Section.");
        return false;
    }

    status = ProgBody(in, line);
    if(!status) {
        ParseError(line, "Program Body is incorrect");
        return false;
    }
    return true;
}

bool ProgBody(istream&in, int& line){
    LexItem tok = Parser::GetNextToken(in, line);
    if(tok != BEGIN){
        ParseError(line, "Missing Keyword BEGIN");
        return false;
    }
    while (true) {
        tok = Parser::GetNextToken(in,line);
        if (tok == END) break;
        Parser::PushBackToken(tok);
        int old_line=line;

        bool status = Stmt(in,line);
        if (!status){
            ParseError(line,"Syntactic error in Program Body.");
            return false;
        }

        LexItem tok=Parser::GetNextToken(in,line);
        if (tok!=SEMICOL) {
            if (line!=old_line) line=old_line;
            ParseError(line,"Missing semicolon in Statement.");
            return false;
        }
    }
    return true;
}

bool DeclBlock(istream&in, int& line){
    LexItem t = Parser::GetNextToken(in, line);
    if(t.GetToken() != VAR) {
        ParseError(line, "Variable not in Declaration Block");
        return false;
    }

    bool status = DeclStmt(in, line);
    if(!status) {
        ParseError(line, "Syntactic error in Declaration Block.");
        return false;
    }
    while(true) {
        t = Parser::GetNextToken(in, line);
        if(t != SEMICOL){
            ParseError(line, "Missing ; at the end of a Declaration Statement");
            return false;
        }
        
        t = Parser::GetNextToken(in, line);
        Parser::PushBackToken(t);
        if (t == BEGIN){
            break;
        }
        
        status = DeclStmt(in, line);
        if (!status){
            ParseError(line, "Invalid Declaration Statement");
            return false;
        }
    }
    return true;
}

bool DeclStmt(istream&in, int& line){
    LexItem t = Parser::GetNextToken(in, line);
    if(t.GetToken() != IDENT){
        ParseError(line, "Missing IDENT in Declaration Statement");
        return false;
    }
    if((defVar.find(t.GetLexeme()) -> second) == true){
        ParseError(line,"Variable Redefinition");
        ParseError(line,"Incorrect variable in Declaration Statement.");
        return false;
    }
    defVar[t.GetLexeme()] = true;

    while(true){
        t = Parser::GetNextToken(in, line);
        if(t != COMMA) {
            Parser::PushBackToken(t);
            break;
        }
        t = Parser::GetNextToken(in, line);
        if (t != IDENT){
            ParseError(line, "Missing IDENT after comma in Declaration Statement");
            return false;
        }
        if((defVar.find(t.GetLexeme()) -> second) == true) {
            ParseError(line,"Variable Redefinition");
            ParseError(line,"Incorrect variable in Declaration Statement.");
            return false;
        }
        defVar[t.GetLexeme()] = true;
    }
    t = Parser::GetNextToken(in, line);
    if (t != COLON)
    {
        ParseError(line, "Unrecognized Input Pattern");
        cout<<"("<<t.GetLexeme()<<")"<<endl;
        ParseError(line,"Incorrect variable in Declaration Statement.");
        return false;
    }

    t = Parser::GetNextToken(in, line);
    if (t != INTEGER && t != REAL && t != STRING)
    {
        ParseError(line, "Invalid Data Type for Variables");
        return false;
    }
    return true;
}

bool IfStmt(istream&in, int& line){
    LexItem t = Parser::GetNextToken(in, line);
    if( t != LPAREN ) {
        ParseError(line, "Missing Left Parenthesis");
        return false;
    }

    bool status = LogicExpr(in, line);
    if(!status){
        ParseError(line, "Missing logic Expression");
        return false;
    }

    t = Parser::GetNextToken(in, line);
    if( t != RPAREN){
        ParseError(line, "Missing Right Parenthesis");
        return false;
    }

    t = Parser::GetNextToken(in, line);
    if( t != THEN){
        ParseError(line, "Missing THEN in IfStmt");
        return false;
    }

    status = Stmt(in, line);
    if(!status){
        ParseError(line, "Missing Statement for If");
        return false;
    }

    t = Parser::GetNextToken(in, line);
    if(t == ELSE ) {
        status=Stmt(in,line);
        if( !status ) {
            ParseError(line, "Invalid Statement in If's else");
            return false;
        }
    }else{
        Parser::PushBackToken(t);
    }

    return true;
}
