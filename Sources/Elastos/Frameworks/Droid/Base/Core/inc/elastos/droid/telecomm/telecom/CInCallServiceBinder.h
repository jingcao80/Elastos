#ifndef __ELASTOS_DROID_TELECOMM_TELECOM_CINCALLSERVICEBINDER_H__
#define __ELASTOS_DROID_TELECOMM_TELECOM_CINCALLSERVICEBINDER_H__

#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_Telecomm_Telecom_CInCallServiceBinder.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Telecomm::Internal::IIInCallAdapter;
using Elastos::Droid::Telecomm::Internal::IIInCallService;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

/** Manages the binder calls so that the implementor does not need to deal with it. */
CarClass(CInCallServiceBinder)
    , public Object
    , public IIInCallService
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IHandler* handler);

    CARAPI SetInCallAdapter(
        /* [in] */ IIInCallAdapter* inCallAdapter);

    CARAPI AddCall(
        /* [in] */ IParcelableCall* call);

    CARAPI UpdateCall(
        /* [in] */ IParcelableCall* call);

    CARAPI SetPostDial(
        /* [in] */ const String& callId,
        /* [in] */ const String& remaining);

    CARAPI SetPostDialWait(
        /* [in] */ const String& callId,
        /* [in] */ const String& remaining);

    CARAPI OnAudioStateChanged(
        /* [in] */ IAudioState* audioState);

    CARAPI BringToForeground(
        /* [in] */ Boolean showDialpad);

public:
    AutoPtr<IHandler> mHandler;
};

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOMM_TELECOM_CINCALLSERVICEBINDER_H__