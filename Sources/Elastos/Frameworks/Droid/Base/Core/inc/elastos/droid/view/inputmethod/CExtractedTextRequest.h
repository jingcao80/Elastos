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

#ifndef __ELASTOS_DROID_VIEW_INPUTMETHOD_CEXTRACTEDTEXTREQUEST_H__
#define __ELASTOS_DROID_VIEW_INPUTMETHOD_CEXTRACTEDTEXTREQUEST_H__

#include "_Elastos_Droid_View_InputMethod_CExtractedTextRequest.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace View {
namespace InputMethod {

CarClass(CExtractedTextRequest)
    , public Object
    , public IExtractedTextRequest
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CExtractedTextRequest();

    CARAPI constructor();

    CARAPI GetToken(
        /* [out] */ Int32* token);

    CARAPI SetToken(
        /* [in] */ Int32 token);

    CARAPI GetFlags(
        /* [out] */ Int32* flags);

    CARAPI SetFlags(
        /* [in] */ Int32 flags);

    CARAPI GetHintMaxLines(
        /* [out] */ Int32* hintMaxLines);

    CARAPI SetHintMaxLines(
        /* [in] */ Int32 hintMaxLines);

    CARAPI GetHintMaxChars(
        /* [out] */ Int32* hintMaxChars);

    CARAPI SetHintMaxChars(
        /* [in] */ Int32 hintMaxChars);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel *source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel *dest);

public:
    /**
     * Arbitrary integer that can be supplied in the request, which will be
     * delivered back when reporting updates.
     */
    Int32 mToken;

    /**
     * Additional request flags, having the same possible values as the
     * flags parameter of {@link InputConnection#getTextBeforeCursor
     * InputConnection.getTextBeforeCursor()}.
     */
    Int32 mFlags;

    /**
     * Hint for the maximum number of lines to return.
     */
    Int32 mHintMaxLines;

    /**
     * Hint for the maximum number of characters to return.
     */
    Int32 mHintMaxChars;
};

} // namespace InputMethod
} // namespace View
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_INPUTMETHOD_CEXTRACTEDTEXTREQUEST_H__
