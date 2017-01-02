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

#include "elastos/droid/utility/CArrayMapHelper.h"
#include "elastos/droid/utility/CArrayMap.h"

namespace Elastos {
namespace Droid {
namespace Utility {

CAR_SINGLETON_IMPL(CArrayMapHelper);
CAR_INTERFACE_IMPL(CArrayMapHelper, Singleton, IArrayMapHelper);
ECode CArrayMapHelper::GetEMPTY(
    /* [out] */ IArrayMap** map)
{
    VALIDATE_NOT_NULL(map);
    *map = CArrayMap::EMPTY;
    REFCOUNT_ADD(*map);
    return NOERROR;
}

} // Utility
} // Droid
} // Elastos
