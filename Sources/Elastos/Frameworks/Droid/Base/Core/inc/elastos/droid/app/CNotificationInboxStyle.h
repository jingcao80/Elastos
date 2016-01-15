
#ifndef __ELASTOS_DROID_APP_CNOTIFICATIONINBOXSTYLE_H__
#define __ELASTOS_DROID_APP_CNOTIFICATIONINBOXSTYLE_H__

#include "_Elastos_Droid_App_CNotificationInboxStyle.h"
#include "elastos/droid/app/NotificationStyle.h"

using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace App {

/**
 * Helper class for generating large-format notifications that include a list of (up to 5) strings.
 *
 * This class is a "rebuilder": It consumes a Builder object and modifies its behavior, like so:
 * <pre class="prettyprint">
 * Notification noti = new Notification.InboxStyle(
 *      new Notification.Builder()
 *         .setContentTitle(&quot;5 New mails from &quot; + sender.toString())
 *         .setContentText(subject)
 *         .setSmallIcon(R.drawable.new_mail)
 *         .setLargeIcon(aBitmap))
 *      .addLine(str1)
 *      .addLine(str2)
 *      .setContentTitle("")
 *      .setSummaryText(&quot;+3 more&quot;)
 *      .build();
 * </pre>
 *
 * @see Notification#bigContentView
 */
CarClass(CNotificationInboxStyle)
    , public NotificationStyle
    , public INotificationInboxStyle
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CNotificationInboxStyle();

    virtual ~CNotificationInboxStyle();

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
     * Append a line to the digest section of the Inbox notification.
     */
    CARAPI AddLine(
        /* [in] */ ICharSequence* cs);

    CARAPI AddExtras(
        /* [in] */ IBundle* extras);

    CARAPI RestoreFromExtras(
        /* [in] */ IBundle* extras);

    /**
     * @hide
     */
    CARAPI PopulateBigContentView(
        /* [in] */ INotification* wip);

private:
    AutoPtr<IRemoteViews> MakeBigContentView();

private:
    AutoPtr<IArrayList> mTexts;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_CNOTIFICATIONINBOXSTYLE_H__
