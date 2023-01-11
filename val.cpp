#include "val.h"

using namespace std;


Value Value:: operator+(const Value& op) const {
    if ( IsString() && op.IsString()) {
        string Stemp = GetString();
        Stemp += op.GetString();
        return Value(Stemp);
    } else if ( IsInt() && op.IsInt() ) {
        int Itemp = GetInt();
        Itemp += op.GetInt();
        return Value(Itemp);
    } else if ( IsInt() && op.IsReal() ) {
        float Ftemp = GetInt();
        Ftemp += op.GetReal();
        return Value(Ftemp);
    } else if ( IsReal() && op.IsInt() ) {
        float Ftemp = GetReal();
        Ftemp += op.GetInt();
        return Value(Ftemp);
    } else if ( IsReal() && op.IsReal() ) {
        float Ftemp = GetReal();
        Ftemp += op.GetReal();
        return Value(Ftemp);
    }
    return Value();
}

// subtract op from this
Value Value::operator-(const Value& op) const {
    if ( IsInt() && op.IsInt() ) {
        int Itemp = GetInt();
        Itemp -= op.GetInt();
        return Value(Itemp);
    } else if ( IsInt() && op.IsReal() ) {
        float Ftemp = GetInt();
        Ftemp -= op.GetReal();
        return Value(Ftemp);
    } else if ( IsReal() && op.IsInt() ) {
        float Ftemp = GetReal();
        Ftemp -= op.GetInt();
        return Value(Ftemp);
    } else if ( IsReal() && op.IsReal() ) {
        float Ftemp = GetReal();
        Ftemp -= op.GetReal();
        return Value(Ftemp);
    }
    return Value();
}

// multiply this by op
Value Value:: operator*(const Value& op) const {
    if ( IsInt() && op.IsInt() ) {
        int Itemp = GetInt();
        Itemp *= op.GetInt();
        return Value(Itemp);
    } else if ( IsInt() && op.IsReal() ) {
        float Ftemp = GetInt();
        Ftemp *= op.GetReal();
        return Value(Ftemp);
    } else if ( IsReal() && op.IsInt() ) {
        float Ftemp = GetReal();
        Ftemp *= op.GetInt();
        return Value(Ftemp);
    } else if ( IsReal() && op.IsReal() ) {
        float Ftemp = GetReal();
        Ftemp *= op.GetReal();
        return Value(Ftemp);
    }
    return Value();
}

// divide this by op
Value Value:: operator/(const Value& op) const {
    if ( IsInt() && op.IsInt() ) {
        int Itemp = GetInt();
        if ( op.GetInt() != 0.0) {
            Itemp /= op.GetInt();
            return Value(Itemp);
        }
    } else if ( IsInt() && op.IsReal() ) {
        float Ftemp = GetInt();
        if ( op.GetReal() != 0.0) {
            Ftemp /= op.GetReal();
            return Value(Ftemp);
        }
    } else if ( IsReal() && op.IsInt() ) {
        float Ftemp = GetReal();
        if ( op.GetInt() != 0.0) {
            Ftemp /= op.GetInt();
            return Value(Ftemp);
        }
    } else if ( IsReal() && op.IsReal() ) {
        float Ftemp = GetReal();
        if ( op.GetReal() != 0.0) {
            Ftemp /= op.GetReal();
            return Value(Ftemp);
        }
    }
    return Value();
}

Value Value:: operator==(const Value& op) const {
    if ( IsString() && op.IsString()) {
        string Stemp = GetString();
        string Sopp = op.GetString();
        return Value(Stemp == Sopp);
    }
    if ( IsString() || op.IsString() || IsErr() || op.IsErr()) {
        return Value();
    }
    float Ftemp, Sopp;
    if ( IsBool())
	{
		Ftemp = GetBool();
	} else if(IsInt()){
		Ftemp = GetInt();
	}else{
		Ftemp = GetReal();
	}

    if( op.IsBool()){
    	Sopp = op.GetBool();
	} else if(op.IsInt()){
		Sopp = op.GetInt();	
	} else{
		Sopp = op.GetReal();
	} 
    return Value(Ftemp == Sopp);
}

Value Value:: operator>(const Value& op) const {
    if ( IsString() && op.IsString()) {
        string Stemp = GetString();
        string Sopp = op.GetString();
        return Value(Stemp > Sopp);
    }
    if ( IsString() || op.IsString() || IsErr() || op.IsErr()) {
        return Value();
    }
    float Ftemp, Sopp;
    if ( IsBool())
	{
		Ftemp = GetBool();
	} else if(IsInt()){
		Ftemp = GetInt();
	}else{
		Ftemp = GetReal();
	}

    if( op.IsBool()){
    	Sopp = op.GetBool();
	} else if(op.IsInt()){
		Sopp = op.GetInt();	
	} else{
		Sopp = op.GetReal();
	} 
    return Value(Ftemp > Sopp);
}

Value Value:: operator<(const Value& op) const {
    if ( IsString() && op.IsString()) {
        string Stemp = GetString();
        string Sopp = op.GetString();
        return Value(Stemp < Sopp);
    }
    if ( IsString() || op.IsString() || IsErr() || op.IsErr()) {
        return Value();
    }
    float Ftemp, Sopp;
    if ( IsBool())
	{
		Ftemp = GetBool();
	} else if(IsInt()){
		Ftemp = GetInt();
	}else{
		Ftemp = GetReal();
	}

    if( op.IsBool()){
    	Sopp = op.GetBool();
	} else if(op.IsInt()){
		Sopp = op.GetInt();	
	} else{
		Sopp = op.GetReal();
	} 
    return Value(Ftemp < Sopp);
}
