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

#ifndef __ELASTOS_DROID_LOCATION_CLOCATIONREQUESTHELPER_H__
#define __ELASTOS_DROID_LOCATION_CLOCATIONREQUESTHELPER_H__

#include "_Elastos_Droid_Location_CLocationRequestHelper.h"
#include "elastos/droid/location/LocationRequest.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Location {

CarClass(CLocationRequestHelper)
    , public Singleton
    , public ILocationRequestHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Create a location request with default parameters.
     *
     * <p>Default parameters are for a low power, slowly updated location.
     * It can then be adjusted as required by the applications before passing
     * to the {@link LocationManager}
     *
     * @return a new location request
     */
    CARAPI Create(
        /* [out] */ ILocationRequest** lr);

    /** @hide */
    CARAPI CreateFromDeprecatedProvider(
        /* [in] */ const String& provider,
        /* [in] */ Int64 minTime,
        /* [in] */ Float minDistance,
        /* [in] */ Boolean singleShot,
        /* [out] */ ILocationRequest** lr);

    /** @hide */
    CARAPI CreateFromDeprecatedCriteria(
        /* [in] */ ICriteria* criteria,
        /* [in] */ Int64 minTime,
        /* [in] */ Float minDistance,
        /* [in] */ Boolean singleShot,
        /* [out] */ ILocationRequest** lr);

    /** @hide */
    CARAPI QualityToString(
        /* [in] */ Int32 quality,
        /* [out] */ String* str);
};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif // __ELASTOS_DROID_LOCATION_CLOCATIONREQUESTHELPER_H__