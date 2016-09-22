
#ifndef __ELASTOS_DROID_CONTACTS_COMMON_CONTACTTITLELOADERFACTORY_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_CONTACTTITLELOADERFACTORY_H__

#include "Elastos.Droid.Content.h"
#include <elastos/coredef.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::ICursorLoader;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {

/**
 * Used to create {@link CursorLoader}s to load different groups of
 * {@link com.android.contacts.list.ContactTileView}.
 */
class ContactTileLoaderFactory
{
public:
    static CARAPI_(AutoPtr<ICursorLoader>) CreateStrequentLoader(
        /* [in] */ IContext* context);

    static CARAPI_(AutoPtr<ICursorLoader>) CreateStrequentPhoneOnlyLoader(
        /* [in] */ IContext* context);

    static CARAPI_(AutoPtr<ICursorLoader>) CreateStarredLoader(
        /* [in] */ IContext* context);

    static CARAPI_(AutoPtr<ICursorLoader>) CreateFrequentLoader(
        /* [in] */ IContext* context);

public:
    static const Int32 CONTACT_ID = 0;
    static const Int32 DISPLAY_NAME = 1;
    static const Int32 STARRED = 2;
    static const Int32 PHOTO_URI = 3;
    static const Int32 LOOKUP_KEY = 4;
    static const Int32 CONTACT_PRESENCE = 5;
    static const Int32 CONTACT_STATUS = 6;

    // Only used for StrequentPhoneOnlyLoader
    static const Int32 PHONE_NUMBER = 5;
    static const Int32 PHONE_NUMBER_TYPE = 6;
    static const Int32 PHONE_NUMBER_LABEL = 7;
    static const Int32 IS_DEFAULT_NUMBER = 8;
    static const Int32 PINNED = 9;
    // The _ID field returned for strequent items actually contains data._id instead of
    // contacts._id because the query is performed on the data table. In order to obtain the
    // contact id for strequent items, we thus have to use Phone.contact_id instead.
    static const Int32 CONTACT_ID_FOR_DATA = 10;

    /**
     * Projection used for the {@link Contacts#CONTENT_STREQUENT_URI}
     * query when {@link ContactsContract#STREQUENT_PHONE_ONLY} flag
     * is set to true. The main difference is the lack of presence
     * and status data and the addition of phone number and label.
     */
    // @VisibleForTesting
    static const AutoPtr<ArrayOf<String> > COLUMNS_PHONE_ONLY;

private:
    static const AutoPtr<ArrayOf<String> > COLUMNS;

    static const String STARRED_ORDER = Contacts.DISPLAY_NAME+" COLLATE NOCASE ASC";
};

} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_CONTACTTITLELOADERFACTORY_H__
