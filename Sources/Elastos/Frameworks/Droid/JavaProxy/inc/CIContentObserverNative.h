
#ifndef __CICONTENTOBSERVERNATIVE_H__
#define __CICONTENTOBSERVERNATIVE_H__

#include "_CIContentObserverNative.h"
#include <jni.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIContentObserverNative)
{
public:
    ~CIContentObserverNative();

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

    CARAPI OnChange(
        /* [in] */ Boolean selfUpdate,
        /* [in] */ IUri* uri);

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

#endif // __CICONTENTOBSERVERNATIVE_H__
