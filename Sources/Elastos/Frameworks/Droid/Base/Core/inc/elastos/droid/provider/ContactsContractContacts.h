#ifndef __ELASTOS_DROID_PROVIDER_CONTACTSCONTRACTCONTACTS_H__
#define __ELASTOS_DROID_PROVIDER_CONTACTSCONTRACTCONTACTS_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Net::IUri;
using Elastos::IO::IInputStream;

namespace Elastos {
namespace Droid {
namespace Provider {

class ContactsContractContacts
{
public:
    /**
     * Builds a {@link #CONTENT_LOOKUP_URI} style {@link Uri} describing the
     * requested {@link Contacts} entry.
     *
     * @param contactUri A {@link #CONTENT_URI} row, or an existing
     *            {@link #CONTENT_LOOKUP_URI} to attempt refreshing.
     */
    static CARAPI GetLookupUri(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ IUri* contactUri,
        /* [out] */ IUri** uri);

    /**
     * Build a {@link #CONTENT_LOOKUP_URI} lookup {@link Uri} using the
     * given {@link ContactsContract.Contacts#_ID} and {@link #LOOKUP_KEY}.
     */
    static CARAPI GetLookupUri(
        /* [in] */ Int64 contactId,
        /* [in] */ const String& lookupKey,
        /* [out] */ IUri** uri);

    /**
     * Computes a content URI (see {@link #CONTENT_URI}) given a lookup URI.
     * <p>
     * Returns null if the contact cannot be found.
     */
    static CARAPI LookupContact(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ IUri* lookupUri,
        /* [out] */ IUri** uri);

    /**
     * Mark a contact as having been contacted. Updates two fields:
     * {@link #TIMES_CONTACTED} and {@link #LAST_TIME_CONTACTED}. The
     * TIMES_CONTACTED field is incremented by 1 and the LAST_TIME_CONTACTED
     * field is populated with the current system time.
     *
     * @param resolver the ContentResolver to use
     * @param contactId the person who was contacted
     *
     * @deprecated The class DataUsageStatUpdater of the Android support library should
     *     be used instead.
     */
    // @Deprecated
    static CARAPI MarkAsContacted(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ Int64 contactId);

    /**
     * Opens an InputStream for the contacts's photo and returns the
     * photo as a byte stream.
     * @param cr The content resolver to use for querying
     * @param contactUri the contact whose photo should be used. This can be used with
     * either a {@link #CONTENT_URI} or a {@link #CONTENT_LOOKUP_URI} URI.
     * @param preferHighres If this is true and the contact has a higher resolution photo
     * available, it is returned. If false, this function always tries to get the thumbnail
     * @return an InputStream of the photo, or null if no photo is present
     */
    static CARAPI OpenContactPhotoInputStream(
        /* [in] */ IContentResolver* cr,
        /* [in] */ IUri* contactUri,
        /* [in] */ Boolean preferHighres,
        /* [out] */ IInputStream** stream);

    /**
     * Opens an InputStream for the contacts's thumbnail photo and returns the
     * photo as a byte stream.
     * @param cr The content resolver to use for querying
     * @param contactUri the contact whose photo should be used. This can be used with
     * either a {@link #CONTENT_URI} or a {@link #CONTENT_LOOKUP_URI} URI.
     * @return an InputStream of the photo, or null if no photo is present
     * @see #openContactPhotoInputStream(ContentResolver, Uri, boolean), if instead
     * of the thumbnail the high-res picture is preferred
     */
    static CARAPI OpenContactPhotoInputStream(
        /* [in] */ IContentResolver* cr,
        /* [in] */ IUri* contactUri,
        /* [out] */ IInputStream** stream);

    static CARAPI IsEnterpriseContactId(
        /* [in] */ Int64 contactId,
        /* [out] */ Boolean* result);

public:
    static const AutoPtr<IUri> CONTENT_URI;
    static const AutoPtr<IUri> CORP_CONTENT_URI;
    static const AutoPtr<IUri> CONTENT_LOOKUP_URI;
    static const AutoPtr<IUri> CONTENT_VCARD_URI;
    static const AutoPtr<IUri> CONTENT_MULTI_VCARD_URI;
    static const AutoPtr<IUri> CONTENT_FILTER_URI;
    static const AutoPtr<IUri> CONTENT_STREQUENT_URI;
    static const AutoPtr<IUri> CONTENT_FREQUENT_URI;
    static const AutoPtr<IUri> CONTENT_STREQUENT_FILTER_URI;
    static const AutoPtr<IUri> CONTENT_GROUP_URI;
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CONTACTSCONTRACTCONTACTS_H__
