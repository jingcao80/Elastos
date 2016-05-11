
#ifndef __ELASTOS_DROID_JAVAPROXY_CIVOLUMECONTROLLERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIVOLUMECONTROLLERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIVolumeControllerNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Media::IIVolumeController;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIVolumeControllerNative)
    , public Object
    , public IIVolumeController
    , public IBinder
{
public:
    ~CIVolumeControllerNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI DisplaySafeVolumeWarning(
        /* [in] */ Int32 flags);

    CARAPI VolumeChanged(
        /* [in] */ Int32 streamType,
        /* [in] */ Int32 flags);

    CARAPI MasterVolumeChanged(
        /* [in] */ Int32 flags);

    CARAPI MasterMuteChanged(
        /* [in] */ Int32 flags);

    CARAPI SetLayoutDirection(
        /* [in] */ Int32 layoutDirection);

    CARAPI Dismiss();

    CARAPI ToString(
        /* [out] */ String* str);

private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;
};

} // namespace JavaProxy
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_JAVAPROXY_CIVOLUMECONTROLLERNATIVE_H__
