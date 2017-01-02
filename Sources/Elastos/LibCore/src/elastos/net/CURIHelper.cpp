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

#include "elastos/coredef.h"
#include "elastos/net/CURI.h"
#include "elastos/net/CURIHelper.h"

namespace Elastos {
namespace Net {

CAR_INTERFACE_IMPL(CURIHelper, Singleton, IURIHelper)

CAR_SINGLETON_IMPL(CURIHelper)

ECode CURIHelper::Create(
    /* [in] */ const String& uri,
    /* [out] */ IURI** obj)
{
    return URI::Create(uri, obj);
}

ECode CURIHelper::GetEffectivePort(
    /* [in] */ const String& scheme,
    /* [in] */ Int32 specifiedPort,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = URI::GetEffectivePort(scheme, specifiedPort);
    return NOERROR;
}

} // namespace Net
} // namespace Elastos
