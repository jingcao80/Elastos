#ifndef __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTQUICKCONTACT_H__
#define __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTQUICKCONTACT_H__

#include "_Elastos_Droid_Provider_CContactsContractQuickContact.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CContactsContractQuickContact)
    , public Singleton
    , public IContactsContractQuickContact
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Constructs the QuickContacts intent with a view's rect.
     * @hide
     */
    CARAPI ComposeQuickContactsIntent(
        /* [in] */ IContext* context,
        /* [in] */ IView* target,
        /* [in] */ IUri* lookupUri,
        /* [in] */ Int32 mode,
        /* [in] */ ArrayOf<String>* excludeMimes,
        /* [out] */ IIntent** intent);

    /**
     * Constructs the QuickContacts intent.
     * @hide
     */
    CARAPI ComposeQuickContactsIntent(
        /* [in] */ IContext* context,
        /* [in] */ IRect* target,
        /* [in] */ IUri* lookupUri,
        /* [in] */ Int32 mode,
        /* [in] */ ArrayOf<String>* excludeMimes,
        /* [out] */ IIntent** intent);

    /**
     * Trigger a dialog that lists the various methods of interacting with
     * the requested {@link Contacts} entry. This may be based on available
     * {@link ContactsContract.Data} rows under that contact, and may also
     * include social status and presence details.
     *
     * @param context The parent {@link Context} that may be used as the
     *            parent for this dialog.
     * @param target Specific {@link View} from your layout that this dialog
     *            should be centered around. In particular, if the dialog
     *            has a "callout" arrow, it will be pointed and centered
     *            around this {@link View}.
     * @param lookupUri A {@link ContactsContract.Contacts#CONTENT_LOOKUP_URI} style
     *            {@link Uri} that describes a specific contact to feature
     *            in this dialog.
     * @param mode Any of {@link #MODE_SMALL}, {@link #MODE_MEDIUM}, or
     *            {@link #MODE_LARGE}, indicating the desired dialog size,
     *            when supported.
     * @param excludeMimes Optional list of {@link Data#MIMETYPE} MIME-types
     *            to exclude when showing this dialog. For example, when
     *            already viewing the contact details card, this can be used
     *            to omit the details entry from the dialog.
     */
    CARAPI ShowQuickContact(
        /* [in] */ IContext* context,
        /* [in] */ IView* target,
        /* [in] */ IUri* lookupUri,
        /* [in] */ Int32 mode,
        /* [in] */ ArrayOf<String>* excludeMimes);

    /**
     * Trigger a dialog that lists the various methods of interacting with
     * the requested {@link Contacts} entry. This may be based on available
     * {@link ContactsContract.Data} rows under that contact, and may also
     * include social status and presence details.
     *
     * @param context The parent {@link Context} that may be used as the
     *            parent for this dialog.
     * @param target Specific {@link Rect} that this dialog should be
     *            centered around, in screen coordinates. In particular, if
     *            the dialog has a "callout" arrow, it will be pointed and
     *            centered around this {@link Rect}. If you are running at a
     *            non-native density, you need to manually adjust using
     *            {@link DisplayMetrics#density} before calling.
     * @param lookupUri A
     *            {@link ContactsContract.Contacts#CONTENT_LOOKUP_URI} style
     *            {@link Uri} that describes a specific contact to feature
     *            in this dialog.
     * @param mode Any of {@link #MODE_SMALL}, {@link #MODE_MEDIUM}, or
     *            {@link #MODE_LARGE}, indicating the desired dialog size,
     *            when supported.
     * @param excludeMimes Optional list of {@link Data#MIMETYPE} MIME-types
     *            to exclude when showing this dialog. For example, when
     *            already viewing the contact details card, this can be used
     *            to omit the details entry from the dialog.
     */
    CARAPI ShowQuickContact(
        /* [in] */ IContext* context,
        /* [in] */ IRect* target,
        /* [in] */ IUri* lookupUri,
        /* [in] */ Int32 mode,
        /* [in] */ ArrayOf<String>* excludeMimes);

private:
    CARAPI_(void) StartActivityWithErrorToast(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTQUICKCONTACT_H__
