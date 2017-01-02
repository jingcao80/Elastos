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

#ifndef __ELASTOS_DROID_SETTINGS_WIFI_SUMMARY_H__
#define __ELASTOS_DROID_SETTINGS_WIFI_SUMMARY_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::NetworkInfoDetailedState;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Wifi {

class Summary
{
public:
    static CARAPI_(String) Get(
        /* [in] */ IContext* context,
        /* [in] */ const String& ssid,
        /* [in] */ NetworkInfoDetailedState state);

    static CARAPI_(String) Get(
        /* [in] */ IContext* context,
        /* [in] */ NetworkInfoDetailedState state);
};

} // namespace Wifi
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_WIFI_SUMMARY_H__
