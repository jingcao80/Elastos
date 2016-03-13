
#ifndef __ELASTOS_DROID_JAVAPROXY_CIMOUNTSHUTDOWNOBSERVERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIMOUNTSHUTDOWNOBSERVERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIMountShutdownObserverNative.h"
#include <jni.h>

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIMountShutdownObserverNative)
{
public:
    ~CIMountShutdownObserverNative();

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

    CARAPI OnShutDownComplete(
        /* [in] */ Int32 statusCode);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CIMOUNTSHUTDOWNOBSERVERNATIVE_H__
