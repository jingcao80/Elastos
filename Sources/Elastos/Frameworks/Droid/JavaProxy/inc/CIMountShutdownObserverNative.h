
#ifndef __CIMOUNTSHUTDOWNOBSERVERNATIVE_H__
#define __CIMOUNTSHUTDOWNOBSERVERNATIVE_H__

#include "_CIMountShutdownObserverNative.h"
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

#endif // __CIMOUNTSHUTDOWNOBSERVERNATIVE_H__
