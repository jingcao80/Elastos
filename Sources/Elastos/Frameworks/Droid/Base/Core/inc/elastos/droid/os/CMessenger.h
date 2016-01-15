
#ifndef __ELASTOS_DROID_OS_CMESSENGER_H__
#define __ELASTOS_DROID_OS_CMESSENGER_H__

#include "_Elastos_Droid_Os_CMessenger.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IIMessenger;

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CMessenger)
    , public Object
    , public IMessenger
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IHandler* handler);

    CARAPI constructor(
        /* [in] */ IIMessenger* target);

    CARAPI Send(
        /* [in] */ IMessage* message);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetBinder(
        /* [out] */ IBinder** mgr);

    CARAPI GetIMessenger(
        /* [out] */ IIMessenger** mgr);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashcode);

    CARAPI Equals(
        /* [in] */ IMessenger* other,
        /* [out] */ Boolean* equals);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* equals);

private:
    AutoPtr<IIMessenger> mTarget;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CMESSENGER_H__
