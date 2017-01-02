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

#include "CServiceConnection.h"
#include "CActivityOne.h"
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Os.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::App::EIID_IActivity;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace ServiceDemo {

static const String TAG("ServiceDemo::CServiceConnection");

CAR_INTERFACE_IMPL(CServiceConnection, Object, IServiceConnection)

CAR_OBJECT_IMPL(CServiceConnection)

ECode CServiceConnection::constructor(
    /* [in] */ IWeakReference* weakHost)
{
    mWeakHost = weakHost;
    return NOERROR;
}

ECode CServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    Logger::I(TAG, "OnServiceConnected: %s, service:%s", TO_CSTR(name), TO_CSTR(service));
    AutoPtr<IActivity> host;
    mWeakHost->Resolve(EIID_IActivity, (IInterface**)&host);
    if (host) {
        IGetSystemTimeService* getTimeService = IGetSystemTimeService::Probe(service);
        String timeStr;
        getTimeService->GetSystemTime(&timeStr);
        CActivityOne* activity = (CActivityOne*)host.Get();
        activity->UpdateTextView(timeStr);
    }
    return NOERROR;
}

ECode CServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    Logger::I(TAG, "OnServiceDisconnected: %s", TO_CSTR(name));
    return NOERROR;
}

} // namespace ServiceDemo
} // namespace DevSamples
} // namespace Elastos
