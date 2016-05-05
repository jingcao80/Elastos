#ifndef __ELASTOS_DROID_APP_CNOTIFICATIONACTION_H__
#define __ELASTOS_DROID_APP_CNOTIFICATIONACTION_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.App.h"
#include "elastos/droid/app/CNotification.h"
#include <elastos/core/Object.h>
#include "_Elastos_Droid_App_CNotificationAction.h"

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::App::IRemoteInput;
using Elastos::Core::ICharSequence;
using Elastos::Core::ICloneable;

namespace Elastos {
namespace Droid {
namespace App {

/**
 * Structure to encapsulate a named action that can be shown as part of this notification.
 * It must include an icon, a label, and a {@link PendingIntent} to be fired when the action is
 * selected by the user.
 * <p>
 * Apps should use {@link Notification.Builder#addAction(int, CharSequence, PendingIntent)}
 * or {@link Notification.Builder#addAction(Notification.Action)}
 * to attach actions.
 */
CarClass(CNotificationAction)
    , public Object
    , public INotificationAction
    , public IParcelable
    , public ICloneable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    friend class CNotificationBuilder;

    CNotificationAction();

    ~CNotificationAction();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 icon_,
        /* [in] */ ICharSequence* title_,
        /* [in] */ IPendingIntent* intent_);

    CARAPI constructor(
        /* [in] */ Int32 icon_,
        /* [in] */ ICharSequence* title_,
        /* [in] */ IPendingIntent* intent_,
        /* [in] */ IBundle* extras,
        /* [in] */ ArrayOf<IRemoteInput*>* remoteInputs);

    /**
     * Get additional metadata carried around with this Action.
     */
    CARAPI GetExtras(
        /* [out] */ IBundle** extras);

    /**
     * Get the list of inputs to be collected from the user when this action is sent.
     * May return null if no remote inputs were added.
     */
    CARAPI GetRemoteInputs(
        /* [out, callee] */ ArrayOf<IRemoteInput*>** inputs);

    CARAPI Clone(
        /* [out] */ IInterface** obj);

    CARAPI Clone(
        /* [out] */ INotificationAction** obj) { return NOERROR;}

    CARAPI GetIcon(
        /* [out] */ Int32* icon);

    CARAPI SetIcon(
        /* [in] */ Int32 icon);

    CARAPI GetTitle(
        /* [out] */ ICharSequence** title);

    CARAPI SetTitle(
        /* [in] */ ICharSequence* title);

    CARAPI GetActionIntent(
        /* [out] */ IPendingIntent** actionIntent);

    CARAPI SetActionIntent(
        /* [in] */ IPendingIntent* actionIntent);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* out);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* in);

public:
    AutoPtr<IBundle> mExtras;
    AutoPtr<ArrayOf<IRemoteInput*> > mRemoteInputs;

    /**
     * Small icon representing the action.
     */
    Int32 mIcon;

    /**
     * Title of the action.
     */
    AutoPtr<ICharSequence> mTitle;

    /**
     * Intent to send when the user invokes this action. May be null, in which case the action
     * may be rendered in a disabled presentation by the system UI.
     */
    AutoPtr<IPendingIntent> mActionIntent;
};

}
}
}

#endif // __ELASTOS_DROID_APP_CNOTIFICATIONACTION_H__
