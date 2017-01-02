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

#include "elastos/droid/app/CPackageDeleteObserver2.h"

using Elastos::Droid::Content::Pm::EIID_IIPackageDeleteObserver2;
using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL_2(CPackageDeleteObserver2, Object, IIPackageDeleteObserver2, IBinder)

CAR_OBJECT_IMPL(CPackageDeleteObserver2)

ECode CPackageDeleteObserver2::constructor(
    /* [in] */ IPackageDeleteObserver* host)
{
    IWeakReferenceSource::Probe(host)->GetWeakReference((IWeakReference**)&mWeakHost);
    return NOERROR;
}

ECode CPackageDeleteObserver2::OnUserActionRequired(
    /* [in] */ IIntent* intent)
{
    AutoPtr<IPackageDeleteObserver> host;
    mWeakHost->Resolve(EIID_IPackageDeleteObserver, (IInterface**)&host);
    return host->OnUserActionRequired(intent);
}

ECode CPackageDeleteObserver2::OnPackageDeleted(
    /* [in] */ const String& basePackageName,
    /* [in] */ Int32 returnCode,
    /* [in] */ const String& msg)
{
    AutoPtr<IPackageDeleteObserver> host;
    mWeakHost->Resolve(EIID_IPackageDeleteObserver, (IInterface**)&host);
    return host->OnPackageDeleted(basePackageName, returnCode, msg);
}

ECode CPackageDeleteObserver2::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

} // namespace App
} // namespace Droid
} // namespace Elastos
