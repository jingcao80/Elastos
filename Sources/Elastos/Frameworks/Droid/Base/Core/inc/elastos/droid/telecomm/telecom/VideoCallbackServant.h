#ifndef __ELASTOS_DROID_TELECOMM_TELECOM_VIDEOCALLBACKSERVANT_H__
#define __ELASTOS_DROID_TELECOMM_TELECOM_VIDEOCALLBACKSERVANT_H__

#include "Elastos.Droid.Telecomm.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Telecomm::Internal::IIVideoCallback;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

/**
 * A component that provides an RPC servant implementation of {@link IVideoCallback},
 * posting incoming messages on the main thread on a client-supplied delegate object.
 *
 * TODO: Generate this and similar classes using a compiler starting from AIDL interfaces.
 *
 * @hide
 */
class VideoCallbackServant
    : public Object
    , public IVideoCallbackServant
{
public:
    class MyHandler
        : public Handler
    {
    public:
        MyHandler(
            /* [in] */ VideoCallbackServant* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        // Internal method defined to centralize handling of RemoteException
        CARAPI_(void) InternalHandleMessage(
            /* [in] */ IMessage* msg);

    public:
        VideoCallbackServant* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    VideoCallbackServant();

    CARAPI constructor(
        /* [in] */ IIVideoCallback* delegate);

    CARAPI GetStub(
        /* [out] */ IIVideoCallback** result);

public:
    static const Int32 MSG_RECEIVE_SESSION_MODIFY_REQUEST;
    static const Int32 MSG_RECEIVE_SESSION_MODIFY_RESPONSE;
    static const Int32 MSG_HANDLE_CALL_SESSION_EVENT;
    static const Int32 MSG_CHANGE_PEER_DIMENSIONS;
    static const Int32 MSG_CHANGE_CALL_DATA_USAGE;
    static const Int32 MSG_CHANGE_CAMERA_CAPABILITIES;

    AutoPtr<IIVideoCallback> mDelegate;

    AutoPtr<IHandler> mHandler;

    AutoPtr<IIVideoCallback> mStub;
};

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOMM_TELECOM_VIDEOCALLBACKSERVANT_H__