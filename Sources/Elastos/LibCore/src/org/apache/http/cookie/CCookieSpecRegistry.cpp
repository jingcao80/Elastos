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

#include "org/apache/http/cookie/CCookieSpecRegistry.h"
#include "elastos/core/StringBuilder.h"
#include "elastos/core/StringUtils.h"
#include "elastos/core/CString.h"
#include "elastos/core/AutoLock.h"
#include "elastos/utility/CArrayList.h"
#include "elastos/utility/CLinkedHashMap.h"
#include "elastos/utility/logging/Logger.h"

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::ILocale;
using Elastos::Utility::ISet;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::CLinkedHashMap;

namespace Org {
namespace Apache {
namespace Http {
namespace Cookie {

CAR_INTERFACE_IMPL(CCookieSpecRegistry, Object, ICookieSpecRegistry)

CAR_OBJECT_IMPL(CCookieSpecRegistry)

ECode CCookieSpecRegistry::Register(
    /* [in] */ const String& name,
    /* [in] */ ICookieSpecFactory* factory)
{
    {    AutoLock syncLock(this);
        if (name.IsNull()) {
            Logger::E("CCookieSpecRegistry", "Name may not be null");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        if (factory == NULL) {
            Logger::E("CCookieSpecRegistry", "Cookie spec factory may not be null");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        AutoPtr<ICharSequence> cs;
        CString::New(name.ToLowerCase(/*ILocale::ENGLISH*/), (ICharSequence**)&cs);
        mRegisteredSpecs->Put(cs, factory);
    }
    return NOERROR;
}

ECode CCookieSpecRegistry::Unregister(
    /* [in] */ const String& id)
{
    {    AutoLock syncLock(this);
        if (id.IsNull()) {
            Logger::E("CCookieSpecRegistry", "Id may not be null");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        AutoPtr<ICharSequence> cs;
        CString::New(id.ToLowerCase(/*ILocale::ENGLISH*/), (ICharSequence**)&cs);
        mRegisteredSpecs->Remove(cs);
    }
    return NOERROR;
}

ECode CCookieSpecRegistry::GetCookieSpec(
    /* [in] */ const String& name,
    /* [in] */ IHttpParams* params,
    /* [out] */ ICookieSpec** spec)
{
    {    AutoLock syncLock(this);
        VALIDATE_NOT_NULL(spec)
        *spec = NULL;

        if (name.IsNull()) {
            Logger::E("CCookieSpecRegistry", "Name may not be null");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        AutoPtr<ICharSequence> cs;
        CString::New(name.ToLowerCase(/*ILocale::ENGLISH*/), (ICharSequence**)&cs);
        AutoPtr<IInterface> temp;
        mRegisteredSpecs->Get(cs, (IInterface**)&temp);
        AutoPtr<ICookieSpecFactory> factory = ICookieSpecFactory::Probe(temp);
        if (factory != NULL) {
            return factory->NewInstance(params, spec);
        }
        else {
            Logger::E("CCookieSpecRegistry", "Unsupported cookie spec: %s", name.string());
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode CCookieSpecRegistry::GetCookieSpec(
    /* [in] */ const String& name,
    /* [out] */ ICookieSpec** spec)
{
    {    AutoLock syncLock(this);
        VALIDATE_NOT_NULL(spec)
        GetCookieSpec(name, NULL, spec);
    }
    return NOERROR;
}

ECode CCookieSpecRegistry::GetSpecNames(
    /* [out] */ IList** names)
{
    VALIDATE_NOT_NULL(names)
    {    AutoLock syncLock(this);
        AutoPtr<ISet> keySet;
        mRegisteredSpecs->GetKeySet((ISet**)&keySet);
        AutoPtr<ICollection> col = ICollection::Probe(keySet);
        AutoPtr<IArrayList> al;
        CArrayList::New(col, (IArrayList**)&al);
        *names = IList::Probe(al);
        REFCOUNT_ADD(*names)
    }
    return NOERROR;
}

ECode CCookieSpecRegistry::SetItems(
    /* [in] */ IMap* map)
{
    {    AutoLock syncLock(this);
        if (map == NULL) {
            return NOERROR;
        }
        mRegisteredSpecs->Clear();
        mRegisteredSpecs->PutAll(map);
    }
    return NOERROR;
}

ECode CCookieSpecRegistry::constructor()
{
    CLinkedHashMap::New((IMap**)&mRegisteredSpecs);
    return NOERROR;
}

} // namespace Cookie
} // namespace Http
} // namespace Apache
} // namespace Org
