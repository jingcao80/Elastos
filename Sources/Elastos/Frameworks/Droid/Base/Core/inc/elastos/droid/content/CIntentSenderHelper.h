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

#ifndef __ELASTOS_DROID_CONTENT_CINTENTSENDERHELPER_H__
#define __ELASTOS_DROID_CONTENT_CINTENTSENDERHELPER_H__

#include "_Elastos_Droid_Content_CIntentSenderHelper.h"
#include <elastos/core/Singleton.h>

using namespace Elastos;
using namespace Elastos::Core;

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CIntentSenderHelper)
    , public Singleton
    , public IIntentSenderHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Convenience function for writing either a IntentSender or null pointer to
     * a Parcel.  You must use this with {@link #readIntentSenderOrNullFromParcel}
     * for later reading it.
     *
     * @param sender The IntentSender to write, or null.
     * @param out Where to write the IntentSender.
     */
    CARAPI WriteIntentSenderOrNullToParcel(
        /* [in] */ IIntentSender* sender,
        /* [in] */ IParcel* parcel);

    /**
     * Convenience function for reading either a Messenger or null pointer from
     * a Parcel.  You must have previously written the Messenger with
     * {@link #writeIntentSenderOrNullToParcel}.
     *
     * @param in The Parcel containing the written Messenger.
     *
     * @return Returns the Messenger read from the Parcel, or null if null had
     * been written.
     */
    CARAPI ReadIntentSenderOrNullFromParcel(
        /* [in] */ IParcel* parcel,
        /* [out] */ IIntentSender** intentSender);

};

} // Content
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_CONTENT_CINTENTSENDERHELPER_H__
