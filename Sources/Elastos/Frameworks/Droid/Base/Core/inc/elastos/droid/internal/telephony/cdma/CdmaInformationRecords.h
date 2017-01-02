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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CDMAINFORMATIONRECORDS_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CDMAINFORMATIONRECORDS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

class CdmaInformationRecords
    : public Object
    , public ICdmaInformationRecords
{
public:
    /**
      * Signal Information record from 3GPP2 C.S005 3.7.5.5
      */
    class CdmaSignalInfoRec
        : public Object
    {
    public:
        CdmaSignalInfoRec();

        CdmaSignalInfoRec(
            /* [in] */ Int32 isPresent,
            /* [in] */ Int32 signalType,
            /* [in] */ Int32 alertPitch,
            /* [in] */ Int32 signal);

        // @Override
        CARAPI ToString(
            /* [out] */ String* result);

    public:
        Boolean isPresent;
        /* non-zero if signal information record is present */
        Int32 signalType;
        Int32 alertPitch;
        Int32 signal;
    };

    class CdmaDisplayInfoRec
        : public Object
    {
    public:
        CdmaDisplayInfoRec(
            /* [in] */ Int32 id,
            /* [in] */ const String& alpha);

        // @Override
        CARAPI ToString(
            /* [out] */ String* result);

    public:
        Int32 id;
        String alpha;
    };

    class CdmaNumberInfoRec
        : public Object
    {
    public:
        CdmaNumberInfoRec(
            /* [in] */ Int32 id,
            /* [in] */ const String& number,
            /* [in] */ Int32 numberType,
            /* [in] */ Int32 numberPlan,
            /* [in] */ Int32 pi,
            /* [in] */ Int32 si);

        // @Override
        CARAPI ToString(
            /* [out] */ String* result);

    public:
        Int32 id;
        String number;
        Byte numberType;
        Byte numberPlan;
        Byte pi;
        Byte si;
    };

    class CdmaRedirectingNumberInfoRec
        : public Object
    {
    public:
        CdmaRedirectingNumberInfoRec(
            /* [in] */ const String& number,
            /* [in] */ Int32 numberType,
            /* [in] */ Int32 numberPlan,
            /* [in] */ Int32 pi,
            /* [in] */ Int32 si,
            /* [in] */ Int32 reason);

        // @Override
        CARAPI ToString(
            /* [out] */ String* result);

    public:
        static const Int32 REASON_UNKNOWN = 0;
        static const Int32 REASON_CALL_FORWARDING_BUSY = 1;
        static const Int32 REASON_CALL_FORWARDING_NO_REPLY = 2;
        static const Int32 REASON_CALLED_DTE_OUT_OF_ORDER = 9;
        static const Int32 REASON_CALL_FORWARDING_BY_THE_CALLED_DTE = 10;
        static const Int32 REASON_CALL_FORWARDING_UNCONDITIONAL = 15;
        AutoPtr<CdmaNumberInfoRec> numberInfoRec;
        Int32 redirectingReason;
    };

    class CdmaLineControlInfoRec
        : public Object
    {
    public:
        CdmaLineControlInfoRec(
            /* [in] */ Int32 lineCtrlPolarityIncluded,
            /* [in] */ Int32 lineCtrlToggle,
            /* [in] */ Int32 lineCtrlReverse,
            /* [in] */ Int32 lineCtrlPowerDenial);

        // @Override
        CARAPI ToString(
            /* [out] */ String* result);

    public:
        Byte lineCtrlPolarityIncluded;
        Byte lineCtrlToggle;
        Byte lineCtrlReverse;
        Byte lineCtrlPowerDenial;
    };

    class CdmaT53ClirInfoRec
        : public Object
    {
    public:
        CdmaT53ClirInfoRec(
            /* [in] */ Int32 cause);

        // @Override
        CARAPI ToString(
            /* [out] */ String* result);

    public:
        Byte cause;
    };

    class CdmaT53AudioControlInfoRec
        : public Object
    {
    public:
        CdmaT53AudioControlInfoRec(
            /* [in] */ Int32 uplink,
            /* [in] */ Int32 downlink);

        // @Override
        CARAPI ToString(
            /* [out] */ String* result);

    public:
        Byte uplink;
        Byte downlink;
    };

public:
    CAR_INTERFACE_DECL();

    CdmaInformationRecords();

    CARAPI constructor(
        /* [in] */ IParcel* p);

    static CARAPI_(String) IdToString(
        /* [in] */ Int32 id);

public:
    AutoPtr<IObject> record;
};

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CDMAINFORMATIONRECORDS_H__
