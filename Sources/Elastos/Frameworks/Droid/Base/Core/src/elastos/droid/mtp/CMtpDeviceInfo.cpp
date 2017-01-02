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

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/mtp/CMtpDeviceInfo.h"

using namespace Elastos::Core;
using namespace Elastos::Utility::Logging;

namespace Elastos {
namespace Droid {
namespace Mtp {

CMtpDeviceInfo::CMtpDeviceInfo()
    : mManufacturer(NULL)
    , mModel(NULL)
    , mVersion(NULL)
    , mSerialNumber(NULL)
{}

CAR_INTERFACE_IMPL(CMtpDeviceInfo, Object, IMtpDeviceInfo)

CAR_OBJECT_IMPL(CMtpDeviceInfo)

ECode CMtpDeviceInfo::GetManufacturer(
        /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mManufacturer;
    return NOERROR;
}

ECode CMtpDeviceInfo::GetModel(
        /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mModel;
    return NOERROR;
}

ECode CMtpDeviceInfo::GetVersion(
        /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mVersion;
    return NOERROR;
}

ECode CMtpDeviceInfo::GetSerialNumber(
        /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mSerialNumber;
    return NOERROR;
}

ECode CMtpDeviceInfo::SetManufacturer(
        /* [in] */ const String& str)
{
    mManufacturer = str;
    return NOERROR;
}

ECode CMtpDeviceInfo::SetModel(
        /* [in] */ const String& str)
{
    mModel = str;
    return NOERROR;
}

ECode CMtpDeviceInfo::SetVersion(
        /* [in] */ const String& str)
{
    mVersion = str;
    return NOERROR;
}

ECode CMtpDeviceInfo::SetSerialNumber(
        /* [in] */ const String& str)
{
    mSerialNumber = str;
    return NOERROR;
}
} // namespace Mtp
} // namepsace Droid
} // namespace Elastos
