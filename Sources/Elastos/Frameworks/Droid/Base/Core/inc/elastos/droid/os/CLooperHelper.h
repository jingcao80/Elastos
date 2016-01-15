
#ifndef __ELASTOS_DROID_OS_CLOOPERHELPER_H__
#define __ELASTOS_DROID_OS_CLOOPERHELPER_H__

#include "_Elastos_Droid_Os_CLooperHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CLooperHelper)
    , public Singleton
    , public ILooperHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI Prepare();

    CARAPI PrepareMainLooper();

    CARAPI GetMyLooper(
        /* [out] */ ILooper** looper);

    CARAPI GetMyQueue(
        /* [out] */ IMessageQueue** queue);

    CARAPI GetMainLooper(
        /* [out] */ ILooper** looper);

    CARAPI Loop();
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CLOOPERHELPER_H__
