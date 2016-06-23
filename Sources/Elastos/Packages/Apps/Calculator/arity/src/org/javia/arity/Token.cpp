
#include "org/javia/arity/Token.h"
#include "org/javia/arity/Lexer.h"
#include "org/javia/arity/Symbol.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Org {
namespace Javia {
namespace Arity {

const Int32 Token::PREFIX;
const Int32 Token::LEFT;
const Int32 Token::RIGHT;
const Int32 Token::SUFIX;

Token::Token(
    /* [in] */ Int32 id,
    /* [in] */ Int32 priority,
    /* [in] */ Int32 assoc,
    /* [in] */ Int32 vmop)
    : mPriority(priority)
    , mAssoc(assoc)
    , mId(id)
    , mVmop(vmop)
    , mValue(0)
    , mArity(0)
    , mPosition(0)
{
    mArity = id == Lexer::CALL ? 1 : Symbol::CONST_ARITY;
}

void Token::SetPos(
    /* [in] */ Int32 pos)
{
    mPosition = pos;
}

void Token::SetValue(
    /* [in] */ Double value)
{
    mValue = value;
}

void Token::SetAlpha(
    /* [in] */ const String& alpha)
{
    mName = alpha;
}

Boolean Token::IsDerivative()
{
    Int32 len;
    return !mName.IsNull() && (len = name.GetLength()) > 0 && name.GetChar(len - 1) == '\'';
}

ECode Token::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    switch (mId) {
        case Lexer::NUMBER: {
            StringBuilder sb("");
            sb.Append(mValue);
            *str = sb.ToString();
            return NOERROR;
        }
        case Lexer::CALL: {
            StringBuilder sb(mName);
            sb.AppendChar('(');
            sb.Append(mArity);
            sb.AppendChar(')');
            *str = sb.ToString();
            return NOERROR;
        }
        case Lexer::CONST:
            *str = mName;
            return NOERROR;
    }
    StringBuilder sb("");
    sb.Append(mId);
    *str = sb.ToString();
    return NOERROR;
}

} // namespace Arity
} // namespace Javia
} // namespace Org