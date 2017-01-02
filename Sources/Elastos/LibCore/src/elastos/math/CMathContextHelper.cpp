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

#include "CMathContextHelper.h"
#include "CMathContext.h"

namespace Elastos {
namespace Math {

CAR_INTERFACE_IMPL(CMathContextHelper, Singleton, IMathContextHelper)

CAR_SINGLETON_IMPL(CMathContextHelper)

ECode CMathContextHelper::GetDECIMAL32(
    /* [out] */ IMathContext** context)
{
    VALIDATE_NOT_NULL(context);
    *context = CMathContext::DECIMAL32;
    REFCOUNT_ADD(*context);
    return NOERROR;
}

ECode CMathContextHelper::GetDECIMAL64(
    /* [out] */ IMathContext** context)
{
    VALIDATE_NOT_NULL(context);
    *context = CMathContext::DECIMAL64;
    REFCOUNT_ADD(*context);
    return NOERROR;
}

ECode CMathContextHelper::GetDECIMAL128(
    /* [out] */ IMathContext** context)
{
    VALIDATE_NOT_NULL(context);
    *context = CMathContext::DECIMAL128;
    REFCOUNT_ADD(*context);
    return NOERROR;
}

ECode CMathContextHelper::GetUNLIMITED(
    /* [out] */ IMathContext** context)
{
    VALIDATE_NOT_NULL(context);
    *context = CMathContext::UNLIMITED;
    REFCOUNT_ADD(*context);
    return NOERROR;
}

} // namespace Math
} // namespace Elastos
