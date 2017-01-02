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

#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_NEXTALARMCONTROLLER_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_NEXTALARMCONTROLLER_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.App.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Droid::App::IAlarmClockInfo;
using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Core::Object;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

class CNextAlarmControllerBroadcastReceiver;
class NextAlarmController
    : public Object
    , public INextAlarmController
{
public:
    CAR_INTERFACE_DECL()

    NextAlarmController(
        /* [in] */ IContext* context);

    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    CARAPI AddStateChangedCallback(
        /* [in] */ INextAlarmChangeCallback* cb);

    CARAPI RemoveStateChangedCallback(
        /* [in] */ INextAlarmChangeCallback* cb);

    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

private:
    CARAPI_(void) UpdateNextAlarm();

    CARAPI_(void) FireNextAlarmChanged();

private:
    AutoPtr<IArrayList> mChangeCallbacks;  /*<NextAlarmChangeCallback*/

    AutoPtr<IAlarmManager> mAlarmManager;
    AutoPtr<IAlarmClockInfo> mNextAlarm;
    friend class CNextAlarmControllerBroadcastReceiver;
};

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_NEXTALARMCONTROLLER_H__
