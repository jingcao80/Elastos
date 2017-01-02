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

#include "CObjectIdentifierInUtilsHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Utils {

CAR_SINGLETON_IMPL(CObjectIdentifierInUtilsHelper)

CAR_INTERFACE_IMPL(CObjectIdentifierInUtilsHelper, Singleton, IObjectIdentifierInUtilsHelper)

ECode CObjectIdentifierInUtilsHelper::ValidateOid(
    /* [in] */ ArrayOf<Int32> * pOid)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CObjectIdentifierInUtilsHelper::HashIntArray(
    /* [in] */ ArrayOf<Int32> * pArray,
    /* [out] */ Int32 * pHash)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}

