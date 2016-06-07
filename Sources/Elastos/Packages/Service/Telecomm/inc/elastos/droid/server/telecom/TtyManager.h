
#ifndef __ELASTOS_DROID_SERVER_TELECOM_TTYMANAGER_H__
#define __ELASTOS_DROID_SERVER_TELECOM_TTYMANAGER_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include "elastos/droid/server/telecom/WiredHeadsetManager.h"
#include <elastos/core/Object.h>
#include <elastos/droid/R.h>
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

// TODO: Needed for move to system service: import com.android.internal.R;
class TtyManager
    : public Object
    , public IWiredHeadsetManagerListener
{
public:
    class TtyBroadcastReceiver
        : public BroadcastReceiver
        , public ITtyManagerTtyBroadcastReceiver
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ IInterface* host);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        TtyManager* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    TtyManager();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ WiredHeadsetManager* wiredHeadsetManager);

    CARAPI IsTtySupported(
        /* [out] */ Boolean* result);

    CARAPI GetCurrentTtyMode(
        /* [out] */ Int32* result);

    // @Override
    CARAPI OnWiredHeadsetPluggedInChanged(
        /* [in] */ Boolean oldIsPluggedIn,
        /* [in] */ Boolean newIsPluggedIn);

private:
    CARAPI UpdateCurrentTtyMode();

    CARAPI UpdateAudioTtyMode();

private:
    AutoPtr<ITtyManagerTtyBroadcastReceiver> mReceiver;

    AutoPtr<IContext> mContext;

    AutoPtr<WiredHeadsetManager> mWiredHeadsetManager;

    Int32 mPreferredTtyMode;

    Int32 mCurrentTtyMode;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_TTYMANAGER_H__