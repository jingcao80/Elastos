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

#include "Elastos.Droid.Database.h"
#include "elastos/droid/content/CContentValues.h"
#include "elastos/droid/content/CEntity.h"
#include "elastos/droid/content/CursorEntityIterator.h"
#include "elastos/droid/database/DatabaseUtils.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/CContactsContractRawContacts.h"
#include "elastos/droid/provider/ContactsContract.h"
#include "elastos/droid/provider/ContactsContractContacts.h"
#include "elastos/droid/provider/ContactsContractData.h"
#include <Elastos.CoreLibrary.IO.h>

using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::CEntity;
using Elastos::Droid::Content::CursorEntityIterator;
using Elastos::Droid::Content::EIID_ICursorEntityIterator;
using Elastos::Droid::Content::EIID_IEntityIterator;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::ICursorEntityIterator;
using Elastos::Droid::Database::DatabaseUtils;
using Elastos::Droid::Net::Uri;
using Elastos::Utility::EIID_IIterator;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::IO::ICloseable;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CContactsContractRawContacts)

CAR_INTERFACE_IMPL_6(CContactsContractRawContacts, Singleton
    , IContactsContractRawContacts
    , IBaseColumns
    , IContactsContractRawContactsColumns
    , IContactsContractContactOptionsColumns
    , IContactsContractContactNameColumns
    , IContactsContractSyncColumns)

ECode CContactsContractRawContacts::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return Uri::WithAppendedPath(ContactsContract::AUTHORITY_URI.Get(), String("raw_contacts"), uri);
}

ECode CContactsContractRawContacts::GetContactLookupUri(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IUri* rawContactUri,
    /* [out] */ IUri** uri)
{
    // TODO: use a lighter query by joining rawcontacts with contacts in provider
    AutoPtr<IUri> dataUri;
    FAIL_RETURN(Uri::WithAppendedPath(rawContactUri, IContactsContractContactsData::CONTENT_DIRECTORY, (IUri**)&dataUri))
    AutoPtr<ICursor> cursor;
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(2);
    (*args)[0] = IContactsContractRawContactsColumns::CONTACT_ID;
    (*args)[1] = IContactsContractContactsColumns::LOOKUP_KEY;
    FAIL_RETURN(resolver->Query(dataUri, args, String(NULL), NULL, String(NULL), (ICursor**)&cursor))

    //try {
    Boolean result;
    if (cursor != NULL && (cursor->MoveToFirst(&result), result)) {
        Int64 contactId;
        FAIL_GOTO(cursor->GetInt64(0, &contactId), EXIT)
        String lookupKey;
        FAIL_GOTO(cursor->GetString(1, &lookupKey), EXIT)
        FAIL_GOTO(ContactsContractContacts::GetLookupUri(contactId, lookupKey, uri), EXIT)
        ICloseable::Probe(cursor)->Close();
        return NOERROR;
    }
    //} finally {
EXIT:
    if (cursor != NULL) {
        FAIL_RETURN(ICloseable::Probe(cursor)->Close())
    }
    //}
    *uri = NULL;
    return NOERROR;
}

ECode CContactsContractRawContacts::NewEntityIterator(
    /* [in] */ ICursor* cursor,
    /* [out] */ IEntityIterator** iterator)
{
    VALIDATE_NOT_NULL(iterator);

    AutoPtr<EntityIteratorImpl> impl = new EntityIteratorImpl();
    impl->constructor(cursor);

    *iterator = (IEntityIterator*)impl.Get();
    REFCOUNT_ADD(*iterator);
    return NOERROR;
}

static AutoPtr<ArrayOf<String> > initDATAKEYS()
{
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(19);
    (*args)[0] = IContactsContractDataColumns::DATA1;
    (*args)[1] = IContactsContractDataColumns::DATA2;
    (*args)[2] = IContactsContractDataColumns::DATA3;
    (*args)[3] = IContactsContractDataColumns::DATA4;
    (*args)[4] = IContactsContractDataColumns::DATA5;
    (*args)[5] = IContactsContractDataColumns::DATA6;
    (*args)[6] = IContactsContractDataColumns::DATA7;
    (*args)[7] = IContactsContractDataColumns::DATA8;
    (*args)[8] = IContactsContractDataColumns::DATA9;
    (*args)[9] = IContactsContractDataColumns::DATA10;
    (*args)[10] = IContactsContractDataColumns::DATA11;
    (*args)[11] = IContactsContractDataColumns::DATA12;
    (*args)[12] = IContactsContractDataColumns::DATA13;
    (*args)[13] = IContactsContractDataColumns::DATA14;
    (*args)[14] = IContactsContractDataColumns::DATA15;
    (*args)[15] = IContactsContractDataColumns::SYNC1;
    (*args)[16] = IContactsContractDataColumns::SYNC2;
    (*args)[17] = IContactsContractDataColumns::SYNC3;
    (*args)[18] = IContactsContractDataColumns::SYNC4;
    return args;
}

const AutoPtr<ArrayOf<String> > CContactsContractRawContacts::EntityIteratorImpl::DATA_KEYS = initDATAKEYS();

CAR_INTERFACE_IMPL_3(CContactsContractRawContacts::EntityIteratorImpl, Object, ICursorEntityIterator, IEntityIterator, IIterator)

CContactsContractRawContacts::EntityIteratorImpl::EntityIteratorImpl()
{}

