
#include "org/javia/arity/Lexer.h"
#include "org/javia/arity/VM.h"
#include <elastos/core/Character.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::Character;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Slogger;

namespace Org {
namespace Javia {
namespace Arity {

const Int32 Lexer::ADD;
const Int32 Lexer::SUB;
const Int32 Lexer::MUL;
const Int32 Lexer::DIV;
const Int32 Lexer::MOD;
const Int32 Lexer::UMIN;
const Int32 Lexer::POWER;
const Int32 Lexer::FACT;
const Int32 Lexer::NUMBER;
const Int32 Lexer::CONST;
const Int32 Lexer::CALL;
const Int32 Lexer::COMMA;
const Int32 Lexer::LPAREN;
const Int32 Lexer::RPAREN;
const Int32 Lexer::END;
const Int32 Lexer::SQRT;
const Int32 Lexer::PERCENT;

static AutoPtr<Token> InitToken(
    /* [in] */ Int32 id,
    /* [in] */ Int32 priority,
    /* [in] */ Int32 assoc,
    /* [in] */ Int32 vmop)
{
    return new Token(id, priority, assoc, vmop);
}
const AutoPtr<Token> Lexer::TOK_ADD    = InitToken(Lexer::ADD, 4, Token::LEFT, VM::ADD);
const AutoPtr<Token> Lexer::TOK_SUB    = InitToken(Lexer::SUB, 4, Token::LEFT, VM::SUB);
const AutoPtr<Token> Lexer::TOK_MUL    = InitToken(Lexer::MUL, 5, Token::LEFT, VM::MUL),
const AutoPtr<Token> Lexer::TOK_DIV    = InitToken(Lexer::DIV, 5, Token::LEFT, VM::DIV);
const AutoPtr<Token> Lexer::TOK_MOD    = InitToken(Lexer::MOD, 5, Token::LEFT, VM::MOD);
const AutoPtr<Token> Lexer::TOK_UMIN   = InitToken(Lexer::UMIN, 6, Token::PREFIX, VM::UMIN);
const AutoPtr<Token> Lexer::TOK_POWER  = InitToken(Lexer::POWER, 7, Token::RIGHT, VM::POWER);
const AutoPtr<Token> Lexer::TOK_FACT   = InitToken(Lexer::FACT,  8, Token::SUFIX, VM::FACT);
const AutoPtr<Token> Lexer::TOK_PERCENT = InitToken(Lexer::PERCENT, 9, Token::SUFIX, VM::PERCENT);
const AutoPtr<Token> Lexer::TOK_SQRT   = InitToken(Lexer::SQRT,  10, Token::PREFIX, VM::SQRT);
const AutoPtr<Token> Lexer::TOK_LPAREN = InitToken(Lexer::LPAREN, 1, Token::PREFIX, 0);
const AutoPtr<Token> Lexer::TOK_RPAREN = InitToken(Lexer::RPAREN, 3, 0, 0);
const AutoPtr<Token> Lexer::TOK_COMMA  = InitToken(Lexer::COMMA,  2, 0, 0);
const AutoPtr<Token> Lexer::TOK_END    = InitToken(Lexer::END,    0, 0, 0);
const AutoPtr<Token> Lexer::TOK_NUMBER = InitToken(Lexer::NUMBER, 20, 0, 0);
const AutoPtr<Token> Lexer::TOK_CONST  = InitToken(Lexer::CONST,  20, 0, 0);
const String Lexer::WHITESPACE(" \n\r\t");
const Char32 Lexer::END_MARKER;

Lexer::Lexer()
    : mPos(0)
{
    mInput = ArrayOf<Char32>::Alloc(32);
}

ECode Lexer::Scan(
    /* [in] */ const String& str,
    /* [in] */ TokenConsumer* consumer)
{
    if (str.IndexOf(END_MARKER) != -1) {
        Slogger::E("Lexer", "Invalid character '%c\'%d", END_MARKER, str.IndexOf(END_MARKER));
        return E_SYNTAX_EXCEPTION;
    }
    Init(str);
    consumer->Start();
    AutoPtr<Token> token;
    do {
        Int32 savePos = pos;
        FAIL_RETURN(NextToken((Token**)&token))
        token->mPosition = savePos;
        consumer->Push(token);
    } while (token != TOK_END);
    return NOERROR;
}

void Lexer::Init(
    /* [in] */ const String& str)
{
    Int32 len = str.GetLength();
    if (mInput->GetLength() < len + 1) {
        mInput = ArrayOf<Char32>::Alloc(len + 1);
    }
    mInput = str.GetChars(0, len);
    (*mInput)[len] = END_MARKER;
    mPos = 0;
}

ECode Lexer::NextToken(
    /* [out] */ Token** token)
{
    VALIDATE_NOT_NULL(token)
    while (WHITESPACE.IndexOf((*mInput)[pos]) != -1) {
        ++pos;
    }

    Char32 c = (*mInput)[pos];
    Int32 begin = pos++;

    switch (c) {
        case '!': {
            *token = TOK_FACT;
            REFCOUNT_ADD(*token)
            return NOERROR;
        }
        case END_MARKER: {
            *token = TOK_END;
            REFCOUNT_ADD(*token)
            return NOERROR;
        }
        case '%': {
            *token = TOK_PERCENT;
            REFCOUNT_ADD(*token)
            return NOERROR;
        }
        case '#': {
            *token = TOK_MOD;
            REFCOUNT_ADD(*token)
            return NOERROR;
        }
        case '(': {
            *token = TOK_LPAREN;
            REFCOUNT_ADD(*token)
            return NOERROR;
        }
        case ')': {
            *token = TOK_RPAREN;
            REFCOUNT_ADD(*token)
            return NOERROR;
        }
        case '*': {
            *token = TOK_MUL;
            REFCOUNT_ADD(*token)
            return NOERROR;
        }
        case '+': {
            *token = TOK_ADD;
            REFCOUNT_ADD(*token)
            return NOERROR;
        }
        case ',': {
            *token = TOK_COMMA;
            REFCOUNT_ADD(*token)
            return NOERROR;
        }
        case '-': {
            *token = TOK_SUB;
            REFCOUNT_ADD(*token)
            return NOERROR;
        }
        case '/': {
            *token = TOK_DIV;
            REFCOUNT_ADD(*token)
            return NOERROR;
        }
    }

    Int32 p = mPos;
    if (('0' <= c && c <= '9') || c == '.') {
        if (c == '0') {
            Char cc = Character::ToLowerCase((*mInput)[p]);
            Int32 base = (cc == 'x') ? 16 : (cc == 'b') ? 2 : (cc == 'o') ? 8 : 0;
            if (base > 0) {
                p++;
                do {
                    c = (*mInput)[p++];
                } while (('a' <= c && c <= 'z') ||
                         ('A' <= c && c <= 'Z') ||
                         ('0' <= c && c <= '9'));
                String coded(*mInput, begin + 2, p - 3 - begin);
                pos = p - 1;
                // try {
                TOK_NUMBER->SetValue(StringUtils::ParseInt32(coded, base));
                *token = TOK_NUMBER;
                REFCOUNT_ADD(*token)
                return NOERROR;
                // } catch (NumberFormatException e) {
                //     throw exception.set("invalid number '" + String.valueOf(input, begin, p-1-begin) + "'", begin);
                // }
            }
        }

        while (('0' <= c && c <= '9') || c == '.' || c == 'E' || c == 'e') {
            //accept '-' only after E
            if ((c == 'E' || c == 'e') && ((*mInput)[p] == '-' || (*mInput)[p] == UNICODE_MINUS)) {
                (*mInput)[p] = '-'; //replace unicode with plain minus, for Double.parseDouble()
                ++p;
            }
            c = (*mInput)[p++];
        }
        pos = p - 1;
        String nbStr(*mInput, begin, p - 1 - begin);
        // try {
        // parse single dot as 0
        if (nbStr.Equals(".")) {
            TOK_NUMBER->SetValue(0);
            *token = TOK_NUMBER;
            REFCOUNT_ADD(*token)
            return NOERROR;
        }
        else {
            Double numberValue = StringUtils::ParseDouble(nbStr);
            TOK_NUMBER->SetValue(numberValue);
            *token = TOK_NUMBER;
            REFCOUNT_ADD(*token)
            return NOERROR;
        }
        // } catch (NumberFormatException e) {
        //     throw exception.set("invalid number '" + nbStr + "'", begin);
        // }
    }
    else if (('a' <= c && c <= 'z') ||
               ('A' <= c && c <= 'Z')) {
        do {
            c = (*mInput)[p++];
        } while (('a' <= c && c <= 'z') ||
                 ('A' <= c && c <= 'Z') ||
                 ('0' <= c && c <= '9'));
        if (c == '\'') {
            c = (*mInput)[p++];
        }
        String nameValue(*mInput, begin, p - 1 - begin);
        while (WHITESPACE.IndexOf(c) != -1) {
            c = (*mInput)[p++];
        }
        if (c == '(') {
            pos = p;
            *token = new Token(CALL, 0, Token::PREFIX, 0);
            (*token)->SetAlpha(nameValue);
            REFCOUNT_ADD(*token)
            return NOERROR;
        }
        else {
            pos = p - 1;
            TOK_CONST->SetAlpha(nameValue);
            *token = TOK_CONST;
            REFCOUNT_ADD(*token)
            return NOERROR;
        }
    }
    else if ((c >= '\u0391' && c <= '\u03a9') || (c >= '\u03b1' && c <= '\u03c9')
               || c == '\u221e') {
        TOK_CONST->SetAlpha(String("") + c);
        *token = TOK_CONST;
        REFCOUNT_ADD(*token)
        return NOERROR;
    }
    else {
        switch (c) {
            case '^': {
                *token = TOK_POWER;
                REFCOUNT_ADD(*token)
                return NOERROR;
            }
            case UNICODE_MUL: {
                *token = TOK_MUL;
                REFCOUNT_ADD(*token)
                return NOERROR;
            }
            case UNICODE_DIV: {
                *token = TOK_DIV;
                REFCOUNT_ADD(*token)
                return NOERROR;
            }
            case UNICODE_MINUS: {
                *token = TOK_SUB;
                REFCOUNT_ADD(*token)
                return NOERROR;
            }
            case UNICODE_SQRT: {
                *token = TOK_SQRT;
                REFCOUNT_ADD(*token)
                return NOERROR;
            }
            default:
                Slogger::E("Lexer", "invalid character '%c'%d", c, begin);
                return E_SYNTAX_EXCEPTION;
        }
    }
}

} // namespace Arity
} // namespace Javia
} // namespace Org