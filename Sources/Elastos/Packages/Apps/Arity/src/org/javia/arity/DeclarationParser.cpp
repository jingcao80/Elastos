
#include "org/javia/arity/DeclarationParser.h"
#include "org/javia/arity/Lexer.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Utility::CVector;
using Elastos::Utility::Logging::Slogger;

namespace Org {
namespace Javia {
namespace Arity {

static AutoPtr<ArrayOf<String> > InitArgs()
{
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(0);
    return args;
}
const AutoPtr<ArrayOf<String> > DeclarationParser::NO_ARGS = InitArgs();
const Int32 DeclarationParser::UNKNOWN_ARITY;
const Int32 DeclarationParser::MAX_ARITY;

DeclarationParser::DeclarationParser()
    : mArity(0)
{
    CVector::New((IVector**)&mArgs);
}

void DeclarationParser::Start()
{
    mName = String(NULL);
    mArity = UNKNOWN_ARITY;
    mArgs->SetSize(0);
}

ECode DeclarationParser::Push(
    /* [in] */ Token* token)
{
    switch (token->mId) {
        case Lexer::CALL:
            if (mName.IsNull()) {
                mName = token->mName;
                mArity = 0;
            }
            else {
                Slogger::E("DeclarationParser", "repeated CALL in declaration %d", token->mPosition);
                return E_SYNTAX_EXCEPTION;
            }
            break;

        case Lexer::_CONST:
            if (mName.IsNull()) {
                mName = token->mName;
                mArity = UNKNOWN_ARITY;
            }
            else if (mArity >= 0) {
                AutoPtr<ICharSequence> cs;
                CString::New(token->mName, (ICharSequence**)&cs);
                mArgs->AddElement(cs);
                ++mArity;
                if (mArity > MAX_ARITY) {
                    Slogger::E("DeclarationParser", "Arity too large  %d, %d", mArity, token->mPosition);
                    return E_SYNTAX_EXCEPTION;
                }
            }
            else {
                Slogger::E("DeclarationParser", "Invalid declaration %d", token->mPosition);
                return E_SYNTAX_EXCEPTION;
            }
            break;

        case Lexer::RPAREN:
        case Lexer::COMMA:
        case Lexer::END:
            break;

        default:
            Slogger::E("DeclarationParser", "Invalid token in declaration %d", token->mPosition);
            return E_SYNTAX_EXCEPTION;
    }
    return NOERROR;
}

AutoPtr<ArrayOf<String> > DeclarationParser::ArgNames()
{
    if (mArity > 0) {
        AutoPtr<ArrayOf<String> > argNames = ArrayOf<String>::Alloc(mArity);
        AutoPtr<ArrayOf<IInterface*> > temp = ArrayOf<IInterface*>::Alloc(mArity);
        mArgs->CopyInto(temp);
        for (Int32 i = 0; i < temp->GetLength(); ++i) {
            (ICharSequence::Probe((*temp)[i]))->ToString(&(*argNames)[i]);
        }
        return argNames;
    }
    else {
        return NO_ARGS;
    }
}

} // namespace Arity
} // namespace Javia
} // namespace Org