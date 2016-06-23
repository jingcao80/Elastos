
#ifndef __ELASTOS_DROID_OS_REGISTRANT_H__
#define __ELASTOS_DROID_OS_REGISTRANT_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include "elastos/droid/os/AsyncResult.h"

namespace Elastos {
namespace Droid {
namespace Os {

class RegistrantList;

/** @hide */
class Registrant
    : public Object
    , public IRegistrant
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    CARAPI Clear();

    CARAPI NotifyRegistrant();

    CARAPI NotifyResult(
        /* [in] */ IInterface* result);

    CARAPI NotifyException(
        /* [in] */ IThrowable* exception);

    CARAPI NotifyRegistrant(
        /* [in] */ IAsyncResult* ar);

    /*package*/
    void InternalNotifyRegistrant(
        /* [in] */ IInterface* result,
        /* [in] */ IThrowable* exception);

    /**
     * NOTE: May return null if weak reference has been collected
     */
    CARAPI MessageForRegistrant(
        /* [out] */ IMessage** result);

    CARAPI GetHandler(
        /* [out] */ IHandler** result);

private:
    AutoPtr<IWeakReference>     mRefH;
    Int32                       mWhat;
    AutoPtr<IInterface>         mUserObj;
    friend class RegistrantList;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_REGISTRANT_H__
