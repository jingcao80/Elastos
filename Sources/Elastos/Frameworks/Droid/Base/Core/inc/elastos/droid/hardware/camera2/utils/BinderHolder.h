
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_BINDERHOLDER_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_BINDERHOLDER_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

class BinderHolder
    : public Object
    , public IBinderHolder
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    BinderHolder() {}

    virtual ~BinderHolder() {}

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IBinder* binder);

    //@Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    //@Override
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI GetBinder(
        /* [out] */ IBinder** binder);

    CARAPI SetBinder(
        /* [in] */ IBinder* binder);

    // public static final Parcelable.Creator<BinderHolder> CREATOR =
    //          new Parcelable.Creator<BinderHolder>() {
    //      @Override
    //      public BinderHolder createFromParcel(Parcel in) {
    //          return new BinderHolder(in);
    //      }

    //      @Override
    //      public BinderHolder[] newArray(int size) {
    //          return new BinderHolder[size];
    //      }
    // };

private:
    CARAPI constructor(
        /* [in] */ IParcel* _in);

private:
    AutoPtr<IBinder> mBinder;
};

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_BINDERHOLDER_H__
