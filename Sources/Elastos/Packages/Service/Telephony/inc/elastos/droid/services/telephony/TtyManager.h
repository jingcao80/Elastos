#ifndef  __ELASTOS_DROID_SERVICES_TELEPHONY_TTYMANAGER_H__
#define  __ELASTOS_DROID_SERVICES_TELEPHONY_TTYMANAGER_H__

#include "_Elastos.Droid.Server.Telephony.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/os/Handler.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Services {
namespace Telephony {

class TtyManager
    : public Object
    , public ITtyManager
{
public:
    class TtyBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("TtyManager::TtyBroadcastReceiver")

        CARAPI constructor(
            /* [in] */ ITtyManager* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        TtyManager* mHost;
    };

private:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("TtyManager::MyHandler")

        MyHandler(
            /* [in] */ TtyManager* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        TtyManager* mHost;
    };

public:
    TO_STRING_IMPL("TtyManager")

    CAR_INTERFACE_DECL()

    TtyManager(
        /* [in] */ IContext* context,
        /* [in] */ IPhone* phone);

private:
    CARAPI_(void) UpdateTtyMode(
        /* [in] */ Int32 ttyMode);

    static CARAPI_(Int32) TelecomModeToPhoneMode(
        /* [in] */ Int32 telecomMode);

    static CARAPI_(Int32) PhoneModeToTelecomMode(
        /* [in] */ Int32 phoneMode);

private:
    static const Int32 MSG_SET_TTY_MODE_RESPONSE = 1;
    static const Int32 MSG_GET_TTY_MODE_RESPONSE = 2;

    AutoPtr<IBroadcastReceiver> mReceiver;
    AutoPtr<IPhone> mPhone;
    Int32 mTtyMode;

    AutoPtr<IHandler> mHandler;
};

} // namespace Telephony
} // namespace Services
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICES_TELEPHONY_TTYMANAGER_H__