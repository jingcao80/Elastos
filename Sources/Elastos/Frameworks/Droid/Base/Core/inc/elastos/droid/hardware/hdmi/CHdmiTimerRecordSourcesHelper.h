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

#ifndef __ELASTOS_DROID_HARDWARE_HDMI_CHDMITIMERRECORDSOURCESHELPER_H__
#define __ELASTOS_DROID_HARDWARE_HDMI_CHDMITIMERRECORDSOURCESHELPER_H__

#include "_Elastos_Droid_Hardware_Hdmi_CHdmiTimerRecordSourcesHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Hdmi {

CarClass(CHdmiTimerRecordSourcesHelper)
    , public Singleton
    , public IHdmiTimerRecordSourcesHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI OfDigitalSource(
        /* [in] */ IHdmiTimerRecordSourcesTimerInfo* timerInfo,
        /* [in] */ IHdmiRecordSourcesDigitalServiceSource* source,
        /* [out] */ ITimerRecordSource** result);

    CARAPI OfAnalogueSource(
        /* [in] */ IHdmiTimerRecordSourcesTimerInfo* timerInfo,
        /* [in] */ IHdmiRecordSourcesAnalogueServiceSource* source,
        /* [out] */ ITimerRecordSource** result);

    CARAPI OfExternalPlug(
        /* [in] */ IHdmiTimerRecordSourcesTimerInfo* timerInfo,
        /* [in] */ IHdmiRecordSourcesExternalPlugData* source,
        /* [out] */ ITimerRecordSource** result);

    CARAPI OfExternalPhysicalAddress(
        /* [in] */ IHdmiTimerRecordSourcesTimerInfo*  timerInfo,
        /* [in] */ IHdmiRecordSourcesExternalPhysicalAddress* source,
        /* [out] */ ITimerRecordSource** result);

    CARAPI TimeOf(
        /* [in] */ Int32 hour,
        /* [in] */ Int32 minute,
        /* [out] */ IHdmiTimerRecordSourcesTime** time);

    CARAPI DurationOf(
        /* [in] */ Int32 hour,
        /* [in] */ Int32 minute,
        /* [out] */ IHdmiTimerRecordSourcesDuration** result);

    CARAPI TimerInfoOf(
        /* [in] */ Int32 dayOfMonth,
        /* [in] */ Int32 monthOfYear,
        /* [in] */ IHdmiTimerRecordSourcesTime* startTime,
        /* [in] */ IHdmiTimerRecordSourcesDuration* duration,
        /* [in] */ Int32 recordingSequence,
        /* [out] */ IHdmiTimerRecordSourcesTimerInfo** result);

    CARAPI CheckTimerRecordSource(
        /* [in] */ Int32 sourcetype,
        /* [in] */ ArrayOf<Byte>* recordSource,
        /* [out] */ Boolean* result);
};

} // namespace Hdmi
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_HDMI_CHDMITIMERRECORDSOURCESHELPER_H__
