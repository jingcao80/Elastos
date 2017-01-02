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

#ifndef __ELASTOS_DROID_HARDWARE_HDMI_HDMITIMERRECODSOURCES_H__
#define __ELASTOS_DROID_HARDWARE_HDMI_HDMITIMERRECODSOURCES_H__

#include "elastos/droid/hardware/hdmi/HdmiRecordSources.h"
#include "Elastos.Droid.Hardware.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Hdmi {

class HdmiTimerRecordSources
    : public Object
    , public IHdmiTimerRecordSources
{
private:
    class TimeUnit
        : public Object
        , public IHdmiTimerRecordSourcesTimeUnit
    {
    public:
        CAR_INTERFACE_DECL()

        TimeUnit(
            /* [in] */ Int32 hour,
            /* [in] */ Int32 minute);

        CARAPI ToByteArray(
            /* [in] */ ArrayOf<Byte>* data,
            /* [in] */ Int32 index,
            /* [out] */ Int32* result);

        static CARAPI_(Byte) ToBcdByte(
            /* [in] */ Int32 value);

    protected:
        Int32 mHour;
        Int32 mMinute;
    };

public:
    class Time
        : public TimeUnit
        , public IHdmiTimerRecordSourcesTime
    {
    public:
        CAR_INTERFACE_DECL()

    private:
        friend class HdmiTimerRecordSources;

        Time(
            /* [in] */ Int32 hour,
            /* [in] */ Int32 minute);
    };

    class Duration
        : public TimeUnit
        , public IHdmiTimerRecordSourcesDuration
    {
    public:
        CAR_INTERFACE_DECL()

    private:
        friend class HdmiTimerRecordSources;

        Duration(
            /* [in] */ Int32 hour,
            /* [in] */ Int32 minute);
    };

    class TimerInfo
        : public Object
        , public IHdmiTimerRecordSourcesTimerInfo
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI ToByteArray(
            /* [in] */ ArrayOf<Byte>* data,
            /* [in] */ Int32 index,
            /* [out] */ Int32* result);

        CARAPI GetDataSize(
            /* [out] */ Int32* size);

    private:
        friend class HdmiTimerRecordSources;

        TimerInfo(
            /* [in] */ Int32 dayOfMonth,
            /* [in] */ Int32 monthOfYear,
            /* [in] */ IHdmiTimerRecordSourcesTime* startTime,
            /* [in] */ IHdmiTimerRecordSourcesDuration* duration,
            /* [in] */ Int32 recordingSequence);

    private:
        static const Int32 DAY_OF_MONTH_SIZE;
        static const Int32 MONTH_OF_YEAR_SIZE;
        static const Int32 START_TIME_SIZE; // 1byte for hour and 1byte for minute.
        static const Int32 DURATION_SIZE; // 1byte for hour and 1byte for minute.
        static const Int32 RECORDING_SEQUENCE_SIZE;
        static const Int32 BASIC_INFO_SIZE;

        /** Day of month. */
        const Int32 mDayOfMonth;
        /** Month of year. */
        const Int32 mMonthOfYear;
        /**
         * Time of day.
         * [Hour][Minute]. 0 &lt;= Hour &lt;= 24, 0 &lt;= Minute &lt;= 60 in BCD format.
         */
        AutoPtr<IHdmiTimerRecordSourcesTime> mStartTime;
        /**
         * Duration. [Hour][Minute].
         * 0 &lt;= Hour &lt;= 99, 0 &lt;= Minute &lt;= 60 in BCD format.
         * */
        AutoPtr<IHdmiTimerRecordSourcesDuration> mDuration;
        /**
         * Indicates if recording is repeated and, if so, on which days. For repeated recordings,
         * the recording sequence value is the bitwise OR of the days when recordings are required.
         * [Recording Sequence] shall be set to 0x00 when the recording is not repeated. Bit 7 is
         * reserved and shall be set to zero.
         */
        Int32 mRecordingSequence;
    };

    class TimerRecordSource
        : public Object
        , public ITimerRecordSource
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI GetDataSize(
            /* [out] */ Int32* size);

        CARAPI ToByteArray(
            /* [in] */ ArrayOf<Byte>* data,
            /* [in] */ Int32 index,
            /* [out] */ Int32* result);
    private:
        friend class HdmiTimerRecordSources;

        TimerRecordSource(
            /* [in] */ IHdmiTimerRecordSourcesTimerInfo* timerInfo,
            /* [in] */ IRecordSource* recordSource);

    private:
        AutoPtr<IRecordSource> mRecordSource;
        AutoPtr<IHdmiTimerRecordSourcesTimerInfo> mTimerInfo;
    };

