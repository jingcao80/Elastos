
#ifndef __ELASTOS_DROID_JAVAPROXY_CIAUDIOFOCUSDISPATCHER_H__
#define __ELASTOS_DROID_JAVAPROXY_CIAUDIOFOCUSDISPATCHER_H__

#include "_Elastos_Droid_JavaProxy_CIAudioFocusDispatcher.h"
#include <jni.h>

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIAudioFocusDispatcher)
{
public:
    ~CIAudioFocusDispatcher();

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

    CARAPI DispatchAudioFocusChange(
        /* [in] */ Int32 focusChange,
        /* [in] */ const String& clientId);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CIAUDIOFOCUSDISPATCHER_H__
