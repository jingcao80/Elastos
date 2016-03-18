
#ifndef __ELASTOS_DROID_JAVAPROXY_CIINPUTMETHODSESSIONNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIINPUTMETHODSESSIONNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIInputMethodSessionNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Internal::View::IIInputMethodSession;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::InputMethod::ICompletionInfo;
using Elastos::Droid::View::InputMethod::ICursorAnchorInfo;
using Elastos::Droid::View::InputMethod::IExtractedText;
using Elastos::Droid::View::InputMethod::IInputMethodSession;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIInputMethodSessionNative)
    , public Object
    , public IIInputMethodSession
    , public IBinder
{
public:
    ~CIInputMethodSessionNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI GetInternalInputMethodSession(
        /* [out] */ IInputMethodSessionNative** session);

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
        /* [in] */ ArrayOf<ICompletionInfo *>* completions);

    CARAPI AppPrivateCommand(
        /* [in] */ const String& action,
        /* [in] */ IBundle* data);

    CARAPI ToggleSoftInput(
        /* [in] */ Int32 showFlags,
        /* [in] */ Int32 hideFlags);

    CARAPI FinishSession();

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

#endif // __ELASTOS_DROID_JAVAPROXY_CIINPUTMETHODSESSIONNATIVE_H__
