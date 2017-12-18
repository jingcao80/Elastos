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

#ifndef __ELASTOS_DROID_INTERNAL_VIEW_IINPUTMETHODSESSION_H__
#define __ELASTOS_DROID_INTERNAL_VIEW_IINPUTMETHODSESSION_H__

#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>
#include <binder/Binder.h>

using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::InputMethod::ICompletionInfo;
using Elastos::Droid::View::InputMethod::ICursorAnchorInfo;
using Elastos::Droid::View::InputMethod::IExtractedText;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {

class IInputMethodSessionProxy
    : public Object
    , public IIInputMethodSession
{
public:
    IInputMethodSessionProxy(
        /* [in] */ const android::sp<android::IBinder>& remote);

    CAR_INTERFACE_DECL();

    CARAPI FinishInput();

    CARAPI UpdateExtractedText(
        /* [in] */ Int32 token,
        /* [in] */ IExtractedText* text);

    CARAPI UpdateSelection(
        /* [in] */ Int32 oldSelStart,
        /* [in] */ Int32 oldSelEnd,
        /* [in] */ Int32 newSelStart,
        /* [in] */ Int32 newSelEnd,
        /* [in] */ Int32 candidatesStart,
        /* [in] */ Int32 candidatesEnd);

    CARAPI ViewClicked(
        /* [in] */ Boolean focusChanged);

    CARAPI UpdateCursor(
        /* [in] */ IRect* newCursor);

    CARAPI DisplayCompletions(
        /* [in] */ ArrayOf<ICompletionInfo*>* completions);

    CARAPI AppPrivateCommand(
        /* [in] */ const String& action,
        /* [in] */ IBundle* data);

    CARAPI ToggleSoftInput(
        /* [in] */ Int32 showFlags,
        /* [in] */ Int32 hideFlags);

    CARAPI FinishSession();

    CARAPI UpdateCursorAnchorInfo(
        /* [in] */ ICursorAnchorInfo* cursorAnchorInfo);

private:
    static const String DESCRIPTOR;
    static const Int32 TRANSACTION_updateSelection;
    static const Int32 TRANSACTION_viewClicked;
    static const Int32 TRANSACTION_updateCursorAnchorInfo;

    android::sp<android::IBinder> mRemote;
};

} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_VIEW_IINPUTMETHODSESSION_H__
