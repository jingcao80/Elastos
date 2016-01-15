#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_NOTIFICATIONDATA_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_NOTIFICATIONDATA_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::StatusBar::IStatusBarNotification;
using Elastos::Droid::SystemUI::StatusBar::IStatusBarIconView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

class NotificationDataEntry : public ElRefBase
{
public:
    NotificationDataEntry();

    NotificationDataEntry(
        /* [in] */ IBinder* key,
        /* [in] */ IStatusBarNotification* n,
        /* [in] */ IStatusBarIconView* ic);

    CARAPI_(void) SetLargeView(
        /* [in] */ IView* expandedLarge);

    CARAPI_(AutoPtr<IView>) GetLargeView();

    /**
     * Return whether the entry can be expanded.
     */
    CARAPI_(Boolean) Expandable();

    /**
     * Return whether the entry has been manually expanded by the user.
     */
    CARAPI_(Boolean) UserExpanded();

    /**
     * Set the flag indicating that this was manually expanded by the user.
     */
    CARAPI_(Boolean) SetUserExpanded(
        /* [in] */ Boolean userExpanded);

    /**
     * Return whether the entry is being touched by the user.
     */
    CARAPI_(Boolean) UserLocked();

    /**
     * Set the flag indicating that this is being touched by the user.
     */
    CARAPI_(Boolean) SetUserLocked(
        /* [in] */ Boolean userLocked);

    static CARAPI_(Int32) Compare(
        /* [in] */ const NotificationDataEntry* lhs,
        /* [in] */ const NotificationDataEntry* rhs);

public:
    AutoPtr<IBinder> mKey;
    AutoPtr<IStatusBarNotification> mNotification;
    AutoPtr<IStatusBarIconView> mIcon;
    AutoPtr<IView> mRow; // the outer expanded view
    AutoPtr<IView> mContent; // takes the click events and sends the PendingIntent
    AutoPtr<IView> mExpanded; // the inflated RemoteViews
    AutoPtr<IImageView> mLargeIcon;
};


class NotificationData : public ElRefBase
{
public:
    CARAPI_(Int32) Size();

    CARAPI_(AutoPtr<NotificationDataEntry>) Get(
        /* [in] */ Int32 i);

    CARAPI_(List<AutoPtr<NotificationDataEntry> >::Iterator) FindIteratorByKey(
        /* [in] */ IBinder* key);

    CARAPI_(AutoPtr<NotificationDataEntry>) FindByKey(
        /* [in] */ IBinder* key);

    CARAPI_(Int32) Add(
        /* [in] */ NotificationDataEntry* entry);

    CARAPI_(Int32) Add(
        /* [in] */ IBinder* key,
        /* [in] */ IStatusBarNotification* notification,
        /* [in] */ IView* row,
        /* [in] */ IView* content,
        /* [in] */ IView* expanded,
        /* [in] */ IStatusBarIconView* icon);

    CARAPI_(AutoPtr<NotificationDataEntry>) Remove(
        /* [in] */ IBinder* key);

    /**
     * Return whether there are any visible items (i.e. items without an error).
     */
    CARAPI_(Boolean) HasVisibleItems();

    /**
     * Return whether there are any clearable items (that aren't errors).
     */
    CARAPI_(Boolean) HasClearableItems();

    /**
     * Return whether the entry can be expanded.
     */
    static CARAPI_(Boolean) GetIsExpandable(
        /* [in] */ IView* row);

    /**
     * Return whether the entry has been manually expanded by the user.
     */
    static CARAPI_(Boolean) GetUserExpanded(
        /* [in] */ IView* row);

    /**
     * Set whether the entry has been manually expanded by the user.
     */
    static CARAPI_(Boolean) SetUserExpanded(
        /* [in] */ IView* row,
        /* [in] */ Boolean userExpanded);

    /**
     * Return whether the entry is being touched by the user.
     */
    static CARAPI_(Boolean) GetUserLocked(
        /* [in] */ IView* row);

    /**
     * Set whether the entry is being touched by the user.
     */
    static CARAPI_(Boolean) SetUserLocked(
        /* [in] */ IView* row,
        /* [in] */ Boolean userLocked);

protected:
    static CARAPI_(Boolean) ReadBooleanTag(
        /* [in] */ IView* view,
        /* [in] */ Int32 id);

    static CARAPI_(Boolean) WriteBooleanTag(
        /* [in] */ IView* view,
        /* [in] */ Int32 id,
        /* [in] */ Boolean value);

private:
    friend class NotificationDataEntry;
    List<AutoPtr<NotificationDataEntry> > mEntries;
};


}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_NOTIFICATIONDATA_H__
