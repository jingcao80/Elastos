
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
