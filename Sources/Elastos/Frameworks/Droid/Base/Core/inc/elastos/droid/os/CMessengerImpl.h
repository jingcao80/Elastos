
#ifndef __ELASTOS_DROID_OS_CMESSENGERIMPL_H__
#define __ELASTOS_DROID_OS_CMESSENGERIMPL_H__

#include "_Elastos_Droid_Os_CMessengerImpl.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CMessengerImpl)
    , public Object
    , public IIMessenger
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IHandler* handler);

    CARAPI Send(
        /* [in] */ IMessage* message);

    TO_STRING_IMPL("Elastos::Droid::Os::CMessengerImpl");
private:
    //IHandler* mOwner;
    AutoPtr<IWeakReference> mOwner;//IHandler
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CMESSENGERIMPL_H__
