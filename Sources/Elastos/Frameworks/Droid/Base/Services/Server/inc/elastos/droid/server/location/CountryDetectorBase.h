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

#ifndef __ELASTOS_DROID_SERVER_LOCATION_COUNTRYDETECTORBASE_H__
#define __ELASTOS_DROID_SERVER_LOCATION_COUNTRYDETECTORBASE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Server.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Location::ICountry;
using Elastos::Droid::Location::ICountryListener;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

/**
 * This class defines the methods need to be implemented by the country
 * detector.
 * <p>
 * Calling {@link #detectCountry} to start detecting the country. The country
 * could be returned immediately if it is available.
 *
 * @hide
 */
class CountryDetectorBase
    : public Object
{
public:
    CountryDetectorBase(
        /* [in] */ IContext* ctx);

    /**
     * Start detecting the country that the user is in.
     *
     * @return the country if it is available immediately, otherwise null should
     *         be returned.
     */
    virtual CARAPI DetectCountry(
        /* [out] */ ICountry** country) = 0;

    /**
     * Register a listener to receive the notification when the country is detected or changed.
     * <p>
     * The previous listener will be replaced if it exists.
     */
    CARAPI SetCountryListener(
        /* [in] */ ICountryListener* listener);

    /**
     * Stop detecting the country. The detector should release all system services and be ready to
     * be freed
     */
    virtual CARAPI Stop() = 0;

protected:
    CARAPI NotifyListener(
        /* [in] */ ICountry* country);

protected:
    AutoPtr<IHandler> mHandler;
    AutoPtr<IContext> mContext;
    AutoPtr<ICountryListener> mListener;
    AutoPtr<ICountry> mDetectedCountry;
};

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_LOCATION_COUNTRYDETECTORBASE_H__