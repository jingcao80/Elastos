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

#include "CDESKeySpecHelper.h"
#include "CDESKeySpec.h"

namespace Elastosx {
namespace Crypto {
namespace Spec {

CAR_INTERFACE_IMPL(CDESKeySpecHelper, Singleton, IDESKeySpecHelper)

CAR_SINGLETON_IMPL(CDESKeySpecHelper)

ECode CDESKeySpecHelper::IsParityAdjusted(
    /* [in] */ ArrayOf<Byte> * key,
    /* [in] */ Int32 offset,
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result);
    return CDESKeySpec::IsParityAdjusted(key, offset, result);
}

ECode CDESKeySpecHelper::IsWeak(
    /* [in] */ ArrayOf<Byte> * key,
    /* [in] */ Int32 offset,
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result);
    return CDESKeySpec::IsWeak(key, offset, result);
}

} // Spec
} // Crypto
} // Elastosx