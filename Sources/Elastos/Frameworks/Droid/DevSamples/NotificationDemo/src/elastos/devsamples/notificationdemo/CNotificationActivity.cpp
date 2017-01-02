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

#include "CNotificationActivity.h"
#include "R.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace DevSamples {
namespace NotificationDemo {

const String CNotificationActivity::TAG("NotificationDemo::CNotificationActivity");

CAR_OBJECT_IMPL(CNotificationActivity)

ECode CNotificationActivity::constructor()
{
    return Activity::constructor();
}

ECode CNotificationActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Slogger::D(TAG, " >>> OnCreate");
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::notification);

    return NOERROR;
}

ECode CNotificationActivity::OnStart()
{
    Slogger::D(TAG, " >>> OnStart");
    return Activity::OnStart();
}

ECode CNotificationActivity::OnResume()
{
    Slogger::D(TAG, " >>> OnResume");
    return Activity::OnResume();
}

ECode CNotificationActivity::OnPause()
{
    Slogger::D(TAG, " >>> OnPause");
    return Activity::OnPause();
}

ECode CNotificationActivity::OnStop()
{
    Slogger::D(TAG, " >>> OnStop");
    return Activity::OnStop();
}

ECode CNotificationActivity::OnDestroy()
{
    Slogger::D(TAG, " >>> OnDestroy");
    return Activity::OnDestroy();
}

ECode CNotificationActivity::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    Slogger::D(TAG, " >> OnActivityResult()");
    return Activity::OnActivityResult(requestCode, resultCode, data);
}

} // namespace NotificationDemo
} // namespace DevSamples
} // namespace Elastos
