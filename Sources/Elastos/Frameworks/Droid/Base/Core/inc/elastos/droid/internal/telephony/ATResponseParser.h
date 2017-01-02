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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_ATRESOPNSEPARSER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_ATRESOPNSEPARSER_H__

#include "_Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

/**
 * {@hide}
 */
class ATResponseParser
    : public Object
    , public IATResponseParser
{
/***************************** Class Methods *****************************/
public:
    CAR_INTERFACE_DECL()

    ATResponseParser(
        /* [in] */ const String& line);

    CARAPI NextBoolean(
        /* [out] */ Boolean* value);

    /** positive Int32 only */
    CARAPI NextInt32(
        /* [out] */ Int32* value);

    CARAPI NextString(
        /* [out] */ String* value);

    CARAPI HasMore(
        /* [out] */ Boolean* has);

private:
    CARAPI NextTok();

    /** Throws ATParseEx if whitespace extends to the end of string */
    CARAPI SkipWhiteSpace(
        /* [in] */ Char32 c,
        /* [out] */ Char32* value);

    CARAPI SkipPrefix();

private:
    /*************************** Instance Variables **************************/
    String mLine;
    Int32 mNext;
    Int32 mTokStart;
    Int32 mTokEnd;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_ATRESOPNSEPARSER_H__
