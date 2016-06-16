
#ifndef __ELASTOS_DROID_OS_ASYNCRESULT_H__
#define __ELASTOS_DROID_OS_ASYNCRESULT_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::IThrowable;

namespace Elastos {
namespace Droid {
namespace Os {

/** @hide */
class ECO_PUBLIC AsyncResult
    : public Object
{
public:

    /*************************** Instance Variables **************************/

    // Expect either exception or result to be null
    AutoPtr<IInterface> mUserObj;
    AutoPtr<IThrowable> mException;
    AutoPtr<IInterface> mResult;

    /***************************** Class Methods *****************************/

    /** Saves and sets m.obj */
    static AutoPtr<AsyncResult> ForMessage(
        /* [in] */ IMessage* m,
        /* [in] */ IInterface* r,
        /* [in] */ IThrowable* ex);

    /** Saves and sets m.obj */
    static AutoPtr<AsyncResult> ForMessage(
        /* [in] */ IMessage* m);

    /** please note, this sets m.obj to be this */
    AsyncResult (
        /* [in] */ IInterface* uo,
        /* [in] */ IInterface* r,
        /* [in] */ IThrowable* ex);
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_ASYNCRESULT_H__
