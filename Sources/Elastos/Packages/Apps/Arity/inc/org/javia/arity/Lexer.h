
#ifndef _ORG_JAVIA_ARITY_LEXER_H__
#define _ORG_JAVIA_ARITY_LEXER_H__

#include "_Org.Javia.Arity.h"
#include <elastos/droid/ext/frameworkext.h>
#include "org/javia/arity/TokenConsumer.h"

namespace Org {
namespace Javia {
namespace Arity {

class Lexer : public Object
{
public:
    Lexer();

    CARAPI Scan(
        /* [in] */ const String& str,
        /* [in] */ TokenConsumer* consumer);

    CARAPI NextToken(
        /* [out] */ Token** token);

private:
    CARAPI_(void) Init(
        /* [in] */ const String& str);

public:
    static const Int32 ADD = 1;
    static const Int32 SUB = 2;
    static const Int32 MUL = 3;
    static const Int32 DIV = 4;
    static const Int32 MOD = 5;
    static const Int32 UMIN   = 6;
    static const Int32 POWER  = 7;
    static const Int32 FACT   = 8;
    static const Int32 NUMBER = 9;
    static const Int32 _CONST  = 10;
    static const Int32 CALL   = 11;
    static const Int32 COMMA  = 12;
    static const Int32 LPAREN = 13;
    static const Int32 RPAREN = 14;
    static const Int32 END    = 15;
    static const Int32 SQRT   = 16;
    static const Int32 PERCENT = 17;

    static const AutoPtr<Token> TOK_ADD;
    static const AutoPtr<Token> TOK_SUB;

    static const AutoPtr<Token> TOK_MUL;
    static const AutoPtr<Token> TOK_DIV;
    static const AutoPtr<Token> TOK_MOD;

    static const AutoPtr<Token> TOK_UMIN;

    static const AutoPtr<Token> TOK_POWER;

    static const AutoPtr<Token> TOK_FACT;
    static const AutoPtr<Token> TOK_PERCENT;

    static const AutoPtr<Token> TOK_SQRT;

    static const AutoPtr<Token> TOK_LPAREN;
    static const AutoPtr<Token> TOK_RPAREN;
    static const AutoPtr<Token> TOK_COMMA;
    static const AutoPtr<Token> TOK_END;

    static const AutoPtr<Token> TOK_NUMBER;
    static const AutoPtr<Token> TOK_CONST;

private:
    static const Char32 UNICODE_MINUS = 0x2212;
    static const Char32 UNICODE_MUL   = 0x00d7;
    static const Char32 UNICODE_DIV   = 0x00f7;
    static const Char32 UNICODE_SQRT  = 0x221a;

    static const String WHITESPACE;
    static const Char32 END_MARKER = '$';
    AutoPtr<ArrayOf<Char32> > mInput;
    Int32 mPos;
};

} // namespace Arity
} // namespace Javia
} // namespace Org

#endif // _ORG_JAVIA_ARITY_LEXER_H__