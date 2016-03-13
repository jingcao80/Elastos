
#ifndef __ELASTOS_DROID_JAVAPROXY_CIINPUTCONTEXTCALLBACKNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIINPUTCONTEXTCALLBACKNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIInputContextCallbackNative.h"
#include <jni.h>

using Elastos::Core::ICharSequence;
using Elastos::Droid::View::InputMethod::IExtractedText;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIInputContextCallbackNative)
{
public:
    ~CIInputContextCallbackNative();

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

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


private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;
};

}
}
}

#endif // __ELASTOS_DROID_JAVAPROXY_CIINPUTCONTEXTCALLBACKNATIVE_H__
