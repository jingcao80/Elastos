
#ifndef __CISYNCSTATUSOBSERVER_H__
#define __CISYNCSTATUSOBSERVER_H__

#include "_CISyncStatusObserver.h"
#include <jni.h>

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CISyncStatusObserver)
{
public:
    ~CISyncStatusObserver();

    CARAPI OnStatusChanged(
        /* [in] */ Int32 which);

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;
};

}
}
}

#endif // __CISYNCSTATUSOBSERVER_H__
