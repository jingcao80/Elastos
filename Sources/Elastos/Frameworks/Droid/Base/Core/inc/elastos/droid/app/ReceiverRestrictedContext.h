
#ifndef __ELASTOS_DROID_APP_RECEIVERRESTRICTEDCONTEXT_H__
#define __ELASTOS_DROID_APP_RECEIVERRESTRICTEDCONTEXT_H__

#include "Elastos.Droid.App.h"
#include "elastos/droid/content/ContextWrapper.h"

using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::ContextWrapper;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace App {

class ReceiverRestrictedContext
    : public ContextWrapper
    , public IReceiverRestrictedContext
{
public:
    CAR_INTERFACE_DECL()

    ReceiverRestrictedContext();

    virtual ~ReceiverRestrictedContext();

    CARAPI constructor(
        /* [in] */ IContext* base);

    CARAPI RegisterReceiver(
        /* [in] */ IBroadcastReceiver* receiver,
        /* [in] */ IIntentFilter* filter,
        /* [out] */ IIntent** intent);

    CARAPI RegisterReceiver(
        /* [in] */ IBroadcastReceiver* receiver,
        /* [in] */ IIntentFilter* filter,
        /* [in] */ const String& broadcastPermission,
        /* [in] */ IHandler* scheduler,
        /* [out] */ IIntent** intent);

    CARAPI RegisterReceiverAsUser(
        /* [in] */ IBroadcastReceiver* receiver,
        /* [in] */ IUserHandle* user,
        /* [in] */ IIntentFilter* filter,
        /* [in] */ const String& broadcastPermission,
        /* [in] */ IHandler* scheduler,
        /* [out] */ IIntent** intent);

    CARAPI BindService(
        /* [in] */ IIntent* service,
        /* [in] */ IServiceConnection* conn,
        /* [in] */ Int32 flags,
        /* [out] */ Boolean* succeeded);
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_RECEIVERRESTRICTEDCONTEXT_H__
