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

#include "elastos/droid/internal/widget/CLockPatternUtilsCacheHelper.h"
#include "elastos/droid/internal/widget/CLockPatternUtilsCache.h"
#include <elastos/core/AutoLock.h>

using Elastos::Core::AutoLock;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

CAR_SINGLETON_IMPL(CLockPatternUtilsCacheHelper);
CAR_INTERFACE_IMPL(CLockPatternUtilsCacheHelper, Singleton, ILockPatternUtilsCacheHelper);

ECode CLockPatternUtilsCacheHelper::GetInstance(
    /* [in] */ IILockSettings* service,
    /* [out] */ ILockPatternUtilsCache** instance)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(instance);
    AutoPtr<ILockPatternUtilsCache> cache = CLockPatternUtilsCache::GetInstance(service);
    *instance = cache;
    REFCOUNT_ADD(*instance);

    return NOERROR;
}

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos
