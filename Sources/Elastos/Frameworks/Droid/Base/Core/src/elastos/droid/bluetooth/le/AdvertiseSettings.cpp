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

#include "elastos/droid/bluetooth/le/AdvertiseSettings.h"
#include "elastos/droid/bluetooth/le/CAdvertiseSettings.h"
#include "elastos/core/StringUtils.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Bluetooth {
namespace LE {

//=====================================================================
//                      AdvertiseSettings::Builder
//=====================================================================
CAR_INTERFACE_IMPL(AdvertiseSettings::Builder, Object, IAdvertiseSettingsBuilder)

AdvertiseSettings::Builder::Builder()
{
}

ECode AdvertiseSettings::Builder::SetAdvertiseMode(
    /* [in] */ Int32 advertiseMode)
{
    if (advertiseMode < ADVERTISE_MODE_LOW_POWER
            || advertiseMode > ADVERTISE_MODE_LOW_LATENCY) {
        //throw new IllegalArgumentException("unknown mode " + advertiseMode);
        Slogger::E("AdvertiseSettings::Builder::SetAdvertiseMode", "unknown mode %d", advertiseMode);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mMode = advertiseMode;
    //return this;
    return NOERROR;
}

ECode AdvertiseSettings::Builder::SetTxPowerLevel(
    /* [in] */ Int32 txPowerLevel)
{
    if (txPowerLevel < ADVERTISE_TX_POWER_ULTRA_LOW
            || txPowerLevel > ADVERTISE_TX_POWER_HIGH) {
        //throw new IllegalArgumentException("unknown tx power level " + txPowerLevel);
        Slogger::E("AdvertiseSettings::Builder::SetTxPowerLevel", "unknown tx power level %d", txPowerLevel);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mTxPowerLevel = txPowerLevel;
    //return this;
    return NOERROR;
}

ECode AdvertiseSettings::Builder::SetConnectable(
    /* [in] */ Boolean connectable)
{
    mConnectable = connectable;
    //return this;
    return NOERROR;
}

ECode AdvertiseSettings::Builder::SetTimeout(
    /* [in] */ Int32 timeoutMillis)
{
    if (timeoutMillis < 0 || timeoutMillis > LIMITED_ADVERTISING_MAX_MILLIS) {
        //throw new IllegalArgumentException("timeoutMillis invalid (must be 0-"
        //                    + LIMITED_ADVERTISING_MAX_MILLIS + " milliseconds)");
        Slogger::E("AdvertiseSettings::Builder::SetTimeout",
                "timeoutMillis invalid (must be 0-%d milliseconds)", LIMITED_ADVERTISING_MAX_MILLIS);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mTimeoutMillis = timeoutMillis;
    //return this;
    return NOERROR;
}

ECode AdvertiseSettings::Builder::Build(
    /* [out] */ IAdvertiseSettings** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    CAdvertiseSettings::New(mMode, mTxPowerLevel, mConnectable, mTimeoutMillis, result);
    return NOERROR;
}

//=====================================================================
//                          AdvertiseSettings
//=====================================================================
const Int32 AdvertiseSettings::LIMITED_ADVERTISING_MAX_MILLIS;

CAR_INTERFACE_IMPL_2(AdvertiseSettings, Object, IAdvertiseSettings, IParcelable);

AdvertiseSettings::AdvertiseSettings()
{
}

ECode AdvertiseSettings::GetMode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAdvertiseMode;
    return NOERROR;
}

ECode AdvertiseSettings::GetTxPowerLevel(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAdvertiseTxPowerLevel;
    return NOERROR;
}

ECode AdvertiseSettings::IsConnectable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAdvertiseConnectable;
    return NOERROR;
}

ECode AdvertiseSettings::GetTimeout(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAdvertiseTimeoutMillis;
    return NOERROR;
}

CARAPI AdvertiseSettings::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = String("Settings [mAdvertiseMode=") + StringUtils::ToString(mAdvertiseMode)
        + String(", mAdvertiseTxPowerLevel=") + StringUtils::ToString(mAdvertiseTxPowerLevel)
        + String(", mAdvertiseConnectable=") + StringUtils::ToString(mAdvertiseConnectable)
        + String(", mAdvertiseTimeoutMillis=") + StringUtils::ToString(mAdvertiseTimeoutMillis) + String("]");
    return NOERROR;
}

ECode AdvertiseSettings::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);
    dest->WriteInt32(mAdvertiseMode);
    dest->WriteInt32(mAdvertiseTxPowerLevel);
    dest->WriteInt32(mAdvertiseConnectable ? 1 : 0);
    dest->WriteInt32(mAdvertiseTimeoutMillis);
    return NOERROR;
}

ECode AdvertiseSettings::ReadFromParcel(
            /* [in] */ IParcel* in)
{
    //in->ReadInt32(&mAdvertiseMode);
    //in->ReadInt32(&mAdvertiseTxPowerLevel);
    //Int32 connectable;
    //in->ReadInt32(&connectable);
    //mAdvertiseConnectable = connectable != 0 ? TRUE : FALSE;
    //in->ReadInt32(&mAdvertiseTimeoutMillis);
    return constructor(in);
}

ECode AdvertiseSettings::constructor()
{
    return NOERROR;
}

ECode AdvertiseSettings::constructor(
    /* [in] */ Int32 advertiseMode,
    /* [in] */ Int32 advertiseTxPowerLevel,
    /* [in] */ Boolean advertiseConnectable,
    /* [in] */ Int32 advertiseTimeout)
{
    mAdvertiseMode = advertiseMode;
    mAdvertiseTxPowerLevel = advertiseTxPowerLevel;
    mAdvertiseConnectable = advertiseConnectable;
    mAdvertiseTimeoutMillis = advertiseTimeout;
    return NOERROR;
}

ECode AdvertiseSettings::constructor(
    /* [in] */ IParcel* in)
{
    in->ReadInt32(&mAdvertiseMode);
    in->ReadInt32(&mAdvertiseTxPowerLevel);
    Int32 connectable;
    in->ReadInt32(&connectable);
    mAdvertiseConnectable = connectable != 0 ? TRUE : FALSE;
    in->ReadInt32(&mAdvertiseTimeoutMillis);
    return NOERROR;
}

} // namespace LE
} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
