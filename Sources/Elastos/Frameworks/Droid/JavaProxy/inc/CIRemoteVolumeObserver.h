
#ifndef __CIREMOTEVOLUMEOBSERVER_H__
#define __CIREMOTEVOLUMEOBSERVER_H__

#include "_CIRemoteVolumeObserver.h"
#include <jni.h>

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIRemoteVolumeObserver)
{
public:
    ~CIRemoteVolumeObserver();

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

    CARAPI DispatchRemoteVolumeUpdate(
        /* [in] */ Int32 direction,
        /* [in] */ Int32 value);

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

#endif // __CIREMOTEVOLUMEOBSERVER_H__
