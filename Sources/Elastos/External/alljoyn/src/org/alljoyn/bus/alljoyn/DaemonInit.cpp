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

#include "org/alljoyn/bus/alljoyn/DaemonInit.h"

namespace Org {
namespace Alljoyn {
namespace Bus {
namespace Alljoyn {

AutoPtr<IBroadcastReceiver> DaemonInit::sReceiver;
const String DaemonInit::TAG("DaemonInit");
AutoPtr<IContext> DaemonInit::sContext;

AutoPtr<IContext> DaemonInit::GetContext()
{
    return sContext;
}

Boolean DaemonInit::PrepareDaemon(
    /* [in] */ IContext* context)
{
    // Log.v(TAG, "Android version : " + android.os.Build.VERSION.SDK_INT);

    sContext = NULL;
    context->GetApplicationContext((IContext**)&sContext);
    // Log.v(TAG, "Saved application context");

    return TRUE;
}

} // namespace Alljoyn
} // namespace Bus
} // namespace Alljoyn
} // namespace Org
