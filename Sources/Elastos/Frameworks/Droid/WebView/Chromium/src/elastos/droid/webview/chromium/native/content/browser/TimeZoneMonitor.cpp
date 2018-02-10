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

#include "elastos/droid/webkit/webview/chromium/native/content/browser/TimeZoneMonitor.h"
#include "elastos/droid/webkit/webview/chromium/native/content/api/TimeZoneMonitor_dec.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::EIID_IBroadcastReceiver;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

//=====================================================================
//               TimeZoneMonitor::InnerBroadcastReceiver
//=====================================================================
TimeZoneMonitor::InnerBroadcastReceiver::InnerBroadcastReceiver(
    /* [in] */ TimeZoneMonitor* owner)
    : mOwner(owner)
{
}

CAR_INTERFACE_IMPL(TimeZoneMonitor::InnerBroadcastReceiver, Object, IBroadcastReceiver);

ECode TimeZoneMonitor::InnerBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (!action.Equals(IIntent::ACTION_TIMEZONE_CHANGED)) {
        Slogger::E(TAG, "unexpected intent");
        return NOERROR;
    }

    return mOwner->NativeTimeZoneChangedFromJava(mOwner->mNativePtr);
}

//=====================================================================
//                           TimeZoneMonitor
//=====================================================================
const String TimeZoneMonitor::TAG("TimeZoneMonitor");

TimeZoneMonitor::TimeZoneMonitor(
    /* [in] */ IContext* context,
    /* [in] */ HANDLE nativePtr)
    : mNativePtr(nativePtr)
{
    context->GetApplicationContext((IContext**)&mAppContext);
    AutoPtr<IIntent> stickyIntent;
    mAppContext->RegisterReceiver(mBroadcastReceiver, mFilter, (IIntent**)&stickyIntent);
}

AutoPtr<TimeZoneMonitor> TimeZoneMonitor::GetInstance(
    /* [in] */ IContext* context,
    /* [in] */ HANDLE nativePtr)
{
    return new TimeZoneMonitor(context, nativePtr);
}

ECode TimeZoneMonitor::Stop()
{
    mAppContext->UnregisterReceiver(mBroadcastReceiver);
    mNativePtr = 0;

    return NOERROR;
}

ECode TimeZoneMonitor::NativeTimeZoneChangedFromJava(
    /* [in] */ HANDLE nativeTimeZoneMonitorAndroid)
{
    Elastos_TimeZoneMonitor_nativeTimeZoneChangedFromJava(TO_IINTERFACE(this), nativeTimeZoneMonitorAndroid);
    return NOERROR;
}

AutoPtr<IInterface> TimeZoneMonitor::GetInstance(
    /* [in] */ IInterface* context,
    /* [in] */ HANDLE nativePtr)
{
    AutoPtr<IContext> c = IContext::Probe(context);
    return TO_IINTERFACE(GetInstance(c, nativePtr));
}

void TimeZoneMonitor::Stop(
    /* [in] */ IInterface* obj)
{
    AutoPtr<TimeZoneMonitor> mObj = (TimeZoneMonitor*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Slogger::E(TAG, "TimeZoneMonitor::Stop, mObj is NULL");
        return;
    }
    mObj->Stop();
}

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
