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

#include "org/apache/http/protocol/CDefaultedHttpContext.h"
#include "elastos/core/CString.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

CAR_INTERFACE_IMPL_2(CDefaultedHttpContext, Object, IDefaultedHttpContext, IHttpContext)

CAR_OBJECT_IMPL(CDefaultedHttpContext)

ECode CDefaultedHttpContext::GetAttribute(
    /* [in] */ const String& id,
    /* [out] */ IInterface** attr)
{
    VALIDATE_NOT_NULL(attr)
    AutoPtr<IInterface> obj;
    mLocal->GetAttribute(id, (IInterface**)&obj);
    if (obj == NULL) {
        return mDefaults->GetAttribute(id, attr);
    }
    else {
        *attr = obj;
        REFCOUNT_ADD(*attr)
        return NOERROR;
    }
}

ECode CDefaultedHttpContext::RemoveAttribute(
    /* [in] */ const String& id,
    /* [out] */ IInterface** attrs)
{
    VALIDATE_NOT_NULL(attrs)
    return mLocal->RemoveAttribute(id, attrs);
}

ECode CDefaultedHttpContext::SetAttribute(
    /* [in] */ const String& id,
    /* [in] */ IInterface* obj)
{
    return mLocal->SetAttribute(id, obj);
}

ECode CDefaultedHttpContext::GetDefaults(
    /* [out] */ IHttpContext** defaults)
{
    VALIDATE_NOT_NULL(defaults)
    *defaults = mDefaults;
    REFCOUNT_ADD(*defaults)
    return NOERROR;
}

ECode CDefaultedHttpContext::constructor(
    /* [in] */ IHttpContext* local,
    /* [in] */ IHttpContext* defaults)
{
    if (local == NULL) {
        Logger::E("CDefaultedHttpContext", "HTTP context may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mLocal = local;
    mDefaults = defaults;
    return NOERROR;
}

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org
