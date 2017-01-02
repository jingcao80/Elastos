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

#include "elastos/droid/app/PackageDeleteObserver.h"
#include "elastos/droid/app/CPackageDeleteObserver2.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(PackageDeleteObserver, Object, IPackageDeleteObserver)

ECode PackageDeleteObserver::constructor()
{
    return CPackageDeleteObserver2::New(this, (IIPackageDeleteObserver2**)&mBinder);
}

ECode PackageDeleteObserver::GetBinder(
    /* [out] */ IIPackageDeleteObserver2** observer)
{
    VALIDATE_NOT_NULL(observer)
    *observer = mBinder;
    REFCOUNT_ADD(*observer)
    return NOERROR;
}

ECode PackageDeleteObserver::OnUserActionRequired(
    /* [in] */ IIntent* intent)
{
    return NOERROR;
}

ECode PackageDeleteObserver::OnPackageDeleted(
    /* [in] */ const String& basePackageName,
    /* [in] */ Int32 returnCode,
    /* [in] */ const String& msg)
{
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
