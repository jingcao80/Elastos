
#ifndef __ELASTOS_DROID_SERVER_MEDIA_CPROVIDERCALLBACK_H__
#define __ELASTOS_DROID_SERVER_MEDIA_CPROVIDERCALLBACK_H__

#include "_Elastos_Droid_Server_Media_CProviderCallback.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Media::IIRemoteDisplayCallback;
using Elastos::Droid::Media::IRemoteDisplayState;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Media {

CarClass(CProviderCallback)
    , public Object
    , public IIRemoteDisplayCallback
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 host);

    CARAPI_(void) Dispose();

    CARAPI OnStateChanged(
        /* [in] */ IRemoteDisplayState* state);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    AutoPtr<IWeakReference> mConnectionRef;
};

} // namespace Media
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_MEDIA_CPROVIDERCALLBACK_H__
