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

#ifndef __ELASTOS_DROID_TELEPHONY_CCELLSIGNALSTRENGTHCDMA_H__
#define __ELASTOS_DROID_TELEPHONY_CCELLSIGNALSTRENGTHCDMA_H__

#include "_Elastos_Droid_Telephony_CCellSignalStrengthCdma.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/telephony/CellSignalStrength.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CCellSignalStrengthCdma)
    , public CellSignalStrength
    , public ICellSignalStrengthCdma
    , public IParcelable
{
public:
    CCellSignalStrengthCdma();

    virtual ~CCellSignalStrengthCdma();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 cdmaDbm,
        /* [in] */ Int32 cdmaEcio,
        /* [in] */ Int32 evdoDbm,
        /* [in] */ Int32 evdoEcio,
        /* [in] */ Int32 evdoSnr);

    CARAPI constructor(
        /* [in] */ ICellSignalStrengthCdma* css);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI SetDefaultValues();

    CARAPI GetLevel(
        /* [out] */ Int32* level);

    CARAPI GetAsuLevel(
        /* [out] */ Int32* asuLevel);

    CARAPI GetDbm(
        /* [out] */ Int32* dbm);

    CARAPI Copy(
        /* [out] */ ICellSignalStrength** css);

    CARAPI Copy(
        /* [out] */ ICellSignalStrengthCdma** css);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* res);

    CARAPI Initialize(
        /* [in] */ Int32 cdmaDbm,
        /* [in] */ Int32 cdmaEcio,
        /* [in] */ Int32 evdoDbm,
        /* [in] */ Int32 evdoEcio,
        /* [in] */ Int32 evdoSnr);

    CARAPI GetCdmaLevel(
        /* [out] */ Int32* cdmaLevel);

    CARAPI GetEvdoLevel(
        /* [out] */ Int32* evdoLevel);

    CARAPI GetCdmaDbm(
        /* [out] */ Int32* cdmaDbm);

    CARAPI SetCdmaDbm(
        /* [in] */ Int32 cdmaDbm);

    CARAPI GetCdmaEcio(
        /* [out] */ Int32* cdmaEcio);

    CARAPI SetCdmaEcio(
        /* [in] */ Int32 cdmaEcio);

    CARAPI GetEvdoDbm(
        /* [out] */ Int32* evdoDbm);

    CARAPI SetEvdoDbm(
        /* [in] */ Int32 evdoDbm);

    CARAPI GetEvdoEcio(
        /* [out] */ Int32* evdoEcio);

    CARAPI SetEvdoEcio(
        /* [in] */ Int32 evdoEcio);

    CARAPI GetEvdoSnr(
        /* [out] */ Int32* evdoSnr);

    CARAPI SetEvdoSnr(
        /* [in] */ Int32 evdoSnr);

    CARAPI ToString(
        /* [out] */ String* str);

protected:
    CARAPI CopyFrom(
        /* [in] */ ICellSignalStrengthCdma* css);

private:

    static CARAPI Log(
        /* [in] */ const String& s);

    static const String TAG;
    static const Boolean DBG;

    Int32 mCdmaDbm;   // This value is the RSSI value
    Int32 mCdmaEcio;  // This value is the Ec/Io
    Int32 mEvdoDbm;   // This value is the EVDO RSSI value
    Int32 mEvdoEcio;  // This value is the EVDO Ec/Io
    Int32 mEvdoSnr;   // Valid values are 0-8.  8 is the highest signal to noise ratio
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CCELLSIGNALSTRENGTHCDMA_H__
