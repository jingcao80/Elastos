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

#ifndef __ELASTOS_APPS_CONTACTS_COMMON_GEOUTIL_H__
#define __ELASTOS_APPS_CONTACTS_COMMON_GEOUTIL_H__

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {

/**
 * Static methods related to Geo.
 */
class ECO_PUBLIC GeoUtil
    : public Object
{
public:
    static CARAPI_(String) GetCurrentCountryIso(
        /* [in] */ IContext* context);

    static CARAPI_(String) GetGeocodedLocationFor(
        /* [in] */ IContext* context,
        /* [in] */ const String& phoneNumber);
};

} // Common
} // Contacts
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_CONTACTS_COMMON_GEOUTIL_H__
