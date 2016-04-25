
#include "elastos/droid/hardware/hdmi/CHdmiTimerRecordSourcesHelper.h"
#include "elastos/droid/hardware/hdmi/HdmiTimerRecordSources.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Hdmi {

CAR_INTERFACE_IMPL(CHdmiTimerRecordSourcesHelper, Singleton, IHdmiTimerRecordSourcesHelper)

CAR_SINGLETON_IMPL(CHdmiTimerRecordSourcesHelper)

ECode CHdmiTimerRecordSourcesHelper::OfDigitalSource(
    /* [in] */ IHdmiTimerRecordSourcesTimerInfo* timerInfo,
    /* [in] */ IHdmiRecordSourcesDigitalServiceSource* source,
    /* [out] */ ITimerRecordSource** result)
{
    VALIDATE_NOT_NULL(result);

    return HdmiTimerRecordSources::OfDigitalSource(timerInfo, source, result);
}

ECode CHdmiTimerRecordSourcesHelper::OfAnalogueSource(
    /* [in] */ IHdmiTimerRecordSourcesTimerInfo* timerInfo,
    /* [in] */ IHdmiRecordSourcesAnalogueServiceSource* source,
    /* [out] */ ITimerRecordSource** result)
{
    VALIDATE_NOT_NULL(result);

    return HdmiTimerRecordSources::OfAnalogueSource(timerInfo, source, result);
}

ECode CHdmiTimerRecordSourcesHelper::OfExternalPlug(
    /* [in] */ IHdmiTimerRecordSourcesTimerInfo* timerInfo,
    /* [in] */ IHdmiRecordSourcesExternalPlugData* source,
    /* [out] */ ITimerRecordSource** result)
{
    VALIDATE_NOT_NULL(result);

    return HdmiTimerRecordSources::OfExternalPlug(timerInfo, source, result);
}

ECode CHdmiTimerRecordSourcesHelper::OfExternalPhysicalAddress(
    /* [in] */ IHdmiTimerRecordSourcesTimerInfo*  timerInfo,
    /* [in] */ IHdmiRecordSourcesExternalPhysicalAddress* source,
    /* [out] */ ITimerRecordSource** result)
{
    VALIDATE_NOT_NULL(result);

    return HdmiTimerRecordSources::OfExternalPhysicalAddress(timerInfo, source, result);
}

ECode CHdmiTimerRecordSourcesHelper::TimeOf(
    /* [in] */ Int32 hour,
    /* [in] */ Int32 minute,
    /* [out] */ IHdmiTimerRecordSourcesTime** time)
{
    VALIDATE_NOT_NULL(time);

    return HdmiTimerRecordSources::TimeOf(hour, minute, time);
}

ECode CHdmiTimerRecordSourcesHelper::DurationOf(
    /* [in] */ Int32 hour,
    /* [in] */ Int32 minute,
    /* [out] */ IHdmiTimerRecordSourcesDuration** result)
{
    VALIDATE_NOT_NULL(result);

    return HdmiTimerRecordSources::DurationOf(hour, minute, result);
}

ECode CHdmiTimerRecordSourcesHelper::TimerInfoOf(
    /* [in] */ Int32 dayOfMonth,
    /* [in] */ Int32 monthOfYear,
    /* [in] */ IHdmiTimerRecordSourcesTime* startTime,
    /* [in] */ IHdmiTimerRecordSourcesDuration* duration,
    /* [in] */ Int32 recordingSequence,
    /* [out] */ IHdmiTimerRecordSourcesTimerInfo** result)
{
    VALIDATE_NOT_NULL(result);

    return HdmiTimerRecordSources::TimerInfoOf(dayOfMonth, monthOfYear, startTime, duration,
            recordingSequence, result);
}

ECode CHdmiTimerRecordSourcesHelper::CheckTimerRecordSource(
    /* [in] */ Int32 sourcetype,
    /* [in] */ ArrayOf<Byte>* recordSource,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    return HdmiTimerRecordSources::CheckTimerRecordSource(sourcetype, recordSource, result);
}

} // namespace Hdmi
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
