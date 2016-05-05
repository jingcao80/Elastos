
#ifndef __ELASTOS_DROID_APP_NOTIFICATIONACTIONBUILDER_H__
#define __ELASTOS_DROID_APP_NOTIFICATIONACTIONBUILDER_H__

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/Object.h>

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Os::IBundle;
using Elastos::Core::ICharSequence;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace App {

class NotificationActionBuilder
    : public Object
    , public INotificationActionBuilder
{
public:
    CAR_INTERFACE_DECL()

    NotificationActionBuilder();

    /**
     * Construct a new builder for {@link Action} object.
     * @param icon icon to show for this action
     * @param title the title of the action
     * @param intent the {@link PendingIntent} to fire when users trigger this action
     */
    CARAPI constructor(
        /* [in] */ Int32 icon,
        /* [in] */ ICharSequence* title,
        /* [in] */ IPendingIntent* intent);

    /**
     * Construct a new builder for {@link Action} object using the fields from an
     * {@link Action}.
     * @param action the action to read fields from.
     */
    CARAPI constructor(
        /* [in] */ INotificationAction* action);

    /**
     * Merge additional metadata into this builder.
     *
     * <p>Values within the Bundle will replace existing extras values in this Builder.
     *
     * @see Notification.Action#extras
     */
    CARAPI AddExtras(
        /* [in] */ IBundle* extras);

    /**
     * Get the metadata Bundle used by this Builder.
     *
     * <p>The returned Bundle is shared with this Builder.
     */
    CARAPI GetExtras(
        /* [out] */ IBundle** result);

    /**
     * Add an input to be collected from the user when this action is sent.
     * Response values can be retrieved from the fired intent by using the
     * {@link RemoteInput#getResultsFromIntent} function.
     * @param remoteInput a {@link RemoteInput} to add to the action
     * @return this object for method chaining
     */
    CARAPI AddRemoteInput(
        /* [in] */ IRemoteInput* remoteInput);

    /**
     * Apply an extender to this action builder. Extenders may be used to add
     * metadata or change options on this builder.
     */
    CARAPI Extend(
        /* [in] */ INotificationActionExtender* extender);

    /**
     * Combine all of the options that have been set and return a new {@link Action}
     * object.
     * @return the built action
     */
    CARAPI Build(
        /* [out] */ INotificationAction** result);

private:
    CARAPI constructor(
        /* [in] */ Int32 icon,
        /* [in] */ ICharSequence* title,
        /* [in] */ IPendingIntent* intent,
        /* [in] */ IBundle* extras,
        /* [in] */ ArrayOf<IRemoteInput*>* remoteInputs);

private:
    Int32 mIcon;

    AutoPtr<ICharSequence> mTitle;

    AutoPtr<IPendingIntent> mIntent;

    AutoPtr<IBundle> mExtras;

    AutoPtr<IArrayList> mRemoteInputs;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_NOTIFICATIONACTIONBUILDER_H__
