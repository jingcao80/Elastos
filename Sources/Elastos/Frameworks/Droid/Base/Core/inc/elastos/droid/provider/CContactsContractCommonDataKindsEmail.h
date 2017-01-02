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

#ifndef __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTCOMMONDATAKINDSEMAIL_H__
#define __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTCOMMONDATAKINDSEMAIL_H__

#include "_Elastos_Droid_Provider_CContactsContractCommonDataKindsEmail.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::ICharSequence;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Content::Res::IResources;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CContactsContractCommonDataKindsEmail)
    , public Singleton
    , public IContactsContractCommonDataKindsEmail
    , public IContactsContractDataColumnsWithJoins
    , public IContactsContractCommonDataKindsCommonColumns
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * The content:// style URI for all data records of the
     * {@link #CONTENT_ITEM_TYPE} MIME type, combined with the
     * associated raw contact and aggregate contact data.
     */
    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * <p>
     * The content:// style URL for looking up data rows by email address. The
     * lookup argument, an email address, should be passed as an additional path segment
     * after this URI.
     * </p>
     * <p>Example:
     * <pre>
     * Uri uri = Uri.withAppendedPath(Email.CONTENT_LOOKUP_URI, Uri.encode(email));
     * Cursor c = getContentResolver().query(uri,
     *          new String[]{Email.CONTACT_ID, Email.DISPLAY_NAME, Email.DATA},
     *          null, null, null);
     * </pre>
     * </p>
     */
    CARAPI GetCONTENT_LOOKUP_URI(
        /* [out] */ IUri** uri);

    /**
     * <p>
     * The content:// style URL for email lookup using a filter. The filter returns
     * records of MIME type {@link #CONTENT_ITEM_TYPE}. The filter is applied
     * to display names as well as email addresses. The filter argument should be passed
     * as an additional path segment after this URI.
     * </p>
     * <p>The query in the following example will return "Robert Parr (bob@incredibles.com)"
     * as well as "Bob Parr (incredible@android.com)".
     * <pre>
     * Uri uri = Uri.withAppendedPath(Email.CONTENT_LOOKUP_URI, Uri.encode("bob"));
     * Cursor c = getContentResolver().query(uri,
     *          new String[]{Email.DISPLAY_NAME, Email.DATA},
     *          null, null, null);
     * </pre>
     * </p>
     */
    CARAPI GetCONTENT_FILTER_URI(
        /* [out] */ IUri** uri);

    /**
     * Return the string resource that best describes the given
     * {@link #TYPE}. Will always return a valid resource.
     */
    CARAPI GetTypeLabelResource(
        /* [in] */ Int32 type,
        /* [out] */ Int32* resource);

    /**
     * Return a {@link CharSequence} that best describes the given type,
     * possibly substituting the given {@link #LABEL} value
     * for {@link #TYPE_CUSTOM}.
     */
    CARAPI GetTypeLabel(
        /* [in] */ IResources* res,
        /* [in] */ Int32 type,
        /* [in] */ ICharSequence* label,
        /* [out] */ ICharSequence** lb);
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTCOMMONDATAKINDSEMAIL_H__
