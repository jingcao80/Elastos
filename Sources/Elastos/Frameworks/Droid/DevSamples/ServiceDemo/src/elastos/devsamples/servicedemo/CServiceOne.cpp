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

#include "CServiceOne.h"
#include "CGetSystemTimeService.h"
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.App.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace ServiceDemo {

static const String TAG("ServiceDemo::CServiceOne");

CAR_OBJECT_IMPL(CServiceOne)

ECode CServiceOne::constructor()
{
    return NOERROR;
}

ECode CServiceOne::OnCreate()
{
    Logger::I(TAG, "OnCreate");
    return NOERROR;
}

ECode CServiceOne::OnStart(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 startId)
{
    Logger::I(TAG, "OnStart: %s", TO_CSTR(intent));
    return NOERROR;
}

ECode CServiceOne::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** binder)
{
    VALIDATE_NOT_NULL(binder)
    Logger::I(TAG, "OnBind: %s", TO_CSTR(intent));
    return CGetSystemTimeService::New(binder);
}

ECode CServiceOne::OnUnbind(
    /* [in] */ IIntent* intent,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    Logger::I(TAG, "OnUnbind: %s", TO_CSTR(intent));
    *succeeded = TRUE;
    return NOERROR;
}



} // namespace ServiceDemo
} // namespace DevSamples
} // namespace Elastos
