
#ifndef __ELASTOS_DROID_CONTACTS_COMMON_CONTACTSUTILS_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_CONTACTSUTILS_H__

#include "Elastos.Droid.Content.h"
#include <elastos/utility/etl/Pair.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Etl::Pair;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {

class ContactsUtils
{
public:
    /**
     * This looks up the provider name defined in
     * ProviderNames from the predefined IM protocol id.
     * This is used for interacting with the IM application.
     *
     * @param protocol the protocol ID
     * @return the provider name the IM app uses for the given protocol, or null if no
     * provider is defined for the given protocol
     * @hide
     */
    static CARAPI_(String) LookupProviderNameFromId(
        /* [in] */ Int32 protocol);

    /**
     * Test if the given {@link CharSequence} contains any graphic characters,
     * first checking {@link TextUtils#isEmpty(CharSequence)} to handle null.
     */
    static CARAPI_(Boolean) IsGraphic(
        /* [in] */ ICharSequence* str);

    /**
     * Returns true if two objects are considered equal.  Two null references are equal here.
     */
    // @NeededForTesting
    static CARAPI_(Boolean) AreObjectsEqual(
        /* [in] */ IInterface* a,
        /* [in] */ IInterface* b);

    /**
     * Returns true if two {@link Intent}s are both null, or have the same action.
     */
    static CARAPI_(Boolean) AreIntentActionEqual(
        /* [in] */ IIntent* a,
        /* [in] */ IIntent* b);

    static CARAPI_(Boolean) AreContactWritableAccountsAvailable(
        /* [in] */ IContext* context);

    static CARAPI_(Boolean) AreGroupWritableAccountsAvailable(
        /* [in] */ IContext* context);

    /**
     * Returns the size (width and height) of thumbnail pictures as configured in the provider. This
     * can safely be called from the UI thread, as the provider can serve this without performing
     * a database access
     */
    static CARAPI_(Int32) GetThumbnailSize(
        /* [in] */ IContext* context);

    /**
     * Returns the proper Intent for an ImDatItem. If available, a secondary intent is stored
     * in the second Pair slot
     */
    static CARAPI_(AutoPtr<Pair<AutoPtr<IIntent>, AutoPtr<IIntent> > >) BuildImIntent(
        /* [in] */ IContext* context,
        /* [in] */ ImDataItem* im);

private:
    static CARAPI_(AutoPtr<IIntent>) GetCustomImIntent(
        /* [in] */ ImDataItem* im,
        /* [in] */ Int32 protocol);

public:
    // Telecomm related schemes are in CallUtil
    static const String SCHEME_IMTO;
    static const String SCHEME_MAILTO;
    static const String SCHEME_SMSTO;

private:
    static const String TAG;
    static Int32 sThumbnailSize;
};

} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_CONTACTSUTILS_H__
