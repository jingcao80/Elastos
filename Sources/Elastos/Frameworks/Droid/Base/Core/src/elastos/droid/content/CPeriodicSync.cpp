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

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Accounts.h"
#include "elastos/droid/content/CPeriodicSync.h"
#include "elastos/droid/os/CBundle.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Os::CBundle;
using Elastos::Utility::ISet;
using Elastos::Utility::IIterator;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL_2(CPeriodicSync, Object, IPeriodicSync, IParcelable)

CAR_OBJECT_IMPL(CPeriodicSync)

CPeriodicSync::CPeriodicSync()
    : mPeriod(0)
    , mFlexTime(0)
{}

CPeriodicSync::~CPeriodicSync()
{}

ECode CPeriodicSync::constructor()
{
    return NOERROR;
}

ECode CPeriodicSync::constructor(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authority,
    /* [in] */ IBundle* extras,
    /* [in] */ Int64 period)
{
    mAccount = account;
    mAuthority = authority;
    FAIL_RETURN(CBundle::New(extras, (IBundle**)&mExtras))
    mPeriod = period;
    return NOERROR;
}

ECode CPeriodicSync::constructor(
    /* [in] */ IPeriodicSync* other)
{
    return NOERROR;
}

ECode CPeriodicSync::constructor(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authority,
    /* [in] */ IBundle* extras,
    /* [in] */ Int64 periodInSeconds,
    /* [in] */ Int64 flexTime)
{
    return NOERROR;
}

ECode CPeriodicSync::GetAccount(
    /* [out] */ IAccount** account)
{
    VALIDATE_NOT_NULL(account)
    *account = mAccount;
    REFCOUNT_ADD(*account);
    return NOERROR;
}

ECode CPeriodicSync::GetAuthority(
    /* [out] */ String* authority)
{
    VALIDATE_NOT_NULL(authority)
    *authority = mAuthority;
    return NOERROR;
}

ECode CPeriodicSync::GetExtras(
    /* [out] */ IBundle** extras)
{
    VALIDATE_NOT_NULL(extras)
    *extras = mExtras;
    REFCOUNT_ADD(*extras);
    return NOERROR;
}

ECode CPeriodicSync::GetPeriod(
    /* [out] */ Int64* period)
{
    VALIDATE_NOT_NULL(period)
    *period = mPeriod;
    return NOERROR;
}

ECode CPeriodicSync::GetFlexTime(
    /* [out] */ Int64* flexTime)
{
    VALIDATE_NOT_NULL(flexTime)
    *flexTime = mFlexTime;
    return NOERROR;
}

ECode CPeriodicSync::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* isEqual)
{
    VALIDATE_NOT_NULL(isEqual)
    *isEqual = FALSE;

    if (IPeriodicSync::Probe(obj) == NULL) {
        return NOERROR;
    }

    AutoPtr<IPeriodicSync> other = IPeriodicSync::Probe(obj);
    if (obj == (IPeriodicSync*)this) {
        *isEqual = TRUE;
        return NOERROR;
    }

    AutoPtr<IAccount> account;
    AutoPtr<IBundle> extras;
    String authority;
    Int64 period = 0;
    FAIL_RETURN(other->GetAccount((IAccount**)&account))
    FAIL_RETURN(other->GetExtras((IBundle**)&extras))
    FAIL_RETURN(other->GetAuthority(&authority))
    FAIL_RETURN(other->GetPeriod(&period))

    *isEqual = Object::Equals(mAccount, account)
            && mAuthority.Equals(authority)
            && mPeriod == period
            && SyncExtrasEquals(extras, extras);

    return NOERROR;
}

Boolean CPeriodicSync::SyncExtrasEquals(
    /* [in] */ IBundle* b1,
    /* [in] */ IBundle* b2)
{
    if (b1 == b2) {
        return TRUE;
    }

    Int32 size1, size2;
    b1->GetSize(&size1);
    b2->GetSize(&size2);
    if (size2 != size2) {
        return FALSE;
    }

    Boolean isEmpty;
    b1->IsEmpty(&isEmpty);
    if (isEmpty) {
        return TRUE;
    }

    AutoPtr<ISet> keyset;
    b1->GetKeySet((ISet**)&keyset);
    AutoPtr<IIterator> it;
    keyset->GetIterator((IIterator**)&it);
    Boolean hasNext, contains;
    String key;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> keyObj;
        it->GetNext((IInterface**)&keyObj);
        key = Object::ToString(keyObj);
        b2->ContainsKey(key, &contains);
        if (!contains) {
            return FALSE;
        }

        AutoPtr<IInterface> v1, v2;
        b1->Get(key, (IInterface**)&v1);
        b2->Get(key, (IInterface**)&v2);
        if (!Object::Equals(v1, v2)) {
            return FALSE;
        }
    }

    return TRUE;
}

ECode CPeriodicSync::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("account: ");
    sb += Object::ToString(mAccount);
    sb += ", authority: ";
    sb += mAuthority;
    sb += ". period: ";
    sb += mPeriod;
    sb += "s ";
    sb += ", flex: ";
    sb += mFlexTime;
    *str = sb.ToString();
    return NOERROR;
}

ECode CPeriodicSync::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    AutoPtr<IInterface> account;
    source->ReadInterfacePtr((Handle32*)(IInterface**)&account);
    mAccount = IAccount::Probe(account);
    source->ReadString(&mAuthority);
    source->ReadInterfacePtr((Handle32*)&mExtras);
    source->ReadInt64(&mPeriod);
    source->ReadInt64(&mFlexTime);
    return NOERROR;
}

ECode CPeriodicSync::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInterfacePtr(mAccount);
    dest->WriteString(mAuthority);
    dest->WriteInterfacePtr(mExtras);
    dest->WriteInt64(mPeriod);
    dest->WriteInt64(mFlexTime);
    return NOERROR;
}

} // namespace Content
} // namespace Elastos
} // namespace Droid

