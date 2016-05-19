
#ifndef __ELASTOS_DROID_JAVAPROXY_CIPACKAGEDELETEOBSERVER2NATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIPACKAGEDELETEOBSERVER2NATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIPackageDeleteObserver2Native.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IIPackageDeleteObserver2;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIPackageDeleteObserver2Native)
    , public Object
    , public IIPackageDeleteObserver2
    , public IBinder
{
public:
    ~CIPackageDeleteObserver2Native();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI OnUserActionRequired(
        /* [in] */ IIntent* intent);

    CARAPI OnPackageDeleted(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 returnCode,
        /* [in] */ const String& msg);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CIPACKAGEDELETEOBSERVER2NATIVE_H__
