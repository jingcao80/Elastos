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

#include "org/apache/http/impl/client/BasicCookieStore.h"
#include "org/apache/http/cookie/CCookieIdentityComparator.h"
#include "elastos/core/AutoLock.h"
#include "elastos/utility/CDate.h"
#include "elastos/utility/CArrayList.h"
#include "elastos/utility/logging/Logger.h"
#include "elastos/utility/Collections.h"

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CDate;
using Elastos::Utility::Collections;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::Client::IAuthenticationHandler;
using Org::Apache::Http::Client::EIID_IAuthenticationHandler;
using Org::Apache::Http::Cookie::CCookieIdentityComparator;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Client {

BasicCookieStore::BasicCookieStore()
{
    CArrayList::New((IArrayList**)&mCookies);
    CCookieIdentityComparator::New((IComparator**)&mCookieComparator);
}

CAR_INTERFACE_IMPL(BasicCookieStore, Object, IAuthenticationHandler)

ECode BasicCookieStore::AddCookie(
    /* [in] */ ICookie* cookie)
{
    {    AutoLock syncLock(this);
        if (cookie != NULL) {
            // first remove any old cookie that is equivalent
            AutoPtr<IIterator> it;
            mCookies->GetIterator((IIterator**)&it);
            Boolean hasNext;
            while(it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> value;
                it->GetNext((IInterface**)&value);
                Int32 result;
                if (mCookieComparator->Compare(cookie, value, &result), result == 0) {
                    it->Remove();
                    break;
                }
            }
            AutoPtr<IDate> date;
            CDate::New((IDate**)&date);
            Boolean isExpired;
            if (cookie->IsExpired(date, &isExpired), !isExpired) {
                mCookies->Add(cookie);
            }
        }
    }
    return NOERROR;
}

void BasicCookieStore::AddCookies(
    /* [in] */ ArrayOf<ICookie*>* cookies)
{
    {    AutoLock syncLock(this);
        if (cookies != NULL) {
            for (Int32 i = 0; i < cookies->GetLength(); ++i) {
                AddCookie((*cookies)[i]);
            }
        }
    }
}

ECode BasicCookieStore::GetCookies(
    /* [out] */ IList** cookies)
{
    VALIDATE_NOT_NULL(cookies)
    {    AutoLock syncLock(this);
        Collections::UnmodifiableList(IList::Probe(mCookies), cookies);
    }
    return NOERROR;
}

ECode BasicCookieStore::ClearExpired(
    /* [in] */ IDate* date,
    /* [out] */ Boolean* purged)
{
    VALIDATE_NOT_NULL(purged)
    {    AutoLock syncLock(this);
        if (date == NULL) {
            *purged = FALSE;
            return NOERROR;
        }
        Boolean removed = FALSE;
        AutoPtr<IIterator> it;
        mCookies->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while(it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> value;
            it->GetNext((IInterface**)&value);
            AutoPtr<ICookie> cookie = ICookie::Probe(value);
            Boolean isExpired;
            if (cookie->IsExpired(date, &isExpired), isExpired) {
                it->Remove();
                removed = TRUE;
            }
        }
        *purged = removed;
    }
    return NOERROR;
}

ECode BasicCookieStore::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    return IObject::Probe(mCookies)->ToString(str);
}

ECode BasicCookieStore::Clear()
{
    {    AutoLock syncLock(this);
        mCookies->Clear();
    }
    return NOERROR;
}

} // namespace Client
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org