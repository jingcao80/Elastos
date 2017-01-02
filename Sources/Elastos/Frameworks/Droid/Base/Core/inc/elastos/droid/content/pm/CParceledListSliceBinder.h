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

#ifndef __ELASTOS_DROID_CONTENT_PM_CPARCELEDLISTSLICEBINDER_H__
#define __ELASTOS_DROID_CONTENT_PM_CPARCELEDLISTSLICEBINDER_H__

#include "_Elastos_Droid_Content_Pm_CParceledListSliceBinder.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/os/Binder.h"

using Elastos::Droid::Os::Binder;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

/**
 * Transfer a large list of Parcelable objects across an IPC.  Splits into
 * multiple transactions if needed.
 *
 * Caveat: for efficiency and security, all elements must be the same concrete type.
 * In order to avoid writing the class name of each object, we must ensure that
 * each object is the same type, or else unparceling then reparceling the data may yield
 * a different result if the class name encoded in the Parcelable is a Base type.
 * See b/17671747.
 *
 * @hide
 */
CarClass(CParceledListSliceBinder)
    , public Binder
{
public:
    CAR_OBJECT_DECL()

    CParceledListSliceBinder();

    CARAPI constructor(
        /* [in] */ IInterface* obj,
        /* [in] */ IList* list,
        /* [in] */ Int32 N);

    CARAPI Transact(
        /* [in] */ Int32 code,
        /* [in] */ IParcel* data,
        /* [in] */ IParcel* reply) /*throws RemoteException*/;

private:
    AutoPtr<IList> mList;
    AutoPtr<IInterface> mObj;
    Int32 mN;
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_CPARCELEDLISTSLICEBINDER_H__
