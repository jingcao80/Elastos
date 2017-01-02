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

#include "CApplicationOne.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace ApplicationDemo {

static const String TAG("ApplicationDemo::CApplicationOne");

CAR_INTERFACE_IMPL(CApplicationOne, Application, IApplicationOne)

CAR_OBJECT_IMPL(CApplicationOne)

ECode CApplicationOne::constructor()
{
    Logger::I(TAG, " >>> create CApplicationOne: %s", TO_CSTR(this));
    return Application::constructor();
}

ECode CApplicationOne::OnCreate()
{
    SetValue(String("CustomAppcalition"));
    return Application::OnCreate();
}

ECode CApplicationOne::SetValue(
    /* [in] */ const String& value)
{
    mValue = value;
    return NOERROR;
}

ECode CApplicationOne::GetValue(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mValue;
    return NOERROR;
}


} // namespace ApplicationDemo
} // namespace DevSamples
} // namespace Elastos