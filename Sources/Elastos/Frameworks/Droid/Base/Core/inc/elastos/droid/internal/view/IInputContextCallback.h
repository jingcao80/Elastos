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

#ifndef __ELASTOS_DROID_INTERNAL_VIEW_IINPUTCONTEXTCALLBACK_H__
#define __ELASTOS_DROID_INTERNAL_VIEW_IINPUTCONTEXTCALLBACK_H__

#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>
#include <binder/Binder.h>

using Elastos::Droid::View::InputMethod::IExtractedText;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {

class IInputContextCallbackProxy
    : public Object
    , public IIInputContextCallback
{
public:
    IInputContextCallbackProxy(
        /* [in] */ const android::sp<android::IBinder>& remote);

    CAR_INTERFACE_DECL();

    CARAPI SetTextBeforeCursor(
        /* [in] */ ICharSequence* textBeforeCursor,
        /* [in] */ Int32 seq);

    CARAPI SetTextAfterCursor(
        /* [in] */ ICharSequence* textAfterCursor,
        /* [in] */ Int32 seq);

    CARAPI SetCursorCapsMode(
        /* [in] */ Int32 capsMode,
        /* [in] */ Int32 seq);

    CARAPI SetExtractedText(
        /* [in] */ IExtractedText* extractedText,
        /* [in] */ Int32 seq);

    CARAPI SetSelectedText(
        /* [in] */ ICharSequence* selectedText,
        /* [in] */ Int32 seq);

    CARAPI SetRequestUpdateCursorAnchorInfoResult(
        /* [in] */ Boolean result,
        /* [in] */ Int32 seq);

private:
    static const String DESCRIPTOR;
    static const Int32 TRANSACTION_setTextBeforeCursor;
    static const Int32 TRANSACTION_setTextAfterCursor;
    static const Int32 TRANSACTION_setExtractedText;
    static const Int32 TRANSACTION_setSelectedText;
    static const Int32 TRANSACTION_setRequestUpdateCursorAnchorInfoResult;

    android::sp<android::IBinder> mRemote;
};

} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_VIEW_IINPUTCONTEXTCALLBACK_H__
