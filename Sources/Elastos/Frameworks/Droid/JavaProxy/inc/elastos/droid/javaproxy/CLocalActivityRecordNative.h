
#ifndef __ELASTOS_DROID_JAVAPROXY_CLOCALACTIVITYRECORDNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CLOCALACTIVITYRECORDNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CLocalActivityRecordNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::App::ILocalActivityRecord;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CLocalActivityRecordNative)
    , public Object
    , public ILocalActivityRecord
    , public IBinder
{
public:
    ~CLocalActivityRecordNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CLOCALACTIVITYRECORDNATIVE_H__
