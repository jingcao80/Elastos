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

#ifndef __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTRAWCONTACTS_H__
#define __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTRAWCONTACTS_H__

#include "_Elastos_Droid_Provider_CContactsContractRawContacts.h"
#include "elastos/droid/content/CursorEntityIterator.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/net/Uri.h"
#include <elastos/core/Singleton.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Content::CursorEntityIterator;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::ICursorEntityIterator;
using Elastos::Droid::Content::IEntity;
using Elastos::Droid::Content::IEntityIterator;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CContactsContractRawContacts)
    , public Singleton
    , public IContactsContractRawContacts
    , public IBaseColumns
    , public IContactsContractRawContactsColumns
    , public IContactsContractContactOptionsColumns
    , public IContactsContractContactNameColumns
    , public IContactsContractSyncColumns
{
private:
    class EntityIteratorImpl
        : public CursorEntityIterator
    {
    public:
        CAR_INTERFACE_DECL()

        EntityIteratorImpl();

        ~EntityIteratorImpl();

        CARAPI constructor(
            /* [in] */ ICursor* cursor);

        // @Override
        CARAPI GetEntityAndIncrementCursor(
            /* [in] */ ICursor* cursor,
            /* [out] */ IEntity** entity);

    private:
        static const AutoPtr<ArrayOf<String> > DATA_KEYS;
    };

public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * The content:// style URI for this table, which requests a directory of
     * raw contact rows matching the selection criteria.
     */
    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * Build a {@link android.provider.ContactsContract.Contacts#CONTENT_LOOKUP_URI}
     * style {@link Uri} for the parent {@link android.provider.ContactsContract.Contacts}
     * entry of the given {@link RawContacts} entry.
     */
    CARAPI GetContactLookupUri(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ IUri* rawContactUri,
        /* [out] */ IUri** uri);

    /**
     * TODO: javadoc
     * @param cursor
     * @return
     */
    CARAPI NewEntityIterator(
        /* [in] */ ICursor* cursor,
        /* [out] */ IEntityIterator** iterator);
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTRAWCONTACTS_H__
