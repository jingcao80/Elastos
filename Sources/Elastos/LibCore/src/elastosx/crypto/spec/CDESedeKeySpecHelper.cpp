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

#include "CDESedeKeySpecHelper.h"
#include "CDESedeKeySpec.h"

namespace Elastosx {
namespace Crypto {
namespace Spec {

CAR_INTERFACE_IMPL(CDESedeKeySpecHelper, Singleton, IDESedeKeySpecHelper)

CAR_SINGLETON_IMPL(CDESedeKeySpecHelper)

ECode CDESedeKeySpecHelper::IsParityAdjusted(
    /* [in] */ ArrayOf<Byte> * key,
    /* [in] */ Int32 offset,
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result);
    return CDESedeKeySpec::IsParityAdjusted(key, offset, result);
}

} // Spec
} // Crypto
} // Elastosx