private:
    class ExternalSourceDecorator
        : public HdmiRecordSources::RecordSource
    {
    public:
        CARAPI ExtraParamToByteArray(
            /* [in] */ ArrayOf<Byte>* data,
            /* [in] */ Int32 index,
            /* [out] */ Int32* array);

    private:
        friend class HdmiTimerRecordSources;

        ExternalSourceDecorator(
            /* [in] */ IRecordSource* recordSource,
            /* [in] */ Int32 externalSourceSpecifier);

    private:
        AutoPtr<IRecordSource> mRecordSource;
        Int32 mExternalSourceSpecifier;
    };

public:
    CAR_INTERFACE_DECL()

    virtual ~HdmiTimerRecordSources() {}

    /**
     * Create {@link TimerRecordSource} for digital source which is used for &lt;Set Digital
     * Timer&gt;.
     *
     * @param timerInfo timer info used for timer recording
     * @param source digital source used for timer recording
     * @return {@link TimerRecordSource}
     * @throws IllegalArgumentException if {@code timerInfo} or {@code source} is null
     */
    static CARAPI OfDigitalSource(
        /* [in] */ IHdmiTimerRecordSourcesTimerInfo* timerInfo,
        /* [in] */ IHdmiRecordSourcesDigitalServiceSource* source,
        /* [out] */ ITimerRecordSource** result);

    /**
     * Create {@link TimerRecordSource} for analogue source which is used for &lt;Set Analogue
     * Timer&gt;.
     *
     * @param timerInfo timer info used for timer recording
     * @param source digital source used for timer recording
     * @return {@link TimerRecordSource}
     * @throws IllegalArgumentException if {@code timerInfo} or {@code source} is null
     */
    static CARAPI OfAnalogueSource(
        /* [in] */ IHdmiTimerRecordSourcesTimerInfo* timerInfo,
        /* [in] */ IHdmiRecordSourcesAnalogueServiceSource* source,
        /* [out] */ ITimerRecordSource** result);

    /**
     * Create {@link TimerRecordSource} for external plug which is used for &lt;Set External
     * Timer&gt;.
     *
     * @param timerInfo timer info used for timer recording
     * @param source digital source used for timer recording
     * @return {@link TimerRecordSource}
     * @throws IllegalArgumentException if {@code timerInfo} or {@code source} is null
     */
    static CARAPI OfExternalPlug(
        /* [in] */ IHdmiTimerRecordSourcesTimerInfo* timerInfo,
        /* [in] */ IHdmiRecordSourcesExternalPlugData* source,
        /* [out] */ ITimerRecordSource** result);

    /**
     * Create {@link TimerRecordSource} for external physical address which is used for &lt;Set
     * External Timer&gt;.
     *
     * @param timerInfo timer info used for timer recording
     * @param source digital source used for timer recording
     * @return {@link TimerRecordSource}
     * @throws IllegalArgumentException if {@code timerInfo} or {@code source} is null
     */
    static CARAPI OfExternalPhysicalAddress(
        /* [in] */ IHdmiTimerRecordSourcesTimerInfo*  timerInfo,
        /* [in] */ IHdmiRecordSourcesExternalPhysicalAddress* source,
        /* [out] */ ITimerRecordSource** result);

    /**
     * Create {@link Duration} for time value.
     *
     * @param hour hour in range of [0, 23]
     * @param minute minute in range of [0, 60]
     * @return {@link Duration}
     * @throws IllegalArgumentException if hour or minute is out of range
     */
    static CARAPI TimeOf(
        /* [in] */ Int32 hour,
        /* [in] */ Int32 minute,
        /* [out] */ IHdmiTimerRecordSourcesTime** time);

    /**
     * Create {@link Duration} for duration value.
     *
     * @param hour hour in range of [0, 99]
     * @param minute minute in range of [0, 59]
     * @return {@link Duration}
     * @throws IllegalArgumentException if hour or minute is out of range
     */
    static CARAPI DurationOf(
        /* [in] */ Int32 hour,
        /* [in] */ Int32 minute,
        /* [out] */ IHdmiTimerRecordSourcesDuration** result);

    /**
     * Create {@link TimerInfo} with the given information.
     *
     * @param dayOfMonth day of month
     * @param monthOfYear month of year
     * @param startTime start time in {@link Time}
     * @param duration duration in {@link Duration}
     * @param recordingSequence recording sequence. Use RECORDING_SEQUENCE_REPEAT_ONCE_ONLY for no
     *            repeat. Otherwise use combination of {@link #RECORDING_SEQUENCE_REPEAT_SUNDAY},
     *            {@link #RECORDING_SEQUENCE_REPEAT_MONDAY},
     *            {@link #RECORDING_SEQUENCE_REPEAT_TUESDAY},
     *            {@link #RECORDING_SEQUENCE_REPEAT_WEDNESDAY},
     *            {@link #RECORDING_SEQUENCE_REPEAT_THURSDAY},
     *            {@link #RECORDING_SEQUENCE_REPEAT_FRIDAY},
     *            {@link #RECORDING_SEQUENCE_REPEAT_SATUREDAY}.
     * @return {@link TimerInfo}.
     * @throws IllegalArgumentException if input value is invalid
     */
    static CARAPI TimerInfoOf(
        /* [in] */ Int32 dayOfMonth,
        /* [in] */ Int32 monthOfYear,
        /* [in] */ IHdmiTimerRecordSourcesTime* startTime,
        /* [in] */ IHdmiTimerRecordSourcesDuration* duration,
        /* [in] */ Int32 recordingSequence,
        /* [out] */ IHdmiTimerRecordSourcesTimerInfo** result);

    /**
     * Check the byte array of timer record source.
     * @param sourcetype
     * @param recordSource
     * @hide
     */
    //@SystemApi
    static CARAPI CheckTimerRecordSource(
        /* [in] */ Int32 sourcetype,
        /* [in] */ ArrayOf<Byte>* recordSource,
        /* [out] */ Boolean* result);

private:
    HdmiTimerRecordSources() {}

    static CARAPI CheckTimerRecordSourceInputs(
        /* [in] */ IHdmiTimerRecordSourcesTimerInfo* timerInfo,
        /* [in] */ IRecordSource* source);

    static CARAPI CheckTimeValue(
        /* [in] */ Int32 hour,
        /* [in] */ Int32 minute);

    static CARAPI CheckDurationValue(
        /* [in] */ Int32 hour,
        /* [in] */ Int32 minute);

private:
    static const String TAG;

    static const Int32 RECORDING_SEQUENCE_REPEAT_MASK;

    /**
     * External source specifier types.
     */
    static const Int32 EXTERNAL_SOURCE_SPECIFIER_EXTERNAL_PLUG;
    static const Int32 EXTERNAL_SOURCE_SPECIFIER_EXTERNAL_PHYSICAL_ADDRESS;
};

} //Hdmi
} //Hardware
} //Droid
} //Elastos

#endif  //__ELASTOS_DROID_HARDWARE_HDMI_HDMITIMERRECODSOURCES_H__
