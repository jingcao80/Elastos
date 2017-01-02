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

#include "org/apache/http/params/CDefaultedHttpParams.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Params {

CAR_INTERFACE_IMPL(CDefaultedHttpParams, AbstractHttpParams, IDefaultedHttpParams)

CAR_OBJECT_IMPL(CDefaultedHttpParams)

ECode CDefaultedHttpParams::Copy(
    /* [out] */ IHttpParams** httpParams)
{
    VALIDATE_NOT_NULL(httpParams)
    AutoPtr<IHttpParams> clone;
    mLocal->Copy((IHttpParams**)&clone);
    AutoPtr<IDefaultedHttpParams> hp;
    CDefaultedHttpParams::New(clone, mDefaults, (IDefaultedHttpParams**)&hp);
    *httpParams = IHttpParams::Probe(hp);
    REFCOUNT_ADD(*httpParams)
    return NOERROR;
}

ECode CDefaultedHttpParams::GetParameter(
    /* [in] */ const String& name,
    /* [out] */ IInterface** parameter)
{
    VALIDATE_NOT_NULL(parameter)
    AutoPtr<IInterface> obj;
    mLocal->GetParameter(name, (IInterface**)&obj);
    if (obj == NULL && mDefaults != NULL) {
        mDefaults->GetParameter(name, (IInterface**)&obj);
    }
    *parameter = obj;
    REFCOUNT_ADD(*parameter)
    return NOERROR;
}

ECode CDefaultedHttpParams::RemoveParameter(
    /* [in] */ const String& name,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mLocal->RemoveParameter(name, result);
}

ECode CDefaultedHttpParams::SetParameter(
    /* [in] */ const String& name,
    /* [in] */ IInterface* value,
    /* [out] */ IHttpParams** httpParams)
{
    VALIDATE_NOT_NULL(httpParams)
    return mLocal->SetParameter(name, value, httpParams);
}

ECode CDefaultedHttpParams::GetDefaults(
    /* [out] */ IHttpParams** defaults)
{
    VALIDATE_NOT_NULL(defaults)
    *defaults = mDefaults;
    REFCOUNT_ADD(*defaults)
    return NOERROR;
}

ECode CDefaultedHttpParams::constructor(
    /* [in] */ IHttpParams* local,
    /* [in] */ IHttpParams* defaults)
{
    if (local == NULL) {
        Logger::E("CDefaultedHttpParams", "HTTP parameters may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mLocal = local;
    mDefaults = defaults;
    return NOERROR;
}

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org
