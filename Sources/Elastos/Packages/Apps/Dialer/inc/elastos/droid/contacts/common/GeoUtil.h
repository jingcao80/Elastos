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

#ifndef __ELASTOS_DROID_CONTACTS_COMMON_GEOUTIL_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_GEOUTIL_H__

#include "Elastos.Droid.Content.h"

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {

/**
 * Static methods related to Geo.
 */
class GeoUtil
{
public:
    /**
     * Returns the country code of the country the user is currently in. Before calling this
     * method, make sure that {@link CountryDetector#initialize(Context)} has already been called
     * in {@link Application#onCreate()}.
     * @return The ISO 3166-1 two letters country code of the country the user
     *         is in.
     */
    static CARAPI_(String) GetCurrentCountryIso(
        /* [in] */ IContext* context);

    static CARAPI_(String) GetGeocodedLocationFor(
        /* [in] */ IContext* context,
        /* [in] */ const String& phoneNumber);
};

} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_GEOUTIL_H__
