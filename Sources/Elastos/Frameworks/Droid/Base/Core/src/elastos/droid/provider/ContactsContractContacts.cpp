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

#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Database.h"
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/content/CContentUris.h"
#include "elastos/droid/content/CContentValues.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/ContactsContract.h"
#include "elastos/droid/provider/ContactsContractContacts.h"
#include "elastos/droid/provider/ContactsContractProfile.h"

using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::Res::IAssetFileDescriptor;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Provider::IBaseColumns;
using Elastos::IO::CByteArrayInputStream;
using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::IFileInputStream;
using Elastos::IO::ICloseable;
using Elastos::Core::CInteger64;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::Core::IInteger64;

namespace Elastos {
namespace Droid {
namespace Provider {

static AutoPtr<IUri> initCONTENT_URI()
{
    AutoPtr<IUri> uri;
    Uri::WithAppendedPath(ContactsContract::AUTHORITY_URI.Get(), String("contacts"), (IUri**)&uri);
    return uri;
}
const AutoPtr<IUri> ContactsContractContacts::CONTENT_URI = initCONTENT_URI();

static AutoPtr<IUri> initCORP_CONTENT_URI()
{
    AutoPtr<IUri> uri;
    Uri::WithAppendedPath(ContactsContract::AUTHORITY_URI.Get(), String("contacts_corp"), (IUri**)&uri);
    return uri;
}
const AutoPtr<IUri> ContactsContractContacts::CORP_CONTENT_URI = initCORP_CONTENT_URI();

static AutoPtr<IUri> initCONTENT_LOOKUP_URI()
{
    AutoPtr<IUri> uri;
    Uri::WithAppendedPath(ContactsContractContacts::CONTENT_URI.Get(), String("lookup"), (IUri**)&uri);
    return uri;
}
const AutoPtr<IUri> ContactsContractContacts::CONTENT_LOOKUP_URI = initCONTENT_LOOKUP_URI();


static AutoPtr<IUri> initCONTENT_VCARD_URI()
{
    AutoPtr<IUri> uri;
    Uri::WithAppendedPath(ContactsContractContacts::CONTENT_URI.Get(), String("as_vcard"), (IUri**)&uri);
    return uri;
}
const AutoPtr<IUri> ContactsContractContacts::CONTENT_VCARD_URI = initCONTENT_VCARD_URI();

static AutoPtr<IUri> initCONTENT_MULTI_VCARD_URI()
{
    AutoPtr<IUri> uri;
    Uri::WithAppendedPath(ContactsContractContacts::CONTENT_URI.Get(), String("as_multi_vcard"), (IUri**)&uri);
    return uri;
}
const AutoPtr<IUri> ContactsContractContacts::CONTENT_MULTI_VCARD_URI = initCONTENT_MULTI_VCARD_URI();

static AutoPtr<IUri> initCONTENT_FILTER_URI()
{
    AutoPtr<IUri> uri;
    Uri::WithAppendedPath(ContactsContractContacts::CONTENT_URI.Get(), String("filter"), (IUri**)&uri);
    return uri;
}
const AutoPtr<IUri> ContactsContractContacts::CONTENT_FILTER_URI = initCONTENT_FILTER_URI();

static AutoPtr<IUri> initCONTENT_STREQUENT_URI()
{
    AutoPtr<IUri> uri;
    Uri::WithAppendedPath(ContactsContractContacts::CONTENT_URI.Get(), String("strequent"), (IUri**)&uri);
    return uri;
}
const AutoPtr<IUri> ContactsContractContacts::CONTENT_STREQUENT_URI = initCONTENT_STREQUENT_URI();


static AutoPtr<IUri> initCONTENT_FREQUENT_URI()
{
    AutoPtr<IUri> uri;
    Uri::WithAppendedPath(ContactsContractContacts::CONTENT_URI.Get(), String("frequent"), (IUri**)&uri);
    return uri;
}
const AutoPtr<IUri> ContactsContractContacts::CONTENT_FREQUENT_URI = initCONTENT_FREQUENT_URI();


static AutoPtr<IUri> initCONTENT_STREQUENT_FILTER_URI()
{
    AutoPtr<IUri> uri;
    Uri::WithAppendedPath(ContactsContractContacts::CONTENT_STREQUENT_URI.Get(), String("filter"), (IUri**)&uri);
    return uri;
}
const AutoPtr<IUri> ContactsContractContacts::CONTENT_STREQUENT_FILTER_URI = initCONTENT_STREQUENT_FILTER_URI();


static AutoPtr<IUri> initCONTENT_GROUP_URI()
{
    AutoPtr<IUri> uri;
    Uri::WithAppendedPath(ContactsContractContacts::CONTENT_URI.Get(), String("group"), (IUri**)&uri);
    return uri;
}
const AutoPtr<IUri> ContactsContractContacts::CONTENT_GROUP_URI = initCONTENT_GROUP_URI();

ECode ContactsContractContacts::GetLookupUri(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IUri* contactUri,
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    AutoPtr<ICursor> c;
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(2);
    (*args)[0] = IContactsContractContactsColumns::LOOKUP_KEY;
    (*args)[1] = IBaseColumns::ID;
    FAIL_RETURN(resolver->Query(contactUri, args, String(NULL), NULL, String(NULL), (ICursor**)&c))
    if (c == NULL) {
        *uri = NULL;
        return NOERROR;
    }

    //try {
    Boolean result;
    FAIL_GOTO(c->MoveToFirst(&result), EXIT)
    if (result) {
        String lookupKey;
        FAIL_GOTO(c->GetString(0, &lookupKey), EXIT)
        Int64 contactId;
        FAIL_GOTO(c->GetInt64(1, &contactId), EXIT)
        FAIL_GOTO(GetLookupUri(contactId, lookupKey, uri), EXIT)
        return ICloseable::Probe(c)->Close();
    }
    //} finally {
EXIT:
    FAIL_RETURN(ICloseable::Probe(c)->Close())
    //}
    *uri = NULL;
    return NOERROR;
}

ECode ContactsContractContacts::GetLookupUri(
    /* [in] */ Int64 contactId,
    /* [in] */ const String& lookupKey,
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    AutoPtr<IUri> _uri;
    FAIL_RETURN(Uri::WithAppendedPath(ContactsContractContacts::CONTENT_LOOKUP_URI.Get(), lookupKey, (IUri**)&_uri))
    AutoPtr<IContentUris> contentUris;
    CContentUris::AcquireSingleton((IContentUris**)&contentUris);
    return contentUris->WithAppendedId(_uri, contactId, uri);
}

ECode ContactsContractContacts::LookupContact(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IUri* lookupUri,
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    if (lookupUri == NULL) {
        *uri = NULL;
        return NOERROR;
    }

    AutoPtr<ICursor> c;
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(1);
    (*args)[0] = IBaseColumns::ID;
    FAIL_RETURN(resolver->Query(lookupUri, args, String(NULL), NULL, String(NULL), (ICursor**)&c))
    if (c == NULL) {
        *uri = NULL;
        return NOERROR;
    }

    //try {
    Boolean result;
    FAIL_GOTO(c->MoveToFirst(&result), EXIT)
    if (result) {
        Int64 contactId;
        FAIL_GOTO(c->GetInt64(0, &contactId), EXIT)
        AutoPtr<IContentUris> contentUris;
        CContentUris::AcquireSingleton((IContentUris**)&contentUris);
        FAIL_GOTO(contentUris->WithAppendedId(ContactsContractContacts::CONTENT_URI.Get(), contactId, uri), EXIT)
        ICloseable::Probe(c)->Close();
        return NOERROR;
    }
    //} finally {
EXIT:
    FAIL_RETURN(ICloseable::Probe(c)->Close())
    //}
    *uri = NULL;
    return NOERROR;
}

ECode ContactsContractContacts::MarkAsContacted(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ Int64 contactId)
{
    AutoPtr<IUri> uri;
    AutoPtr<IContentUris> contentUris;
    FAIL_RETURN(CContentUris::AcquireSingleton((IContentUris**)&contentUris))
    FAIL_RETURN(contentUris->WithAppendedId(ContactsContractContacts::CONTENT_URI.Get(), contactId, (IUri**)&uri))
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);

    // TIMES_CONTACTED will be incremented when LAST_TIME_CONTACTED is modified.
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 now;
    system->GetCurrentTimeMillis(&now);
    FAIL_RETURN(values->Put(IContactsContractContactOptionsColumns::LAST_TIME_CONTACTED, now))
    Int32 result;
    return resolver->Update(uri, values, String(NULL), NULL, &result);
}

ECode ContactsContractContacts::OpenContactPhotoInputStream(
    /* [in] */ IContentResolver* cr,
    /* [in] */ IUri* contactUri,
    /* [in] */ Boolean preferHighres,
    /* [out] */ IInputStream** stream)
{
    VALIDATE_NOT_NULL(stream);

    if (preferHighres) {
        AutoPtr<IUri> displayPhotoUri;
        FAIL_RETURN(Uri::WithAppendedPath(contactUri, IContactsContractContactsPhoto::DISPLAY_PHOTO, (IUri**)&displayPhotoUri))
        AutoPtr<IInputStream> inputStream;
        //try {
            AutoPtr<IAssetFileDescriptor> fd;
            FAIL_RETURN(cr->OpenAssetFileDescriptor(displayPhotoUri, String("r"), (IAssetFileDescriptor**)&fd))
            AutoPtr<IFileInputStream> fstream;
            FAIL_RETURN(fd->CreateInputStream((IFileInputStream**)&fstream))
            *stream = IInputStream::Probe(fstream);
            REFCOUNT_ADD(*stream);
            return NOERROR;
        //} catch (IOException e) {
            // fallback to the thumbnail code
        //}
    }

    AutoPtr<IUri> photoUri;
    FAIL_RETURN(Uri::WithAppendedPath(contactUri, IContactsContractContactsPhoto::CONTENT_DIRECTORY, (IUri**)&photoUri))
    if (photoUri == NULL) {
        *stream = NULL;
        return NOERROR;
    }
    AutoPtr<ICursor> cursor;
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(1);
    (*args)[0] = IContactsContractCommonDataKindsPhoto::PHOTO;
    FAIL_RETURN(cr->Query(photoUri, args, String(NULL), NULL, String(NULL), (ICursor**)&cursor))

    //try {
    Boolean result;
    AutoPtr<ArrayOf<Byte> > data;
    AutoPtr<IByteArrayInputStream> _stream;
    if (cursor == NULL || (cursor->MoveToNext(&result), !result)) {
        *stream = NULL;
        goto FAIL_FINALLY;
    }
    FAIL_GOTO(cursor->GetBlob(0, (ArrayOf<Byte>**)&data), FAIL_FINALLY)
    if (data == NULL) {
        *stream = NULL;
        goto FAIL_FINALLY;
    }
    FAIL_GOTO(CByteArrayInputStream::New(data, (IByteArrayInputStream**)&_stream), FAIL_FINALLY)
    *stream = IInputStream::Probe(_stream);
    REFCOUNT_ADD(*stream);
    goto FAIL_FINALLY;
    //} finally {
FAIL_FINALLY:
    if (cursor != NULL) {
        FAIL_RETURN(ICloseable::Probe(cursor)->Close())
    }
    //}
    return NOERROR;
}

ECode ContactsContractContacts::OpenContactPhotoInputStream(
    /* [in] */ IContentResolver* cr,
    /* [in] */ IUri* contactUri,
    /* [out] */ IInputStream** stream)
{
    return OpenContactPhotoInputStream(cr, contactUri, FALSE, stream);
}

ECode ContactsContractContacts::IsEnterpriseContactId(
    /* [in] */ Int64 contactId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int64 profileId;
    ContactsContractProfile::GetMINID(&profileId);
    *result = (contactId >= IContactsContractContacts::ENTERPRISE_CONTACT_ID_BASE) && (contactId < profileId);
    return NOERROR;
}

} //Provider
} //Droid
} //Elastos
