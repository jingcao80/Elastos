#ifndef __ELASTOS_DROID_APP_NOTIFICATIONSTYLE_H__
#define __ELASTOS_DROID_APP_NOTIFICATIONSTYLE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.App.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Widget::IRemoteViews;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace App {

/**
 * An object that can apply a rich notification style to a {@link Notification.Builder}
 * object.
 */
class NotificationStyle
    : public Object
    , public INotificationStyle
{
public:
    CAR_INTERFACE_DECL()

    NotificationStyle();

    virtual ~NotificationStyle();

    CARAPI SetBuilder(
        /* [in] */ INotificationBuilder * builder);

    /**
     * Calls {@link android.app.Notification.Builder#build()} on the Builder this Style is
     * attached to.
     *
     * @return the fully constructed Notification.
     */
    CARAPI Build(
        /* [out] */ INotification** result);

    /**
     * @hide
     */
    CARAPI AddExtras(
        /* [in] */ IBundle* extras);

    /**
     * @hide
     */
    CARAPI BuildStyled(
        /* [in] */ INotification* wip);

    /**
     * @hide
     * @return true if the style positions the progress bar on the second line; false if the
     *         style hides the progress bar
     */
    CARAPI_(Boolean) HasProgress();

    /**
     * Overrides ContentTitle in the big form of the template.
     * This defaults to the value passed to setContentTitle().
     */
    CARAPI_(void) InternalSetBigContentTitle(
        /* [in] */ ICharSequence * title);

    /**
     * Set the first line of text after the detail section in the big form of the template.
     */
    CARAPI_(void) InternalSetSummaryText(
        /* [in] */ ICharSequence * cs);

    CARAPI CheckBuilder();

    CARAPI_(AutoPtr<IRemoteViews>) GetStandardView(
        /* [in] */ Int32 layoutId);

    /**
     * Changes the padding of the first line such that the big and small content view have the
     * same top padding.
     *
     * @hide
     */
    CARAPI ApplyTopPadding(
        /* [in] */ IRemoteViews* contentView);

    /**
     * @hide
     */
    CARAPI RestoreFromExtras(
        /* [in] */ IBundle* extras);


    // The following methods are split out so we can re-create notification partially.
    /**
     * @hide
     */
    CARAPI PopulateTickerView(
        /* [in] */ INotification* wip);

    /**
     * @hide
     */
    CARAPI PopulateContentView(
        /* [in] */ INotification* wip);

    /**
     * @hide
     */
    CARAPI PopulateBigContentView(
        /* [in] */ INotification* wip);

    /**
     * @hide
     */
    CARAPI PopulateHeadsUpContentView(
        /* [in] */ INotification* wip);

public:
    AutoPtr<ICharSequence> mBigContentTitle;
    AutoPtr<ICharSequence> mSummaryText;
    Boolean mSummaryTextSet;
    AutoPtr<INotificationBuilder> mBuilder;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_NOTIFICATIONSTYLE_H__