CContactsContractRawContacts::EntityIteratorImpl::~EntityIteratorImpl()
{}

ECode CContactsContractRawContacts::EntityIteratorImpl::constructor(
    /* [in] */ ICursor* cursor)
{
    return CursorEntityIterator::constructor(cursor);
}

ECode CContactsContractRawContacts::EntityIteratorImpl::GetEntityAndIncrementCursor(
    /* [in] */ ICursor* cursor,
    /* [out] */ IEntity** entity)
{
    VALIDATE_NOT_NULL(entity);

    Int32 columnRawContactId;
    FAIL_RETURN(cursor->GetColumnIndexOrThrow(IBaseColumns::ID, &columnRawContactId))
    Int64 rawContactId;
    FAIL_RETURN(cursor->GetInt64(columnRawContactId, &rawContactId))

    // we expect the cursor is already at the row we need to read from
    AutoPtr<IContentValues> cv;
    FAIL_RETURN(CContentValues::New((IContentValues**)&cv))
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, ACCOUNT_NAME);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, ACCOUNT_TYPE);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, DATA_SET);
    DatabaseUtils::CursorInt64ToContentValuesIfPresent(cursor, cv, IBaseColumns::ID);
    DatabaseUtils::CursorInt64ToContentValuesIfPresent(cursor, cv, DIRTY);
    DatabaseUtils::CursorInt64ToContentValuesIfPresent(cursor, cv, VERSION);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, SOURCE_ID);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, IContactsContractBaseSyncColumns::SYNC1);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, IContactsContractBaseSyncColumns::SYNC2);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, IContactsContractBaseSyncColumns::SYNC3);
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, IContactsContractBaseSyncColumns::SYNC4);
    DatabaseUtils::CursorInt64ToContentValuesIfPresent(cursor, cv, DELETED);
    DatabaseUtils::CursorInt64ToContentValuesIfPresent(cursor, cv, CONTACT_ID);
    DatabaseUtils::CursorInt64ToContentValuesIfPresent(cursor, cv, STARRED);
    DatabaseUtils::CursorInt32ToContentValuesIfPresent(cursor, cv, NAME_VERIFIED);
    AutoPtr<IEntity> contact;
    FAIL_RETURN(CEntity::New(cv, (IEntity**)&contact))

    // read data rows until the contact id changes
    Boolean result;
    do {
        Int64 id;
        FAIL_RETURN(cursor->GetInt64(columnRawContactId, &id))
        if (rawContactId != id) {
            break;
        }
        // add the data to to the contact
        FAIL_RETURN(CContentValues::New((IContentValues**)&cv))
        Int32 columnIndex;
        FAIL_RETURN(cursor->GetColumnIndexOrThrow(IContactsContractContactsEntity::DATA_ID, &columnIndex))
        Int64 columnValue;
        FAIL_RETURN(cursor->GetInt64(columnIndex, &columnValue))
        FAIL_RETURN(cv->Put(IBaseColumns::ID, columnValue))
        DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, IContactsContractDataColumns::RES_PACKAGE);
        DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, IContactsContractDataColumns::MIMETYPE);
        DatabaseUtils::CursorInt64ToContentValuesIfPresent(cursor, cv, IContactsContractDataColumns::IS_PRIMARY);
        DatabaseUtils::CursorInt64ToContentValuesIfPresent(cursor, cv, IContactsContractDataColumns::IS_SUPER_PRIMARY);
        DatabaseUtils::CursorInt64ToContentValuesIfPresent(cursor, cv, IContactsContractDataColumns::DATA_VERSION);
        DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, IContactsContractCommonDataKindsGroupMembership::GROUP_SOURCE_ID);
        DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, cv, IContactsContractDataColumns::DATA_VERSION);
        for (Int32 i = 0; i < DATA_KEYS->GetLength(); i++) {
            String key = (*DATA_KEYS)[i];
            Int32 columnIndex;
            FAIL_RETURN(cursor->GetColumnIndexOrThrow(key, &columnIndex))
            Int32 type;
            FAIL_RETURN(cursor->GetType(columnIndex, &type))
            String str;
            AutoPtr<ICharSequence> cstr;
            AutoPtr<ArrayOf<Byte> > blob;
            switch (type) {
                case ICursor::FIELD_TYPE_NULL:
                    // don't put anything
                    break;
                case ICursor::FIELD_TYPE_INTEGER:
                case ICursor::FIELD_TYPE_FLOAT:
                case ICursor::FIELD_TYPE_STRING:
                    FAIL_RETURN(cursor->GetString(columnIndex, &str))
                    FAIL_RETURN(cv->Put(key, str))
                    break;
                case ICursor::FIELD_TYPE_BLOB:
                    FAIL_RETURN(cursor->GetBlob(columnIndex, (ArrayOf<Byte>**)&blob))
                    FAIL_RETURN(cv->Put(key, blob))
                    break;
                default:
                    //throw new IllegalStateException("Invalid or unhandled data type");
                    return E_ILLEGAL_STATE_EXCEPTION;
            }
        }
        AutoPtr<IUri> _uri;
        ContactsContractData::GetCONTENT_URI((IUri**)&_uri);
        FAIL_RETURN(contact->AddSubValue(_uri, cv))
    } while ((cursor->MoveToNext(&result), result));

    return NOERROR;
}

} //Provider
} //Droid
} //Elastos