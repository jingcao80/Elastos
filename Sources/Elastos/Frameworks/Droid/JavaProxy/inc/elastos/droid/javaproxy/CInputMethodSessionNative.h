
#ifndef __ELASTOS_DROID_JAVAPROXY_CINPUTMETHODSESSIONNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CINPUTMETHODSESSIONNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CInputMethodSessionNative.h"
#include "Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::InputMethod::ICompletionInfo;
using Elastos::Droid::View::InputMethod::ICursorAnchorInfo;
using Elastos::Droid::View::InputMethod::IExtractedText;
using Elastos::Droid::View::InputMethod::IInputMethodSession;
using Elastos::Droid::View::InputMethod::ILocalInputMethodSessionEventCallback;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CInputMethodSessionNative)
    , public Object
    , public IInputMethodSession
    , public IInputMethodSessionNative
    , public IBinder
{
public:
    ~CInputMethodSessionNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI SetEnabled(
        /* [in] */ Boolean enabled);

    CARAPI FinishInput();

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
        /* [in] */ ArrayOf<ICompletionInfo *>* completions);

    CARAPI DispatchKeyEvent(
        /* [in] */ Int32 seq,
        /* [in] */ IKeyEvent* event,
        /* [in] */ ILocalInputMethodSessionEventCallback* eventCallback);

    CARAPI DispatchTrackballEvent(
        /* [in] */ Int32 seq,
        /* [in] */ IMotionEvent* event,
        /* [in] */ ILocalInputMethodSessionEventCallback* eventCallback);

    CARAPI DispatchGenericMotionEvent(
        /* [in] */ Int32 seq,
        /* [in] */ IMotionEvent* event,
        /* [in] */ ILocalInputMethodSessionEventCallback* cb);

    CARAPI UpdateExtractedText(
        /* [in] */ Int32 token,
        /* [in] */ IExtractedText* text);

    CARAPI AppPrivateCommand(
        /* [in] */ const String& action,
        /* [in] */ IBundle* data);

    CARAPI ToggleSoftInput(
        /* [in] */ Int32 showFlags,
        /* [in] */ Int32 hideFlags);

    CARAPI UpdateCursorAnchorInfo(
        /* [in] */ ICursorAnchorInfo* cursorAnchorInfo);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;
};

}
}
}

#endif // __ELASTOS_DROID_JAVAPROXY_CINPUTMETHODSESSIONNATIVE_H__
