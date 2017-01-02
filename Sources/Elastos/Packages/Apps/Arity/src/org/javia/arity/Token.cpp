//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
    return !mName.IsNull() && (len = mName.GetLength()) > 0 && mName.GetChar(len - 1) == '\'';
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
        case Lexer::_CONST:
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