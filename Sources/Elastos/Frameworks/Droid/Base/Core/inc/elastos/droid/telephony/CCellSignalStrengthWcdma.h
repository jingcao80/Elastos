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

#ifndef __ELASTOS_DROID_TELEPHONY_CCELLSIGNALSTRENGTHWCDMA_H__
#define __ELASTOS_DROID_TELEPHONY_CCELLSIGNALSTRENGTHWCDMA_H__

#include "_Elastos_Droid_Telephony_CCellSignalStrengthWcdma.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/telephony/CellSignalStrength.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CCellSignalStrengthWcdma)
    , public CellSignalStrength
    , public ICellSignalStrengthWcdma
    , public IParcelable
{
public:
    CCellSignalStrengthWcdma();

    virtual ~CCellSignalStrengthWcdma();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 ss,
        /* [in] */ Int32 ber);

    CARAPI constructor(
        /* [in] */ ICellSignalStrengthWcdma* css);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI Initialize(
        /* [in] */ Int32 ss,
        /* [in] */ Int32 ber);

    CARAPI Copy(
        /* [out] */ ICellSignalStrength** css);

    CARAPI SetDefaultValues();

    CARAPI GetLevel(
        /* [out] */ Int32* level);

    CARAPI GetAsuLevel(
        /* [out] */ Int32* asuLevel);

    CARAPI GetDbm(
        /* [out] */ Int32* dbm);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* res);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI GetSignalStrength(
        /* [out] */ Int32* signalStrength);

    CARAPI SetSignalStrength(
        /* [in] */ Int32 signalStrength);

    CARAPI GetBitErrorRate(
        /* [out] */ Int32* bitErrorRate);

    CARAPI SetBitErrorRate(
        /* [in] */ Int32 bitErrorRate);

protected:

    CARAPI CopyFrom(
        /* [in] */ ICellSignalStrengthWcdma* css);

private:

    static CARAPI Log(
        /* [in] */ const String& s);

    static const String TAG;
    static const Boolean DBG;

    static const Int32 WCDMA_SIGNAL_STRENGTH_GREAT;
    static const Int32 WCDMA_SIGNAL_STRENGTH_GOOD;
    static const Int32 WCDMA_SIGNAL_STRENGTH_MODERATE;

    Int32 mSignalStrength; // Valid values are (0-31, 99) as defined in TS 27.007 8.5
    Int32 mBitErrorRate;   // bit error rate (0-7, 99) as defined in TS 27.007 8.5
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CCELLSIGNALSTRENGTHWCDMA_H__
