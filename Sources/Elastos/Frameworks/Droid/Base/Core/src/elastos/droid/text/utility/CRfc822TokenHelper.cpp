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

#include "elastos/droid/text/utility/CRfc822TokenHelper.h"
#include "elastos/droid/text/utility/Rfc822Token.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Utility {

CAR_INTERFACE_IMPL(CRfc822TokenHelper, Singleton, IRfc822TokenHelper)

CAR_SINGLETON_IMPL(CRfc822TokenHelper)

ECode CRfc822TokenHelper::QuoteNameIfNecessary(
    /* [in] */ const String& name,
    /* [out] */ String* ret)
{
    return Rfc822Token::QuoteNameIfNecessary(name, ret);
}

ECode CRfc822TokenHelper::QuoteName(
    /* [in] */ const String& name,
    /* [out] */ String* ret)
{
    return Rfc822Token::QuoteName(name, ret);
}

ECode CRfc822TokenHelper::QuoteComment(
    /* [in] */ const String& comment,
    /* [out] */ String* ret)
{
    return Rfc822Token::QuoteComment(comment, ret);
}

} //namespace Utility
} //namespace Text
} //namespace Droid
} //namespace Elastos

