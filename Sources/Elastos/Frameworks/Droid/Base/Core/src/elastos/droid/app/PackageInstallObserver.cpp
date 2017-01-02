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

#include "Elastos.Droid.Content.h"
#include "elastos/droid/app/PackageInstallObserver.h"
#include "elastos/droid/app/CPackageInstallObserver2.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(PackageInstallObserver, Object, IPackageInstallObserver)

ECode PackageInstallObserver::constructor()
{
    return CPackageInstallObserver2::New(this, (IIPackageInstallObserver2**)&mBinder);
}

ECode PackageInstallObserver::GetBinder(
    /* [out] */ IIPackageInstallObserver2** pio)
{
    VALIDATE_NOT_NULL(pio)
    *pio = mBinder;
    REFCOUNT_ADD(*pio)
    return NOERROR;
}

ECode PackageInstallObserver::OnUserActionRequired(
    /* [in] */ IIntent* intent)
{
    return NOERROR;
}

ECode PackageInstallObserver::OnPackageInstalled(
    /* [in] */ const String& basePackageName,
    /* [in] */ Int32 returnCode,
    /* [in] */ const String& msg,
    /* [in] */ IBundle* extras)
{
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
