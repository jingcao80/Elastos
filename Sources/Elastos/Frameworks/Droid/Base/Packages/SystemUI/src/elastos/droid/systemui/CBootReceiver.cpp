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

#include "elastos/droid/systemui/CBootReceiver.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/droid/provider/Settings.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {

//===============================================================
// CBootReceiver::
//===============================================================
String CBootReceiver::TAG = String("SystemUIBootReceiver");

CAR_OBJECT_IMPL(CBootReceiver)

ECode CBootReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    // Start the load average overlay, if activated
    AutoPtr<IContentResolver> res;
    context->GetContentResolver((IContentResolver**)&res);

    Int32 value = 0;
    ECode ec = Elastos::Droid::Provider::Settings::Global::GetInt32(
        res, ISettingsGlobal::SHOW_PROCESSES, 0, &value);
    if (FAILED(ec)) {
        Logger::E(TAG, "Can't start load average service");
    }

    if (value != 0) {
        AutoPtr<IIntent> loadavg;
        CIntent::New(context,
            ECLSID_CLoadAverageService/*com.android.systemui.LoadAverageService.class*/,
            (IIntent**)&loadavg);

        AutoPtr<IComponentName> name;
        if (FAILED(context->StartService(loadavg, (IComponentName**)&name))) {
            Logger::E(TAG, "Can't start load average service");
        }
    }

    return NOERROR;
}

} // namespace SystemUI
} // namepsace Droid
} // namespace Elastos