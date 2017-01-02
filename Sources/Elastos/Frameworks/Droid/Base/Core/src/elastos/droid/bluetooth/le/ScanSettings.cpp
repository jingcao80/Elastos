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

#include "elastos/droid/bluetooth/le/ScanSettings.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Bluetooth {
namespace LE {

//=====================================================================
//                        ScanSettings::Builder
//=====================================================================
CAR_INTERFACE_IMPL(ScanSettings::Builder, Object, IScanSettingsBuilder);

ScanSettings::Builder::Builder()
{
    mScanMode = SCAN_MODE_LOW_POWER;
    mCallbackType = CALLBACK_TYPE_ALL_MATCHES;
    mScanResultType = SCAN_RESULT_TYPE_FULL;
    mReportDelayMillis = 0;
}

ECode ScanSettings::Builder::SetScanMode(
    /* [in] */ Int32 scanMode)
{
    if (scanMode < SCAN_MODE_LOW_POWER || scanMode > SCAN_MODE_LOW_LATENCY) {
        //throw new IllegalArgumentException("invalid scan mode " + scanMode);
        Logger::E("canSettings::Builder::SetScanMode", "invalid scan mode %d", scanMode);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mScanMode = scanMode;
    //return this;
    return NOERROR;
}

ECode ScanSettings::Builder::SetCallbackType(
    /* [in] */ Int32 callbackType)
{
    if (!IsValidCallbackType(callbackType)) {
        //throw new IllegalArgumentException("invalid callback type - " + callbackType);
        Logger::E("canSettings::Builder::SetCallbackType", "invalid callback type  %d", callbackType);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mCallbackType = callbackType;
    // return this;
    return NOERROR;
}

ECode ScanSettings::Builder::SetScanResultType(
    /* [in] */ Int32 scanResultType)
{
    if (scanResultType < SCAN_RESULT_TYPE_FULL
            || scanResultType > SCAN_RESULT_TYPE_ABBREVIATED) {
        //throw new IllegalArgumentException(
        //        "invalid scanResultType - " + scanResultType);
        Logger::E("canSettings::Builder::SetScanResultType", "invalid scanResultType - %d", scanResultType);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mScanResultType = scanResultType;
    // return this;
    return NOERROR;
}

ECode ScanSettings::Builder::SetReportDelay(
    /* [in] */ Int64 reportDelayMillis)
{
    if (reportDelayMillis < 0) {
        //throw new IllegalArgumentException("reportDelay must be > 0");
        Logger::E("canSettings::Builder::SetReportDelay", "reportDelay must be > 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mReportDelayMillis = reportDelayMillis;
    // return this;
    return NOERROR;
}

ECode ScanSettings::Builder::Build(
    /* [out] */ IScanSettings** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ScanSettings> ss = new ScanSettings();
    ss->constructor(mScanMode, mCallbackType, mScanResultType,
            mReportDelayMillis);
    *result = ss;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

Boolean ScanSettings::Builder::IsValidCallbackType(
    /* [in] */ Int32 callbackType)
{
    if (callbackType == CALLBACK_TYPE_ALL_MATCHES ||
            callbackType == CALLBACK_TYPE_FIRST_MATCH ||
            callbackType == CALLBACK_TYPE_MATCH_LOST) {
        return TRUE;
    }
    return callbackType == (CALLBACK_TYPE_FIRST_MATCH | CALLBACK_TYPE_MATCH_LOST);
}

//=====================================================================
//                             ScanSettings
//=====================================================================
CAR_INTERFACE_IMPL_2(ScanSettings, Object, IScanSettings, IParcelable);

ScanSettings::ScanSettings()
{
}

ECode ScanSettings::GetScanMode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mScanMode;
    return NOERROR;
}

ECode ScanSettings::GetCallbackType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCallbackType;
    return NOERROR;
}

ECode ScanSettings::GetScanResultType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mScanResultType;
    return NOERROR;
}

ECode ScanSettings::GetReportDelayMillis(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mReportDelayMillis;
    return NOERROR;
}

ECode ScanSettings::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mScanMode);
    dest->WriteInt32(mCallbackType);
    dest->WriteInt32(mScanResultType);
    dest->WriteInt64(mReportDelayMillis);
    return NOERROR;
}

// @Override
CARAPI ScanSettings::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    in->ReadInt32(&mScanMode);
    in->ReadInt32(&mCallbackType);
    in->ReadInt32(&mScanResultType);
    in->ReadInt64(&mReportDelayMillis);
    return NOERROR;
}

ECode ScanSettings::constructor()
{
    return NOERROR;
}

ECode ScanSettings::constructor(
    /* [in] */ Int32 scanMode,
    /* [in] */ Int32 callbackType,
    /* [in] */ Int32 scanResultType,
    /* [in] */ Int64 reportDelayMillis)
{
    mScanMode = scanMode;
    mCallbackType = callbackType;
    mScanResultType = scanResultType;
    mReportDelayMillis = reportDelayMillis;
    return NOERROR;
}

ECode ScanSettings::constructor(
    /* [in] */ IParcel* in)
{
    in->ReadInt32(&mScanMode);
    in->ReadInt32(&mCallbackType);
    in->ReadInt32(&mScanResultType);
    in->ReadInt64(&mReportDelayMillis);
    return NOERROR;
}

} // namespace LE
} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
