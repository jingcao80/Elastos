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

#include "CCloseGuardHelper.h"
#include "CCloseGuard.h"

namespace Elastos {
namespace Core {

CAR_INTERFACE_IMPL(CCloseGuardHelper, Singleton, ICloseGuardHelper)

CAR_SINGLETON_IMPL(CCloseGuardHelper)

ECode CCloseGuardHelper::Get(
    /* [out] */ ICloseGuard** guard)
{
    VALIDATE_NOT_NULL(guard);

    AutoPtr<ICloseGuard> res = CCloseGuard::Get();
    *guard = res;
    REFCOUNT_ADD(*guard)
    return NOERROR;
}

ECode CCloseGuardHelper::SetEnabled(
    /* [in] */ Boolean enabled)
{
    return CCloseGuard::SetEnabled(enabled);
}

ECode CCloseGuardHelper::SetReporter(
    /* [in] */ ICloseGuardReporter* reporter)
{
    return CCloseGuard::SetReporter(reporter);
}

ECode CCloseGuardHelper::GetReporter(
    /* [out] */ ICloseGuardReporter** report)
{
    VALIDATE_NOT_NULL(report);

    AutoPtr<ICloseGuardReporter> res = CCloseGuard::GetReporter();
    *report = res;
    REFCOUNT_ADD(*report)
    return NOERROR;
}

} // namespace Core
} // namespace Elastos