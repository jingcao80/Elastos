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

#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/content/CContentProviderOperationHelper.h"
#include "elastos/droid/content/CContentUris.h"
#include "elastos/droid/content/CContentValues.h"
#include "elastos/droid/provider/SyncStateContractHelpers.h"
#include "elastos/droid/utility/CPair.h"
#include <Elastos.CoreLibrary.IO.h>
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::CContentProviderOperationHelper;
using Elastos::Droid::Content::IContentProviderOperationBuilder;
using Elastos::Droid::Content::IContentProviderOperationHelper;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Utility::CPair;
using Elastos::Droid::Utility::IPair;
using Elastos::IO::ICloseable;
using Elastos::Core::CArrayOf;
using Elastos::Core::CString;
using Elastos::Core::EIID_IByte;
using Elastos::Core::IArrayOf;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Provider {

static AutoPtr<ArrayOf<String> > initDATAPROJECTION()
{
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(2);
    (*args)[0] = ISyncStateContractColumns::DATA;
    (*args)[0] = IBaseColumns::ID;
    return args;
}

AutoPtr<ArrayOf<String> > SyncStateContractHelpers::DATA_PROJECTION = initDATAPROJECTION();

const String SyncStateContractHelpers::SELECT_BY_ACCOUNT = ISyncStateContractColumns::ACCOUNT_NAME + String("=? AND ")
                                                    + ISyncStateContractColumns::ACCOUNT_TYPE + String("=?");

ECode SyncStateContractHelpers::Get(
    /* [in] */ IContentProviderClient* provider,
    /* [in] */ IUri* uri,
    /* [in] */ IAccount* account,
    /* [out, callee] */ ArrayOf<Byte>** value)
{
    VALIDATE_NOT_NULL(value);

    AutoPtr<ICursor> c;
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(2);
    account->GetName(&(*args)[0]);
    account->GetType(&(*args)[1]);
    FAIL_RETURN(provider->Query(uri, DATA_PROJECTION, SELECT_BY_ACCOUNT, args, String(NULL), (ICursor**)&c))

    // Unable to query the provider
    if (c == NULL) {
        //throw new RemoteException();
        return E_REMOTE_EXCEPTION;
    }

    //try {
    Boolean result;
    FAIL_GOTO(c->MoveToNext(&result), EXIT)
    if (result) {
        Int32 columnIndex;
        FAIL_GOTO(c->GetColumnIndexOrThrow(ISyncStateContractColumns::DATA, &columnIndex), EXIT)
        FAIL_GOTO(c->GetBlob(columnIndex, value), EXIT)
        return ICloseable::Probe(c)->Close();
    }
    //} finally {
EXIT:
    FAIL_RETURN(ICloseable::Probe(c)->Close())
    //}
    *value = NULL;
    return NOERROR;
}

ECode SyncStateContractHelpers::Set(
    /* [in] */ IContentProviderClient* provider,
    /* [in] */ IUri* uri,
    /* [in] */ IAccount* account,
    /* [in] */ ArrayOf<Byte>* data)
{
    AutoPtr<IContentValues> values;
    FAIL_RETURN(CContentValues::New((IContentValues**)&values))

    AutoPtr<IArrayOf> array;
    CArrayOf::New(EIID_IByte, data->GetLength(), (IArrayOf**)&array);
    values->Put(ISyncStateContractColumns::DATA, array);
    String name, type;
    account->GetName(&name);
    account->GetType(&type);
    AutoPtr<ICharSequence> cname, ctype;
    CString::New(name, (ICharSequence**)&cname);
    CString::New(type, (ICharSequence**)&ctype);
    values->Put(ISyncStateContractColumns::ACCOUNT_NAME, cname);
    values->Put(ISyncStateContractColumns::ACCOUNT_TYPE, ctype);
    AutoPtr<IUri> _uri;
    return provider->Insert(uri, values, (IUri**)&_uri);
}

ECode SyncStateContractHelpers::Insert(
    /* [in] */ IContentProviderClient* provider,
    /* [in] */ IUri* uri,
    /* [in] */ IAccount* account,
    /* [in] */ ArrayOf<Byte>* data,
    /* [out] */ IUri** retUri)
{
    VALIDATE_NOT_NULL(retUri);

    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);

    AutoPtr<IArrayOf> array;
    CArrayOf::New(EIID_IByte, data->GetLength(), (IArrayOf**)&array);
    values->Put(ISyncStateContractColumns::DATA, array);
    String name, type;
    account->GetName(&name);
    account->GetType(&type);
    AutoPtr<ICharSequence> cname, ctype;
    CString::New(name, (ICharSequence**)&cname);
    CString::New(type, (ICharSequence**)&ctype);
    values->Put(ISyncStateContractColumns::ACCOUNT_NAME, cname);
    values->Put(ISyncStateContractColumns::ACCOUNT_TYPE, ctype);
    return provider->Insert(uri, values, retUri);
}

ECode SyncStateContractHelpers::Update(
    /* [in] */ IContentProviderClient* provider,
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<Byte>* data)
{
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);

    AutoPtr<IArrayOf> array;
    CArrayOf::New(EIID_IByte, data->GetLength(), (IArrayOf**)&array);
    FAIL_RETURN(values->Put(ISyncStateContractColumns::DATA, array))
    Int32 rowsAffected;
    return provider->Update(uri, values, String(NULL), NULL, &rowsAffected);
}

ECode SyncStateContractHelpers::GetWithUri(
   /* [in] */ IContentProviderClient* provider,
   /* [in] */ IUri* uri,
   /* [in] */ IAccount* account,
   /* [out] */ IPair** value)
{
    VALIDATE_NOT_NULL(value);
    AutoPtr<ICursor> c;
    AutoPtr<ArrayOf<String> > str = ArrayOf<String>::Alloc(2);
    String name;
    String type;
    account->GetName(&name);
    account->GetType(&type);
    (*str)[0] = name;
    (*str)[1] = type;
    provider->Query(uri, DATA_PROJECTION, SELECT_BY_ACCOUNT,
        str.Get(), String(NULL), (ICursor**)&c);

    if (c == NULL) return E_REMOTE_EXCEPTION;

// try {
    Boolean succeeded = FALSE;
    c->MoveToNext(&succeeded);
    if (succeeded) {
        Int64 rowId;
        c->GetInt64(1,&rowId);
        Int32 columnIndex;
        c->GetColumnIndexOrThrow(ISyncStateContractColumns::DATA, &columnIndex);
        AutoPtr<ArrayOf<Byte> > blob;
        c->GetBlob(columnIndex, (ArrayOf<Byte>**)&blob);

        AutoPtr<IContentUris> icu;
        CContentUris::AcquireSingleton((IContentUris**)&icu);
        AutoPtr<IUri> outUri;
        icu->WithAppendedId(uri, rowId, (IUri**)&outUri);
        ICloseable::Probe(c)->Close();
        return CPair::Create(outUri.Get(), CoreUtils::ConvertByteArray(blob).Get(), value);
    }
// } finally {
    ICloseable::Probe(c)->Close();

// }
    *value = NULL;
    return NOERROR;
}

ECode SyncStateContractHelpers::NewSetOperation(
    /* [in] */ IUri* uri,
    /* [in] */ IAccount* account,
    /* [in] */ ArrayOf<Byte>* data,
    /* [out] */ IContentProviderOperation** operation)
{
    VALIDATE_NOT_NULL(operation);

    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);

    AutoPtr<IArrayOf> array;
    CArrayOf::New(EIID_IByte, data->GetLength(), (IArrayOf**)&array);
    values->Put(ISyncStateContractColumns::DATA, array);

    AutoPtr<IContentProviderOperationHelper> helper;
    CContentProviderOperationHelper::AcquireSingleton((IContentProviderOperationHelper**)&helper);
    AutoPtr<IContentProviderOperationBuilder> builder;
    helper->NewInsert(uri, (IContentProviderOperationBuilder**)&builder);

    String name, type;
    account->GetName(&name);
    account->GetType(&type);
    AutoPtr<ICharSequence> cname, ctype;
    CString::New(name, (ICharSequence**)&cname);
    CString::New(type, (ICharSequence**)&ctype);
    builder->WithValue(ISyncStateContractColumns::ACCOUNT_NAME, (IInterface*)cname);
    builder->WithValue(ISyncStateContractColumns::ACCOUNT_TYPE, (IInterface*)ctype);
    return builder->Build(operation);
}

ECode SyncStateContractHelpers::NewUpdateOperation(
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<Byte>* data,
    /* [out] */ IContentProviderOperation** operation)
{
    VALIDATE_NOT_NULL(operation);

    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);

    AutoPtr<IArrayOf> array;
    CArrayOf::New(EIID_IByte, data->GetLength(), (IArrayOf**)&array);
    values->Put(ISyncStateContractColumns::DATA, array);

    AutoPtr<IContentProviderOperationHelper> helper;
    CContentProviderOperationHelper::AcquireSingleton((IContentProviderOperationHelper**)&helper);
    AutoPtr<IContentProviderOperationBuilder> builder;
    helper->NewUpdate(uri, (IContentProviderOperationBuilder**)&builder);
    builder->WithValues(values);
    return builder->Build(operation);
}

} //Provider
} //Droid
} //Elastos
