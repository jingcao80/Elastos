
#include "org/javia/arity/RPN.h"
#include "org/javia/arity/Lexer.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::CStack;
using Elastos::Utility::IVector;
using Elastos::Utility::Logging::Slogger;

namespace Org {
namespace Javia {
namespace Arity {

RPN::RPN()
    : mPrevTokenId(0)
{
    CStack::New((IStack**)&mStack);
}

void RPN::SetConsumer(
    /* [in] */ TokenConsumer* consumer)
{
    mConsumer = consumer;
}

void RPN::Start()
{
    IVector::Probe(mStack)->RemoveAllElements();
    mPrevTokenId = 0;
    mConsumer->Start();
}

AutoPtr<Token> RPN::Top()
{
    Boolean empty;
    if (mStack->IsEmpty(&empty), empty) {
        return NULL;
    }
    else {
        AutoPtr<IInterface> out;
        mStack->Peek((IInterface**)&out);
        return (Token*)IObject::Probe(out);
    }
}

ECode RPN::PopHigher(
    /* [in] */ Int32 priority)
{
    AutoPtr<Token> t = Top();
    while (t != NULL && t->mPriority >= priority) {
        mConsumer->Push(t);
        //code.push(t);
        AutoPtr<IInterface> value;
        FAIL_RETURN(mStack->Pop((IInterface**)&value))
        t = Top();
    }
    return NOERROR;
}

Boolean RPN::IsOperand(
    /* [in] */ Int32 id)
{
    return
        id == Lexer::FACT ||
        id == Lexer::RPAREN ||
        id == Lexer::NUMBER ||
        id == Lexer::_CONST ||
        id == Lexer::PERCENT;
}

ECode RPN::Push(
    /* [in] */ Token* token)
{
    Int32 priority = token->mPriority;
    Int32 id = token->mId;
    switch (id) {
        case Lexer::NUMBER:
        case Lexer::_CONST:
            if (IsOperand(mPrevTokenId)) {
                Push(Lexer::TOK_MUL);
            }
            mConsumer->Push(token);
            break;

        case Lexer::RPAREN: {
            if (mPrevTokenId == Lexer::CALL) {
                Top()->mArity--;
            }
            else if (!IsOperand(mPrevTokenId)) {
                Slogger::E("RPN", "unexpected ) or END %d", token->mPosition);
                return E_SYNTAX_EXCEPTION;
            }

            PopHigher(priority);
            AutoPtr<Token> t = Top();
            if (t != NULL) {
                if (t->mId == Lexer::CALL) {
                    mConsumer->Push(t);
                }
                else if (t != Lexer::TOK_LPAREN) {
                    Slogger::E("RPN", "expected LPAREN or CALL %d", token->mPosition);
                    return E_SYNTAX_EXCEPTION;
                }
                AutoPtr<IInterface> value;
                mStack->Pop((IInterface**)&value);
            }
            break;
        }

        case Lexer::COMMA: {
            if (!IsOperand(mPrevTokenId)) {
                Slogger::E("RPN", "misplaced COMMA %d", token->mPosition);
                return E_SYNTAX_EXCEPTION;
            }
            PopHigher(priority);
            AutoPtr<Token> t = Top();
            if (t == NULL || t->mId != Lexer::CALL) {
                Slogger::E("RPN", "COMMA not inside CALL %d", token->mPosition);
                return E_SYNTAX_EXCEPTION;
            }
            t->mArity++;
            //code.push(stack.pop());
            break;
        }

        case Lexer::END: {
            AutoPtr<Token> t = Lexer::TOK_RPAREN;
            t->mPosition = token->mPosition;
            do {
                Push(t);
            } while (Top() != NULL);
            break;
        }

        default: //operators, CALL, LPAREN
            if (token->mAssoc == Token::PREFIX) {
                if (IsOperand(mPrevTokenId)) {
                    Push(Lexer::TOK_MUL);
                }
                mStack->Push((IObject*)token);
                break;
            }
            if (!IsOperand(mPrevTokenId)) {
                if (id == Lexer::SUB) {
                    //change SUB to unary minus
                    token = Lexer::TOK_UMIN;
                    mStack->Push((IObject*)token);
                    break;
                }
                else if (id == Lexer::ADD) {
                    // ignore, keep prevTokenId unchanged
                    return NOERROR;
                }
                Slogger::E("RPN", "operator without operand %d", token->mPosition);
                return E_SYNTAX_EXCEPTION;
            }
            PopHigher(priority + (token->mAssoc == Token::RIGHT ? 1 : 0));
            mStack->Push((IObject*)token);
    }
    mPrevTokenId = token->mId;
    return NOERROR;
}

} // namespace Arity
} // namespace Javia
} // namespace Org