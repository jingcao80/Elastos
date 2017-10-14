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

#include "elastos/droid/hardware/hdmi/HdmiTimerRecordSources.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Hdmi {

CAR_INTERFACE_IMPL(HdmiTimerRecordSources::TimeUnit, Object,
        IHdmiTimerRecordSourcesTimeUnit)

HdmiTimerRecordSources::TimeUnit::TimeUnit(
    /* [in] */ Int32 hour,
    /* [in] */ Int32 minute)
    : mHour(hour)
    , mMinute(minute)
{
}

ECode HdmiTimerRecordSources::TimeUnit::ToByteArray(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 index,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    (*data)[index] = ToBcdByte(mHour);
    (*data)[index + 1] = ToBcdByte(mMinute);
    *result = 2;
    return NOERROR;
}

Byte HdmiTimerRecordSources::TimeUnit::ToBcdByte(
    /* [in] */ Int32 value)
{
    Int32 digitOfTen = (value / 10) % 10;
    Int32 digitOfOne = value % 10;
    return (Byte)((digitOfTen << 4) | digitOfOne);
}

CAR_INTERFACE_IMPL(HdmiTimerRecordSources::Time, TimeUnit, IHdmiTimerRecordSourcesTime)

HdmiTimerRecordSources::Time::Time(
    /* [in] */ Int32 hour,
    /* [in] */ Int32 minute)
    : TimeUnit(hour, minute)
{
}

CAR_INTERFACE_IMPL(HdmiTimerRecordSources::Duration, TimeUnit, IHdmiTimerRecordSourcesDuration)

HdmiTimerRecordSources::Duration::Duration(
    /* [in] */ Int32 hour,
    /* [in] */ Int32 minute)
    : TimeUnit(hour, minute)
{
}

const Int32 HdmiTimerRecordSources::TimerInfo::DAY_OF_MONTH_SIZE = 1;
const Int32 HdmiTimerRecordSources::TimerInfo::MONTH_OF_YEAR_SIZE = 1;
const Int32 HdmiTimerRecordSources::TimerInfo::START_TIME_SIZE = 2;
const Int32 HdmiTimerRecordSources::TimerInfo::DURATION_SIZE = 2;
const Int32 HdmiTimerRecordSources::TimerInfo::RECORDING_SEQUENCE_SIZE = 1;
const Int32 HdmiTimerRecordSources::TimerInfo::BASIC_INFO_SIZE = DAY_OF_MONTH_SIZE + MONTH_OF_YEAR_SIZE
        + START_TIME_SIZE + DURATION_SIZE + RECORDING_SEQUENCE_SIZE;

CAR_INTERFACE_IMPL(HdmiTimerRecordSources::TimerInfo, Object, IHdmiTimerRecordSourcesTimerInfo)

HdmiTimerRecordSources::TimerInfo::TimerInfo(
    /* [in] */ Int32 dayOfMonth,
    /* [in] */ Int32 monthOfYear,
    /* [in] */ IHdmiTimerRecordSourcesTime* startTime,
    /* [in] */ IHdmiTimerRecordSourcesDuration* duration,
    /* [in] */ Int32 recordingSequence)
    : mDayOfMonth(dayOfMonth)
    , mMonthOfYear(monthOfYear)
    , mStartTime(startTime)
    , mDuration(duration)
    , mRecordingSequence(recordingSequence)
{
}

ECode HdmiTimerRecordSources::TimerInfo::ToByteArray(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 index,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    // [Day of Month]
    (*data)[index] = (Byte)mDayOfMonth;
    index += DAY_OF_MONTH_SIZE;
    // [Month of Year]
    (*data)[index] = (Byte)mMonthOfYear;
    index += MONTH_OF_YEAR_SIZE;
    // [Start Time]
    Int32 res;
    IHdmiTimerRecordSourcesTimeUnit::Probe(mStartTime)->ToByteArray(data, index, &res);
    index += res;
    IHdmiTimerRecordSourcesTimeUnit::Probe(mDuration)->ToByteArray(data, index, &res);
    index += res;
    // [Duration]
    // [Recording Sequence]
    (*data)[index] = (Byte)mRecordingSequence;
    return GetDataSize(result);
}

ECode HdmiTimerRecordSources::TimerInfo::GetDataSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);

    *size = BASIC_INFO_SIZE;
    return NOERROR;
}

