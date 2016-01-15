
#ifndef __ELASTOS_DROID_OS_CMESSAGEHELPER_H__
#define __ELASTOS_DROID_OS_CMESSAGEHELPER_H__

#include "_Elastos_Droid_Os_CMessageHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CMessageHelper)
    , public Singleton
    , public IMessageHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI Obtain(
        /* [out] */ IMessage** msg);

    CARAPI Obtain(
        /* [in] */ IMessage* orig,
        /* [out] */ IMessage** msg);

    CARAPI Obtain(
        /* [in] */ IHandler* target,
        /* [out] */ IMessage** msg);

    CARAPI Obtain(
        /* [in] */ IHandler* target,
        /* [in] */ Int32 what,
        /* [out] */ IMessage** msg);

    CARAPI Obtain(
        /* [in] */ IHandler* target,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj,
        /* [out] */ IMessage** msg);

    CARAPI Obtain(
        /* [in] */ IHandler* target,
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2,
        /* [out] */ IMessage** msg);

    CARAPI Obtain(
        /* [in] */ IHandler* target,
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2,
        /* [in] */ IInterface* obj,
        /* [out] */ IMessage** msg);

    CARAPI Obtain(
        /* [in] */ IHandler* target,
        /* [in] */ IRunnable* callback,
        /* [out] */ IMessage** msg);
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__CMESSAGEHELPER_H__
