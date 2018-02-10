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

#ifndef __ELASTOS_DROID_JAVAPROXY_CINPUTCONTEXTNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CINPUTCONTEXTNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CInputContextNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Core::ICharSequence;

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::InputMethod::ICompletionInfo;
using Elastos::Droid::View::InputMethod::ICorrectionInfo;
using Elastos::Droid::View::InputMethod::IExtractedTextRequest;
using Elastos::Droid::Internal::View::IIInputContext;
using Elastos::Droid::Internal::View::IIInputContextCallback;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CInputContextNative)
    , public Object
    , public IIInputContext
    , public IBinder
{
public:
    ~CInputContextNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ HANDLE jVM,
        /* [in] */ HANDLE jInstance);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI GetTextBeforeCursor(
        /* [in] */ Int32 length,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 seq,
        /* [in] */ IIInputContextCallback* icCallback);

    CARAPI GetTextAfterCursor(
        /* [in] */ Int32 length,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 seq,
        /* [in] */ IIInputContextCallback* icCallback);

    CARAPI GetCursorCapsMode(
        /* [in] */ Int32 reqModes,
        /* [in] */ Int32 seq,
        /* [in] */ IIInputContextCallback* icCallback);

    CARAPI GetExtractedText(
        /* [in] */ IExtractedTextRequest* request,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 seq,
        /* [in] */ IIInputContextCallback* icCallback);

    CARAPI DeleteSurroundingText(
        /* [in] */ Int32 leftLength,
        /* [in] */ Int32 rightLength);

    CARAPI SetComposingText(
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 newCursorPosition);

    CARAPI FinishComposingText();

    CARAPI CommitText(
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 newCursorPosition);

    CARAPI CommitCompletion(
        /* [in] */ ICompletionInfo* completion);

    CARAPI CommitCorrection(
        /* [in] */ ICorrectionInfo* correction);

    CARAPI SetSelection(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    CARAPI PerformEditorAction(
        /* [in] */ Int32 actionCode);

    CARAPI PerformContextMenuAction(
        /* [in] */ Int32 id);

    CARAPI BeginBatchEdit();

    CARAPI EndBatchEdit();

    CARAPI ReportFullscreenMode(
        /* [in] */ Boolean enabled);

    CARAPI SendKeyEvent(
        /* [in] */ IKeyEvent* event);

    CARAPI ClearMetaKeyStates(
        /* [in] */ Int32 states);

    CARAPI PerformPrivateCommand(
        /* [in] */ const String& action,
        /* [in] */ IBundle* data);

    CARAPI SetComposingRegion(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    CARAPI GetSelectedText(
        /* [in] */ Int32 flags,
        /* [in] */ Int32 seq,
        /* [in] */ IIInputContextCallback* icCallback);

    CARAPI RequestUpdateCursorAnchorInfo(
        /* [in] */ Int32 cursorUpdateMode,
        /* [in] */ Int32 seq,
        /* [in] */ IIInputContextCallback* icCallback);

private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;
};

}
}
}

#endif // __ELASTOS_DROID_JAVAPROXY_CINPUTCONTEXTNATIVE_H__
