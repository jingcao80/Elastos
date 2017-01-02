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

#include "elastos/droid/app/CAppOpsManagerAppOpsCallback.h"
#include "elastos/droid/app/AppOpsManager.h"

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Internal::App::EIID_IIAppOpsCallback;

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL_2(CAppOpsManagerAppOpsCallback, Object, IIAppOpsCallback, IBinder)

CAR_OBJECT_IMPL(CAppOpsManagerAppOpsCallback)

ECode CAppOpsManagerAppOpsCallback::constructor(
        /* [in] */ IAppOpsManagerOnOpChangedListener* callback)
{
    mCallback = callback;
    return NOERROR;
}

ECode CAppOpsManagerAppOpsCallback::OpChanged(
    /* [in] */ Int32 op,
    /* [in] */ const String& packageName)
{
    IAppOpsManagerOnOpChangedInternalListener* internal = IAppOpsManagerOnOpChangedInternalListener::Probe(mCallback);
    if (internal) {
        internal->OnOpChanged(op, packageName);
    }

    const String str = AppOpsManager::sOpToString[op];
    if (!str.IsNull()) {
        mCallback->OnOpChanged(str, packageName);
    }
    return NOERROR;
}

ECode CAppOpsManagerAppOpsCallback::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

} // namespace App
} // namespace Droid
} // namespace Elastos

