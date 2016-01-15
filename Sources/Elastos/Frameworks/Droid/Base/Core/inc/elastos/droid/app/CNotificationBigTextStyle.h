
#ifndef __ELASTOS_DROID_APP_CNOTIFICATIONINBOXSTYLE_H__
#define __ELASTOS_DROID_APP_CNOTIFICATIONINBOXSTYLE_H__

#include "_Elastos_Droid_App_CNotificationBigTextStyle.h"
#include "elastos/droid/app/NotificationStyle.h"

namespace Elastos {
namespace Droid {
namespace App {

/**
 * Helper class for generating large-format notifications that include a lot of text.
 *
 * This class is a "rebuilder": It consumes a Builder object and modifies its behavior, like so:
 * <pre class="prettyprint">
 * Notification noti = new Notification.BigTextStyle(
 *      new Notification.Builder()
 *         .setContentTitle(&quot;New mail from &quot; + sender.toString())
 *         .setContentText(subject)
 *         .setSmallIcon(R.drawable.new_mail)
 *         .setLargeIcon(aBitmap))
 *      .bigText(aVeryLongString)
 *      .build();
 * </pre>
 *
 * @see Notification#bigContentView
 */
CarClass(CNotificationBigTextStyle)
    , public NotificationStyle
    , public INotificationBigTextStyle
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CNotificationBigTextStyle();

    virtual ~CNotificationBigTextStyle();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ INotificationBuilder* builder);

    /**
     * Overrides ContentTitle in the big form of the template.
     * This defaults to the value passed to setContentTitle().
     */
    CARAPI SetBigContentTitle(
        /* [in] */ ICharSequence* title);

    /**
     * Set the first line of text after the detail section in the big form of the template.
     */
    CARAPI SetSummaryText(
        /* [in] */ ICharSequence* cs);

    /**
     * Provide the longer text to be displayed in the big form of the
     * template in place of the content text.
     */
    CARAPI BigText(
        /* [in] */ ICharSequence* cs);

    CARAPI AddExtras(
        /* [in] */ IBundle* extras);

    /**
     * @hide
     */
    CARAPI RestoreFromExtras(
        /* [in] */ IBundle* extras);

    CARAPI PopulateBigContentView(
        /* [in] */ INotification* wip);

private:
    AutoPtr<IRemoteViews> MakeBigContentView();

    Int32 CalculateMaxLines();

private:
    static const Int32 MAX_LINES;// = 13;
    static const Int32 LINES_CONSUMED_BY_ACTIONS;// = 3;
    static const Int32 LINES_CONSUMED_BY_SUMMARY;// = 2;

    AutoPtr<ICharSequence> mBigText;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_CNOTIFICATIONINBOXSTYLE_H__
