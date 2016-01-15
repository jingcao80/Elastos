
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
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IHandler* handler);

    CARAPI Send(
        /* [in] */ IMessage* message);

private:
    IHandler* mOwner;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CMESSENGERIMPL_H__
