
#ifndef __CICANCELLATIONSIGNALNATIVE_H__
#define __CICANCELLATIONSIGNALNATIVE_H__

#include "_CICancellationSignalNative.h"
#include <jni.h>

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CICancellationSignalNative)
{
public:
    ~CICancellationSignalNative();

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

    CARAPI Cancel();

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

#endif // __CICANCELLATIONSIGNALNATIVE_H__