CAR_INTERFACE_IMPL(HdmiTimerRecordSources::TimerRecordSource, Object, ITimerRecordSource)

HdmiTimerRecordSources::TimerRecordSource::TimerRecordSource(
    /* [in] */ IHdmiTimerRecordSourcesTimerInfo* timerInfo,
    /* [in] */ IRecordSource* recordSource)
    : mRecordSource(recordSource)
    , mTimerInfo(timerInfo)
{
}

CARAPI HdmiTimerRecordSources::TimerRecordSource::GetDataSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);

    Int32 res1;
    mTimerInfo->GetDataSize(&res1);
    Int32 res2;
    mRecordSource->GetDataSize(FALSE, &res2);
    *size = res1 + res2;
    return NOERROR;
}

CARAPI HdmiTimerRecordSources::TimerRecordSource::ToByteArray(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 index,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    // Basic infos including [Day of Month] [Month of Year] [Start Time] [Duration]
    // [Recording Sequence]
    Int32 res;
    mTimerInfo->ToByteArray(data, index, &res);
    index += res;
    // [Record Source]
    mRecordSource->ToByteArray(FALSE, data, index, &res);
    return GetDataSize(result);
}

HdmiTimerRecordSources::ExternalSourceDecorator::ExternalSourceDecorator(
    /* [in] */ IRecordSource* recordSource,
    /* [in] */ Int32 externalSourceSpecifier)
    : mRecordSource(recordSource)
    , mExternalSourceSpecifier(externalSourceSpecifier)
{
    // External source has one byte field for [External Source Specifier].
    Int32 size;
    recordSource->GetDataSize(FALSE, &size);
    HdmiRecordSources::RecordSource* _recordSource = (HdmiRecordSources::RecordSource*)recordSource;
    HdmiRecordSources::RecordSource::constructor(_recordSource->mSourceType, size + 1);
}

ECode HdmiTimerRecordSources::ExternalSourceDecorator::ExtraParamToByteArray(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 index,
    /* [out] */ Int32* array)
{
    VALIDATE_NOT_NULL(array);

    (*data)[index] = (Byte)mExternalSourceSpecifier;
    Int32 tmp;
    mRecordSource->ToByteArray(FALSE, data, index + 1, &tmp);
    return GetDataSize(FALSE, array);
}

const String HdmiTimerRecordSources::TAG("HdmiTimerRecordSources");

const Int32 HdmiTimerRecordSources::RECORDING_SEQUENCE_REPEAT_MASK =
        (IHdmiTimerRecordSources::RECORDING_SEQUENCE_REPEAT_SUNDAY |
        IHdmiTimerRecordSources::RECORDING_SEQUENCE_REPEAT_MONDAY |
        IHdmiTimerRecordSources::RECORDING_SEQUENCE_REPEAT_TUESDAY |
        IHdmiTimerRecordSources::RECORDING_SEQUENCE_REPEAT_WEDNESDAY |
        IHdmiTimerRecordSources::RECORDING_SEQUENCE_REPEAT_THURSDAY |
        IHdmiTimerRecordSources::RECORDING_SEQUENCE_REPEAT_FRIDAY |
        IHdmiTimerRecordSources::RECORDING_SEQUENCE_REPEAT_SATUREDAY);

const Int32 HdmiTimerRecordSources::EXTERNAL_SOURCE_SPECIFIER_EXTERNAL_PLUG = 4;
const Int32 HdmiTimerRecordSources::EXTERNAL_SOURCE_SPECIFIER_EXTERNAL_PHYSICAL_ADDRESS = 5;

CAR_INTERFACE_IMPL(HdmiTimerRecordSources, Object, IHdmiTimerRecordSources)

