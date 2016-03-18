
#ifndef __ELASTOS_DROID_JAVAPROXY_CIINPUTCONTEXTCALLBACKNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIINPUTCONTEXTCALLBACKNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIInputContextCallbackNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Internal::View::IIInputContextCallback;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::View::InputMethod::IExtractedText;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIInputContextCallbackNative)
    , public Object
    , public IIInputContextCallback
    , public IBinder
{
public:
    ~CIInputContextCallbackNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CIINPUTCONTEXTCALLBACKNATIVE_H__
