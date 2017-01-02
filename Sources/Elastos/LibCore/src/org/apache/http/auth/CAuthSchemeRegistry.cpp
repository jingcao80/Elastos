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

#include "org/apache/http/auth/CAuthSchemeRegistry.h"
#include "elastos/core/CString.h"
#include "elastos/utility/CArrayList.h"
#include "elastos/utility/CLinkedHashMap.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Utility::CLinkedHashMap;
using Elastos::Utility::ISet;
using Elastos::Utility::ICollection;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::ILocale;

namespace Org {
namespace Apache {
namespace Http {
namespace Auth {

CAuthSchemeRegistry::CAuthSchemeRegistry()
    : Object()
{}

CAR_INTERFACE_IMPL(CAuthSchemeRegistry, Object, IAuthSchemeRegistry)

CAR_OBJECT_IMPL(CAuthSchemeRegistry)

ECode CAuthSchemeRegistry::Register(
    /* [in] */ const String& name,
    /* [in] */ IAuthSchemeFactory* factory)
{
    if (name.IsNull()) {
        Logger::E("CAuthSchemeRegistry", "Name may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (factory == NULL) {
        Logger::E("CAuthSchemeRegistry", "Authentication scheme factory may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<ICharSequence> cs;
    CString::New(name.ToLowerCase(/*ILocale::ENGLISH*/), (ICharSequence**)&cs);
    AutoPtr<IInterface> oldValue;
    return mRegisteredSchemes->Put(cs, factory, (IInterface**)&oldValue);
}

ECode CAuthSchemeRegistry::Unregister(
    /* [in] */ const String& name)
{
    if (name.IsNull()) {
        Logger::E("CAuthSchemeRegistry", "Name may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<ICharSequence> cs;
    CString::New(name.ToLowerCase(/*ILocale::ENGLISH*/), (ICharSequence**)&cs);
    AutoPtr<IInterface> value;
    return mRegisteredSchemes->Remove(cs, (IInterface**)&value);
}

ECode CAuthSchemeRegistry::GetAuthScheme(
    /* [in] */ const String& name,
    /* [in] */ IHttpParams* params,
    /* [out] */ IAuthScheme** scheme)
{
    VALIDATE_NOT_NULL(scheme)
    *scheme = NULL;

    if (name.IsNull()) {
        Logger::E("CAuthSchemeRegistry", "Name may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<ICharSequence> cs;
    CString::New(name.ToLowerCase(/*ILocale::ENGLISH*/), (ICharSequence**)&cs);
    AutoPtr<IInterface> factory;
    mRegisteredSchemes->Get(cs, (IInterface**)&factory);
    if (factory != NULL) {
        return IAuthSchemeFactory::Probe(factory)->NewInstance(params, scheme);
    }
    else {
        Logger::E("CAuthSchemeRegistry", "Unsupported authentication scheme: %s", name.string());
        return E_ILLEGAL_STATE_EXCEPTION;
    }
}

ECode CAuthSchemeRegistry::GetSchemeNames(
    /* [out] */ IList** names)
{
    VALIDATE_NOT_NULL(names)

    AutoPtr<ISet> set;
    mRegisteredSchemes->GetKeySet((ISet**)&set);
    AutoPtr<ICollection> col = ICollection::Probe(set);
    AutoPtr<IArrayList> aList;
    CArrayList::New(col, (IArrayList**)&aList);
    *names = IList::Probe(aList);
    REFCOUNT_ADD(*names)
    return NOERROR;
}

ECode CAuthSchemeRegistry::SetItems(
    /* [in] */ IMap* map) // will exit?
{
    if (map == NULL) {
        return NOERROR;
    }
    mRegisteredSchemes->Clear();
    mRegisteredSchemes->PutAll(map);
    return NOERROR;
}

ECode CAuthSchemeRegistry::constructor()
{
    mRegisteredSchemes = NULL;
    CLinkedHashMap::New((IMap**)&mRegisteredSchemes);
    return NOERROR;
}

} // namespace Auth
} // namespace Http
} // namespace Apache
} // namespace Org
