
#ifndef __ELASTOS_DROID_OS_REGISTRANT_H__
#define __ELASTOS_DROID_OS_REGISTRANT_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include "elastos/droid/os/AsyncResult.h"

namespace Elastos {
namespace Droid {
namespace Os {

/** @hide */
class Registrant
    : Object
{
public:
    Registrant(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    void Clear();

    void NotifyRegistrant();

    void NotifyResult(
        /* [in] */ IInterface* result);

    void NotifyException(
        /* [in] */ IThrowable* exception);

    void NotifyRegistrant(
        /* [in] */ AsyncResult* ar);

    /*package*/
    void InternalNotifyRegistrant(
        /* [in] */ IInterface* result,
        /* [in] */ IThrowable* exception);

    /**
     * NOTE: May return null if weak reference has been collected
     */
    AutoPtr<IMessage> MessageForRegistrant();

    AutoPtr<IHandler> GetHandler();

private:
    AutoPtr<IWeakReference>     mRefH;
    Int32                       mWhat;
    AutoPtr<IInterface>         mUserObj;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_REGISTRANT_H__
