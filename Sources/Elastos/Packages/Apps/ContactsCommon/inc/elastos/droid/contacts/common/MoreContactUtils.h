#ifndef __ELASTOS_APPS_CONTACTS_COMMON_MORECONTACTUTILS_H__
#define __ELASTOS_APPS_CONTACTS_COMMON_MORECONTACTUTILS_H__

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {

/**
 * Shared static contact utility methods.
 */
class MoreContactUtils
    : public Object
{
public:
    /**
     * Returns true if two data with mimetypes which represent values in contact entries are
     * considered equal for collapsing in the GUI. For caller-id, use
     * {@link android.telephony.PhoneNumberUtils#compare(android.content.Context, String, String)}
     * instead
     */
    static CARAPI_(Boolean) ShouldCollapse(
        /* [in] */ ICharSequence* mimetype1,
        /* [in] */ ICharSequence* data1,
        /* [in] */ ICharSequence* mimetype2,
        /* [in] */ ICharSequence* data2);

    /**
     * Returns the {@link android.graphics.Rect} with left, top, right, and bottom coordinates
     * that are equivalent to the given {@link android.view.View}'s bounds. This is equivalent to
     * how the target {@link android.graphics.Rect} is calculated in
     * {@link android.provider.ContactsContract.QuickContact#showQuickContact}.
     */
    static CARAPI_(AutoPtr<IRect>) GetTargetRectFromView(
        /* [in] */ IView* view);

    /**
     * Returns a header view based on the R.layout.list_separator, where the
     * containing {@link android.widget.TextView} is set using the given textResourceId.
     */
    static CARAPI_(AutoPtr<ITextView>) CreateHeaderView(
        /* [in] */ IContext* context,
        /* [in] */ Int32 textResourceId);

    /**
     * Set the top padding on the header view dynamically, based on whether the header is in
     * the first row or not.
     */
    static CARAPI_(void) SetHeaderViewBottomPadding(
        /* [in] */ IContext* context,
        /* [in] */ ITextView* textView,
        /* [in] */ Boolean isFirstRow);

    /**
     * Returns the intent to launch for the given invitable account type and contact lookup URI.
     * This will return null if the account type is not invitable (i.e. there is no
     * {@link AccountType#getInviteContactActivityClassName()} or
     * {@link AccountType#syncAdapterPackageName}).
     */
    static CARAPI_(AutoPtr<IIntent>) GetInvitableIntent(
        /* [in] */ IAccountType* accountType,
        /* [in] */ IUri* lookupUri);

private:
    // TODO: Move this to PhoneDataItem.shouldCollapse override
    static CARAPI_(Boolean) ShouldCollapsePhoneNumbers(
        /* [in] */ const String& number1,
        /* [in] */ const String& number2);

private:
    static const String WAIT_SYMBOL_AS_STRING; // = String.valueOf(PhoneNumberUtils.WAIT);
};

} // Common
} // Contacts
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_CONTACTS_COMMON_MORECONTACTUTILS_H__
