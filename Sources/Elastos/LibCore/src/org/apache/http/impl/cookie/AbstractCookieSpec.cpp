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

#include "org/apache/http/impl/cookie/AbstractCookieSpec.h"
#include "elastos/core/CString.h"
#include "elastos/utility/CHashMap.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Utility::IHashMap;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::Cookie::EIID_ICookieSpec;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Cookie {

AbstractCookieSpec::AbstractCookieSpec()
{
    AutoPtr<IHashMap> hm;
    CHashMap::New(10, (IHashMap**)&hm);
    mAttribHandlerMap = IMap::Probe(hm);
}

CAR_INTERFACE_IMPL(AbstractCookieSpec, Object, ICookieSpec)

ECode AbstractCookieSpec::RegisterAttribHandler(
    /* [in] */ const String& name,
    /* [in] */ ICookieAttributeHandler* handler)
{
    if (name.IsNull()) {
        Logger::E("AbstractCookieSpec", "Attribute name may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (handler == NULL) {
        Logger::E("AbstractCookieSpec", "Attribute handler may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<ICharSequence> cs;
    CString::New(name, (ICharSequence**)&cs);
    mAttribHandlerMap->Put(cs, handler);
    return NOERROR;
}

ECode AbstractCookieSpec::FindAttribHandler(
    /* [in] */ const String& name,
    /* [out] */ ICookieAttributeHandler** handler)
{
    VALIDATE_NOT_NULL(handler)
    AutoPtr<ICharSequence> cs;
    CString::New(name, (ICharSequence**)&cs);
    AutoPtr<IInterface> value;
    mAttribHandlerMap->Get(cs, (IInterface**)&value);
    *handler = ICookieAttributeHandler::Probe(value);
    REFCOUNT_ADD(*handler)
    return NOERROR;
}

ECode AbstractCookieSpec::GetAttribHandler(
    /* [in] */ const String& name,
    /* [out] */ ICookieAttributeHandler** _handler)
{
    VALIDATE_NOT_NULL(_handler)
    *_handler = NULL;
    AutoPtr<ICookieAttributeHandler> handler;
    FindAttribHandler(name, (ICookieAttributeHandler**)&handler);
    if (handler == NULL) {
        Logger::E("AbstractCookieSpec", "Handler not registered for %s attribute.", name.string());
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    else {
        *_handler = handler;
        REFCOUNT_ADD(*_handler)
        return NOERROR;
    }
}

ECode AbstractCookieSpec::GetAttribHandlers(
    /* [out] */ ICollection** collection)
{
    VALIDATE_NOT_NULL(collection)
    return mAttribHandlerMap->GetValues(collection);
}

} // namespace Cookie
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org