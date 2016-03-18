
#ifndef __ELASTOS_DROID_JAVAPROXY_CISYNCSTATUSOBSERVER_H__
#define __ELASTOS_DROID_JAVAPROXY_CISYNCSTATUSOBSERVER_H__

#include "_Elastos_Droid_JavaProxy_CISyncStatusObserver.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Content::IISyncStatusObserver;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CISyncStatusObserver)
    , public Object
    , public IISyncStatusObserver
    , public IBinder
{
public:
    ~CISyncStatusObserver();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI OnStatusChanged(
        /* [in] */ Int32 which);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CISYNCSTATUSOBSERVER_H__
