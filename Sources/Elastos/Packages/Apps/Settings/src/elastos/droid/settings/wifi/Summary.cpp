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

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "elastos/droid/settings/wifi/Summary.h"
#include "../R.h"

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Net::INetworkInfoHelper;
using Elastos::Droid::Net::CNetworkInfoHelper;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Wifi {

String Summary::Get(
    /* [in] */ IContext* context,
    /* [in] */ const String& ssid,
    /* [in] */ NetworkInfoDetailedState state)
{
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr< ArrayOf<String> > formats;
    res->GetStringArray(ssid.IsNull()
            ? R::array::wifi_status : R::array::wifi_status_with_ssid,
            (ArrayOf<String>**)&formats);

    AutoPtr<INetworkInfoHelper> helper;
    CNetworkInfoHelper::AcquireSingleton((INetworkInfoHelper**)&helper);
    Int32 index;
    helper->Index(state, &index);

    if (index >= formats->GetLength() || (*formats)[index].GetLength() == 0) {
        return String(NULL);
    }
    String str("");
    str.AppendFormat((*formats)[index].string(), ssid.string());
    return str;
}

String Summary::Get(
    /* [in] */ IContext* context,
    /* [in] */ NetworkInfoDetailedState state)
{
    return Get(context, String(NULL), state);
}

} // namespace Wifi
} // namespace Settings
} // namespace Droid
} // namespace Elastos
