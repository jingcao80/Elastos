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

#ifndef __ELASTOS_DROID_TELEPHONY_CELLLOCATION_H__
#define __ELASTOS_DROID_TELEPHONY_CELLLOCATION_H__

#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Telephony.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Telephony {

class CellLocation
    : public Object
    , public ICellLocation
{
public:

    CAR_INTERFACE_DECL()

    /**
    * Request an update of the current location.  If the location has changed,
    * a broadcast will be sent to everyone registered with {@link
    * PhoneStateListener#LISTEN_CELL_LOCATION}.
    */
    static CARAPI RequestLocationUpdate();

    /**
     * Create a new CellLocation from a intent notifier Bundle
     *
     * This method is used by PhoneStateIntentReceiver and maybe by
     * external applications.
     *
     * @param bundle Bundle from intent notifier
     * @return newly created CellLocation
     *
     * @hide
     */
    static CARAPI NewFromBundle(
        /* [in] */ IBundle* bundle,
        /* [out] */ ICellLocation** cl);

    static CARAPI GetEmpty(
        /* [out] */ ICellLocation** cl);

    CARAPI FillInNotifierBundle(
        /* [in] */ IBundle* bundle);

    CARAPI IsEmpty(
        /* [out] */ Boolean* result);
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CELLLOCATION_H__