ECode HdmiTimerRecordSources::OfDigitalSource(
    /* [in] */ IHdmiTimerRecordSourcesTimerInfo* timerInfo,
    /* [in] */ IHdmiRecordSourcesDigitalServiceSource* source,
    /* [out] */ ITimerRecordSource** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    FAIL_RETURN(CheckTimerRecordSourceInputs(timerInfo, IRecordSource::Probe(source)))
    AutoPtr<ITimerRecordSource> tmp = new TimerRecordSource(timerInfo, IRecordSource::Probe(source));
    *result = tmp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode HdmiTimerRecordSources::OfAnalogueSource(
    /* [in] */ IHdmiTimerRecordSourcesTimerInfo* timerInfo,
    /* [in] */ IHdmiRecordSourcesAnalogueServiceSource* source,
    /* [out] */ ITimerRecordSource** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    FAIL_RETURN(CheckTimerRecordSourceInputs(timerInfo, IRecordSource::Probe(source)))
    AutoPtr<ITimerRecordSource> tmp = new TimerRecordSource(timerInfo, IRecordSource::Probe(source));
    *result = tmp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode HdmiTimerRecordSources::OfExternalPlug(
    /* [in] */ IHdmiTimerRecordSourcesTimerInfo* timerInfo,
    /* [in] */ IHdmiRecordSourcesExternalPlugData* source,
    /* [out] */ ITimerRecordSource** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    FAIL_RETURN(CheckTimerRecordSourceInputs(timerInfo, IRecordSource::Probe(source)))
    AutoPtr<IRecordSource> decorator = new ExternalSourceDecorator(IRecordSource::Probe(source),
            EXTERNAL_SOURCE_SPECIFIER_EXTERNAL_PLUG);

    AutoPtr<ITimerRecordSource> tmp = new TimerRecordSource(timerInfo, decorator);
    *result = tmp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode HdmiTimerRecordSources::OfExternalPhysicalAddress(
    /* [in] */ IHdmiTimerRecordSourcesTimerInfo*  timerInfo,
    /* [in] */ IHdmiRecordSourcesExternalPhysicalAddress* source,
    /* [out] */ ITimerRecordSource** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    FAIL_RETURN(CheckTimerRecordSourceInputs(timerInfo, IRecordSource::Probe(source)))
    AutoPtr<IRecordSource> decorator = new ExternalSourceDecorator(IRecordSource::Probe(source),
                    EXTERNAL_SOURCE_SPECIFIER_EXTERNAL_PHYSICAL_ADDRESS);
    AutoPtr<ITimerRecordSource> tmp = new TimerRecordSource(timerInfo, decorator);
    *result = tmp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode HdmiTimerRecordSources::CheckTimerRecordSourceInputs(
    /* [in] */ IHdmiTimerRecordSourcesTimerInfo* timerInfo,
    /* [in] */ IRecordSource* source)
{
    if (timerInfo == NULL) {
        Logger::W(TAG, "TimerInfo should not be null.");
        //throw new IllegalArgumentException("TimerInfo should not be null.");
        Logger::E(TAG, "TimerInfo should not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (source == NULL) {
        Logger::W(TAG, "source should not be null.");
        //throw new IllegalArgumentException("source should not be null.");
        Logger::E(TAG, "TimerInfo should not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode HdmiTimerRecordSources::TimeOf(
    /* [in] */ Int32 hour,
    /* [in] */ Int32 minute,
    /* [out] */ IHdmiTimerRecordSourcesTime** _time)
{
    VALIDATE_NOT_NULL(_time);
    *_time = NULL;

    FAIL_RETURN(CheckTimeValue(hour, minute))
    AutoPtr<IHdmiTimerRecordSourcesTime> tmp = new Time(hour, minute);
    *_time = tmp;
    REFCOUNT_ADD(*_time);
    return NOERROR;
}

ECode HdmiTimerRecordSources::CheckTimeValue(
    /* [in] */ Int32 hour,
    /* [in] */ Int32 minute)
{
    if (hour < 0 || hour > 23) {
        //throw new IllegalArgumentException("Hour should be in rage of [0, 23]:" + hour);
        Logger::E(TAG, "Hour should be in rage of [0, 23]:%d", hour);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (minute < 0 || minute > 59) {
        //throw new IllegalArgumentException("Minute should be in rage of [0, 59]:" + minute);
        Logger::E(TAG, "Minute should be in rage of [0, 59]:%d", minute);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode HdmiTimerRecordSources::DurationOf(
    /* [in] */ Int32 hour,
    /* [in] */ Int32 minute,
    /* [out] */ IHdmiTimerRecordSourcesDuration** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    FAIL_RETURN(CheckDurationValue(hour, minute))
    AutoPtr<IHdmiTimerRecordSourcesDuration> tmp = new Duration(hour, minute);
    *result = tmp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode HdmiTimerRecordSources::CheckDurationValue(
    /* [in] */ Int32 hour,
    /* [in] */ Int32 minute)
{
    if (hour < 0 || hour > 99) {
        //throw new IllegalArgumentException("Hour should be in rage of [0, 99]:" + hour);
        Logger::E(TAG, "Hour should be in rage of [0, 23]:%d", hour);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (minute < 0 || minute > 59) {
        //throw new IllegalArgumentException("minute should be in rage of [0, 59]:" + minute);
        Logger::E(TAG, "minute should be in rage of [0, 59]:%d", minute);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode HdmiTimerRecordSources::TimerInfoOf(
    /* [in] */ Int32 dayOfMonth,
    /* [in] */ Int32 monthOfYear,
    /* [in] */ IHdmiTimerRecordSourcesTime* startTime,
    /* [in] */ IHdmiTimerRecordSourcesDuration* duration,
    /* [in] */ Int32 recordingSequence,
    /* [out] */ IHdmiTimerRecordSourcesTimerInfo** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    if (dayOfMonth < 0 || dayOfMonth > 31) {
        // throw new IllegalArgumentException(
        //         "Day of month should be in range of [0, 31]:" + dayOfMonth);
        Logger::E(TAG, "Day of month should be in range of [0, 31]:%d", dayOfMonth);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (monthOfYear < 1 || monthOfYear > 12) {
        // throw new IllegalArgumentException(
        //         "Month of year should be in range of [1, 12]:" + monthOfYear);
        Logger::E(TAG, "Month of year should be in range of [1, 12]:%d", monthOfYear);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    HdmiTimerRecordSources::Time* _startTime = (HdmiTimerRecordSources::Time*)startTime;
    FAIL_RETURN(CheckTimeValue(_startTime->mHour, _startTime->mMinute))
    HdmiTimerRecordSources::Duration* _duration = (HdmiTimerRecordSources::Duration*)duration;
    FAIL_RETURN(CheckDurationValue(_duration->mHour, _duration->mMinute))
    // Recording sequence should use least 7 bits or no bits.
    if ((recordingSequence != 0)
            && ((recordingSequence & ~RECORDING_SEQUENCE_REPEAT_MASK) != 0)) {
        // throw new IllegalArgumentException(
        //         "Invalid reecording sequence value:" + recordingSequence);
        Logger::E(TAG, "Invalid reecording sequence value:%d", recordingSequence);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IHdmiTimerRecordSourcesTimerInfo> tmp = new TimerInfo(dayOfMonth,
                monthOfYear, startTime, duration, recordingSequence);
    *result = tmp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode HdmiTimerRecordSources::CheckTimerRecordSource(
    /* [in] */ Int32 sourcetype,
    /* [in] */ ArrayOf<Byte>* recordSource,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    Int32 recordSourceSize = recordSource->GetLength() - TimerInfo::BASIC_INFO_SIZE;
    switch (sourcetype) {
        case IHdmiControlManager::TIMER_RECORDING_TYPE_DIGITAL:
            *result = IHdmiRecordSourcesDigitalServiceSource::EXTRA_DATA_SIZE == recordSourceSize;
            return NOERROR;
        case IHdmiControlManager::TIMER_RECORDING_TYPE_ANALOGUE:
            *result = IHdmiRecordSourcesAnalogueServiceSource::EXTRA_DATA_SIZE == recordSourceSize;
            return NOERROR;
        case IHdmiControlManager::TIMER_RECORDING_TYPE_EXTERNAL:
        {
            Int32 specifier = (*recordSource)[TimerInfo::BASIC_INFO_SIZE];
            if (specifier == EXTERNAL_SOURCE_SPECIFIER_EXTERNAL_PLUG) {
                // One byte for specifier.
                *result = IHdmiRecordSourcesExternalPlugData::EXTRA_DATA_SIZE + 1 == recordSourceSize;
                return NOERROR;
            }
            else if (specifier == EXTERNAL_SOURCE_SPECIFIER_EXTERNAL_PHYSICAL_ADDRESS) {
                // One byte for specifier.
                *result = IHdmiRecordSourcesExternalPhysicalAddress::EXTRA_DATA_SIZE + 1 == recordSourceSize;
                return NOERROR;
            }
            else {
                // Invalid specifier.
                *result = FALSE;
                return NOERROR;
            }
        }
        default:
            *result = FALSE;
            return NOERROR;
    }
    return NOERROR;
}

} // namespace Hdmi
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos
