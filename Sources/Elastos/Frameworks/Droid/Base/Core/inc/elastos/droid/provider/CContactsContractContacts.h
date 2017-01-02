//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTCONTACTS_H__
#define __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTCONTACTS_H__

#include "_Elastos_Droid_Provider_CContactsContractContacts.h"
#include "elastos/droid/provider/ContactsContractContacts.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CContactsContractContacts)
    , public Singleton
    , IContactsContractContacts
    , IBaseColumns
    , IContactsContractContactsColumns
    , IContactsContractContactOptionsColumns
    , IContactsContractContactNameColumns
    , IContactsContractContactStatusColumns
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    CARAPI GetCONTENT_LOOKUP_URI(
        /* [out] */ IUri** uri);

    CARAPI GetCONTENT_VCARD_URI(
        /* [out] */ IUri** uri);

    CARAPI GetCONTENT_MULTI_VCARD_URI(
        /* [out] */ IUri** uri);

    CARAPI GetCONTENT_FILTER_URI(
        /* [out] */ IUri** uri);

    CARAPI GetCONTENT_STREQUENT_URI(
        /* [out] */ IUri** uri);

    CARAPI GetCONTENT_FREQUENT_URI(
        /* [out] */ IUri** uri);

    CARAPI GetCONTENT_STREQUENT_FILTER_URI(
        /* [out] */ IUri** uri);

    CARAPI GetCONTENT_GROUP_URI(
        /* [out] */ IUri** uri);

    CARAPI GetCORP_CONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * Builds a {@link #CONTENT_LOOKUP_URI} style {@link Uri} describing the
     * requested {@link Contacts} entry.
     *
     * @param contactUri A {@link #CONTENT_URI} row, or an existing
     *            {@link #CONTENT_LOOKUP_URI} to attempt refreshing.
     */
    CARAPI GetLookupUri(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ IUri* contactUri,
        /* [out] */ IUri** uri);

    /**
     * Build a {@link #CONTENT_LOOKUP_URI} lookup {@link Uri} using the
     * given {@link ContactsContract.Contacts#_ID} and {@link #LOOKUP_KEY}.
     */
    CARAPI GetLookupUri(
        /* [in] */ Int64 contactId,
        /* [in] */ const String& lookupKey,
        /* [out] */ IUri** uri);

    /**
     * Computes a content URI (see {@link #CONTENT_URI}) given a lookup URI.
     * <p>
     * Returns null if the contact cannot be found.
     */
    CARAPI LookupContact(
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
    CARAPI MarkAsContacted(
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
    CARAPI OpenContactPhotoInputStream(
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
    CARAPI OpenContactPhotoInputStream(
        /* [in] */ IContentResolver* cr,
        /* [in] */ IUri* contactUri,
        /* [out] */ IInputStream** stream);

    CARAPI IsEnterpriseContactId(
        /* [in] */ Int64 contactId,
        /* [out] */ Boolean* result);
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTCONTACTS_H__
