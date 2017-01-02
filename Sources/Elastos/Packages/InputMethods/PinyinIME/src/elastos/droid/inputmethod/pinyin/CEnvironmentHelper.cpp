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

#include "elastos/droid/ext/frameworkdef.h"
#include "CEnvironmentHelper.h"
#include "CEnvironment.h"

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

AutoPtr<IEnvironment> CEnvironmentHelper::mInstance;

ECode CEnvironmentHelper::GetInstance(
    /* [out] */ IEnvironment** env)
{
    VALIDATE_NOT_NULL(env);
    if (NULL == mInstance) {
        CEnvironment::New((IEnvironment**)&mInstance);
    }
    *env = mInstance;
    REFCOUNT_ADD(*env);
    return NOERROR;
}

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos
