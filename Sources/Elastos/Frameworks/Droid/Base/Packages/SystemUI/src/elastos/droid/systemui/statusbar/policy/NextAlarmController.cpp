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

#include "elastos/droid/systemui/statusbar/policy/NextAlarmController.h"
#include "elastos/droid/systemui/statusbar/policy/CNextAlarmControllerBroadcastReceiver.h"
#include "Elastos.Droid.Os.h"

using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

CAR_INTERFACE_IMPL(NextAlarmController, Object, INextAlarmController)
NextAlarmController::NextAlarmController(
    /* [in] */ IContext* context)
{
    CArrayList::New((IArrayList**)&mChangeCallbacks);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::ALARM_SERVICE, (IInterface**)&obj);
    mAlarmManager = IAlarmManager::Probe(obj);
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_USER_SWITCHED);
    filter->AddAction(IAlarmManager::ACTION_NEXT_ALARM_CLOCK_CHANGED);
    AutoPtr<IBroadcastReceiver> b;
    CNextAlarmControllerBroadcastReceiver::New(this, (IBroadcastReceiver**)&b);
    AutoPtr<IIntent> i;
    context->RegisterReceiver(b, filter, (IIntent**)&i);
    UpdateNextAlarm();
}

ECode NextAlarmController::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    pw->Println(String("NextAlarmController state:"));
    pw->Print(String("  mNextAlarm="));
    pw->Println(mNextAlarm);
    return NOERROR;
}

ECode NextAlarmController::AddStateChangedCallback(
    /* [in] */ INextAlarmChangeCallback* cb)
{
    mChangeCallbacks->Add(cb);
    cb->OnNextAlarmChanged(mNextAlarm);
    return NOERROR;
}

ECode NextAlarmController::RemoveStateChangedCallback(
    /* [in] */ INextAlarmChangeCallback* cb)
{
    mChangeCallbacks->Remove(cb);
    return NOERROR;
}

void NextAlarmController::UpdateNextAlarm()
{
    mAlarmManager->GetNextAlarmClock(IUserHandle::USER_CURRENT, (IAlarmClockInfo**)&mNextAlarm);
    FireNextAlarmChanged();
}

void NextAlarmController::FireNextAlarmChanged()
{
    Int32 n = 0;
    mChangeCallbacks->GetSize(&n);
    for (Int32 i = 0; i < n; i++) {
        AutoPtr<IInterface> obj;
        mChangeCallbacks->Get(i, (IInterface**)&obj);
        INextAlarmChangeCallback::Probe(obj)->OnNextAlarmChanged(mNextAlarm);
    }
}

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
