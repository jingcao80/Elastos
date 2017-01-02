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

#include "elastos/droid/teleservice/phone/CHfaService.h"
#include "elastos/droid/teleservice/phone/HfaLogic.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::App::IPendingIntent;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

CAR_INTERFACE_IMPL(CHfaService::MyHfaLogicHfaLogicCallback, Object, IHfaLogicHfaLogicCallback)

ECode CHfaService::MyHfaLogicHfaLogicCallback::OnSuccess()
{
    Logger::I(TAG, "onSuccess");
    mHost->OnComplete();
    return NOERROR;
}

ECode CHfaService::MyHfaLogicHfaLogicCallback::OnError(
    /* [in] */ const String& error)
{
    StringBuilder sb;
    sb += "onError: ";
    sb += error;
    Logger::I(TAG, sb.ToString());
    // We do not respond from this service. On success or failure
    // we do the same thing...finish.
    mHost->OnComplete();
    return NOERROR;
}

const String CHfaService::TAG("CHfaService");// = HfaService.class.getSimpleName();

CAR_OBJECT_IMPL(CHfaService)

ECode CHfaService::constructor()
{
    return Service::constructor();
}

ECode CHfaService::OnCreate()
{
    return Logger::I(TAG, "service started");
}

ECode CHfaService::OnStartCommand(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 startId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IParcelable> parcelable;
    intent->GetParcelableExtra(IOtaUtils::EXTRA_OTASP_RESULT_CODE_PENDING_INTENT, (IParcelable**)&parcelable);
    AutoPtr<IPendingIntent> otaResponseIntent = IPendingIntent::Probe(parcelable);

    AutoPtr<IHfaLogicHfaLogicCallback> c = new MyHfaLogicHfaLogicCallback(this);

    mHfaLogic = new HfaLogic(this, c, otaResponseIntent);
    mHfaLogic->Start();

    *result = START_REDELIVER_INTENT;
    return NOERROR;
}

ECode CHfaService::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** binder)
{
    VALIDATE_NOT_NULL(binder)

    *binder = NULL;
    return NOERROR;
}

void CHfaService::OnComplete()
{
    StopSelf();
}

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos