#ifndef __ELASTOS_APPS_CONTACTS_COMMON_CMORECONTACTUTILS_H__
#define __ELASTOS_APPS_CONTACTS_COMMON_CMORECONTACTUTILS_H__

#include "_Elastos_Apps_Contacts_Common_CMoreContactUtils.h"
#include <elastos/core/Singleton.h>

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {

CarClass(CMoreContactUtils)
    , public Singleton
    , public IMoreContactUtils
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Returns true if two data with mimetypes which represent values in contact entries are
     * considered equal for collapsing in the GUI. For caller-id, use
     * {@link android.telephony.PhoneNumberUtils#compare(android.content.Context, String, String)}
     * instead
     */
    CARAPI ShouldCollapse(
        /* [in] */ ICharSequence* mimetype1,
        /* [in] */ ICharSequence* data1,
        /* [in] */ ICharSequence* mimetype2,
        /* [in] */ ICharSequence* data2,
        /* [out] */ Boolean* result);

    /**
     * Returns the {@link android.graphics.Rect} with left, top, right, and bottom coordinates
     * that are equivalent to the given {@link android.view.View}'s bounds. This is equivalent to
     * how the target {@link android.graphics.Rect} is calculated in
     * {@link android.provider.ContactsContract.QuickContact#showQuickContact}.
     */
    CARAPI GetTargetRectFromView(
        /* [in] */ IView* view,
        /* [out] */ IRect** rect);

    /**
     * Returns a header view based on the R.layout.list_separator, where the
     * containing {@link android.widget.TextView} is set using the given textResourceId.
     */
    CARAPI CreateHeaderView(
        /* [in] */ IContext* context,
        /* [in] */ Int32 textResourceId,
        /* [out] */ ITextView** textView);

    /**
     * Set the top padding on the header view dynamically, based on whether the header is in
     * the first row or not.
     */
    CARAPI SetHeaderViewBottomPadding(
        /* [in] */ IContext* context,
        /* [in] */ ITextView* textView,
        /* [in] */ Boolean isFirstRow);

    /**
     * Returns the intent to launch for the given invitable account type and contact lookup URI.
     * This will return null if the account type is not invitable (i.e. there is no
     * {@link AccountType#getInviteContactActivityClassName()} or
     * {@link AccountType#syncAdapterPackageName}).
     */
    CARAPI GetInvitableIntent(
        /* [in] */ IAccountType* accountType,
        /* [in] */ IUri* lookupUri,
        /* [out] */ IIntent** intent);
};

} // Common
} // Contacts
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_CONTACTS_COMMON_CMORECONTACTUTILS_H__
