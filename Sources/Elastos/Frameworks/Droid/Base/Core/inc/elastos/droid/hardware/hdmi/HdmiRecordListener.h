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

#ifndef __ELASTOS_DROID_HARDWARE_HDMI_HDMIRECORDLISTENER_H__
#define __ELASTOS_DROID_HARDWARE_HDMI_HDMIRECORDLISTENER_H__

#include "Elastos.Droid.Hardware.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Hdmi {

class HdmiRecordListener
    : public Object
    , public IHdmiRecordListener
{
public:
    class TimerStatusData
        : public Object
        , public IHdmiRecordListenerTimerStatusData
    {
    public:
        CAR_INTERFACE_DECL()

        static CARAPI ParseFrom(
            /* [in] */ Int32 result,
            /* [out] */ IHdmiRecordListenerTimerStatusData** _result);

        /**
         * Indicates if there is another timer block already set which overlaps with this new
         * recording request.
         */
        CARAPI IsOverlapped(
            /* [out] */ Boolean* result);

        /**
         * Indicates if removable media is present and its write protect state.
         * It should be one of the following values.
         * <ul>
         *   <li>{@link HdmiControlManager#TIMER_STATUS_MEDIA_INFO_PRESENT_NOT_PROTECTED}
         *   <li>{@link HdmiControlManager#TIMER_STATUS_MEDIA_INFO_PRESENT_PROTECTED}
         *   <li>{@link HdmiControlManager#TIMER_STATUS_MEDIA_INFO_NOT_PRESENT}
         * </ul>
         */
        CARAPI GetMediaInfo(
            /* [out] */ Int32* info);

        /**
         * Selector for [Timer Programmed Info].
         * If it is {@code true}, {@link #getProgrammedInfo()} would have meaningful value and
         * ignore result of {@link #getNotProgammedError()}.
         */
        CARAPI IsProgrammed(
            /* [out] */ Boolean* result);

        /**
         * Information indicating any non-fatal issues with the programming request.
         * It's set only if {@link #isProgrammed()} returns true.
         * It should be one of the following values.
         * <ul>
         *   <li>{@link HdmiControlManager#TIMER_STATUS_PROGRAMMED_INFO_ENOUGH_SPACE}
         *   <li>{@link HdmiControlManager#TIMER_STATUS_PROGRAMMED_INFO_NOT_ENOUGH_SPACE}
         *   <li>{@link HdmiControlManager#TIMER_STATUS_PROGRAMMED_INFO_MIGHT_NOT_ENOUGH_SPACE}
         *   <li>{@link HdmiControlManager#TIMER_STATUS_PROGRAMMED_INFO_NO_MEDIA_INFO}
         * </ul>
         *
         * @throws IllegalStateException if it's called when {@link #isProgrammed()}
         *                               returns false
         */
        CARAPI GetProgrammedInfo(
            /* [out] */ Int32* info);

        /**
         * Information indicating any fatal issues with the programming request.
         * It's set only if {@link #isProgrammed()} returns false.
         * it should be one of the following values.
         * <ul>
         *   <li>{@link HdmiControlManager#TIMER_STATUS_NOT_PROGRAMMED_NO_FREE_TIME}
         *   <li>{@link HdmiControlManager#TIMER_STATUS_NOT_PROGRAMMED_DATE_OUT_OF_RANGE}
         *   <li>{@link HdmiControlManager#TIMER_STATUS_NOT_PROGRAMMED_INVALID_SEQUENCE}
         *   <li>{@link HdmiControlManager#TIMER_STATUS_NOT_PROGRAMMED_INVALID_EXTERNAL_PHYSICAL_NUMBER}
         *   <li>{@link HdmiControlManager#TIMER_STATUS_NOT_PROGRAMMED_CA_NOT_SUPPORTED}
         *   <li>{@link HdmiControlManager#TIMER_STATUS_NOT_PROGRAMMED_NO_CA_ENTITLEMENTS}
         *   <li>{@link HdmiControlManager#TIMER_STATUS_NOT_PROGRAMMED_UNSUPPORTED_RESOLUTION}
         *   <li>{@link HdmiControlManager#TIMER_STATUS_NOT_PROGRAMMED_PARENTAL_LOCK_ON}
         *   <li>{@link HdmiControlManager#TIMER_STATUS_NOT_PROGRAMMED_CLOCK_FAILURE}
         *   <li>{@link HdmiControlManager#TIMER_STATUS_NOT_PROGRAMMED_DUPLICATED}
         * </ul>
         *
         * @throws IllegalStateException if it's called when {@link #isProgrammed()}
         *                               returns true
         */
        CARAPI GetNotProgammedError(
            /* [out] */ Int32* error);

        /**
         * Duration hours.
         * Optional parameter: Contains an estimate of the space left on the media, expressed as a
         * time. This parameter may be returned when:
         *  - [Programmed Info] is “Not enough space available”; or
         *  - [Not Programmed Info] is “Duplicate: already programmed”
         */
        CARAPI GetDurationHour(
            /* [out] */ Int32* hour);

        /**
         * Duration minutes.
         * Optional parameter: Contains an estimate of the space left on the media, expressed as a
         * time. This parameter may be returned when:
         *  - [Programmed Info] is “Not enough space available”; or
         *  - [Not Programmed Info] is “Duplicate: already programmed”
         */
        CARAPI GetDurationMinute(
            /* [out] */ Int32* minute);

        /**
         * Extra error code.
         * <ul>
         * <li>{@link HdmiControlManager#TIMER_RECORDING_RESULT_EXTRA_NO_ERROR}
         *     No extra errors. Other values of this class might be available.
         * <li>{@link HdmiControlManager#TIMER_RECORDING_RESULT_EXTRA_CHECK_RECORDER_CONNECTION}
         *     Check record connection. Other values of this class should be ignored.
         * <li>{@link HdmiControlManager#TIMER_RECORDING_RESULT_EXTRA_FAIL_TO_RECORD_SELECTED_SOURCE}
         *     Fail to record selected source. Other values of this class should be ignored.
         * <li>{@link HdmiControlManager#TIMER_RECORDING_RESULT_EXTRA_CEC_DISABLED}
         *     Cec disabled. Other values of this class should be ignored.
         * </ul>
         */
        CARAPI GetExtraError(
            /* [out] */ Int32* error);

    private:
        TimerStatusData() {}

        // Most significant 4 bits is used for 10 digits and
        // Least significant 4 bits is used for 1 digits.
        static CARAPI_(Int32) BcdByteToInt(
            /* [in] */ Byte value);

    private:
        Boolean mOverlapped;
        Int32 mMediaInfo;
        Boolean mProgrammed;

        Int32 mProgrammedInfo;
        Int32 mNotProgrammedError;
        Int32 mDurationHour;
        Int32 mDurationMinute;

        Int32 mExtraError;
    };

public:
    CAR_INTERFACE_DECL()

protected:
    HdmiRecordListener() {}

    /**
     * Called when TV received one touch record request from record device. The client of this
     * should use {@link HdmiRecordSources} to return it.
     *
     * @param recorderAddress
     * @return record source to be used for recording. Null if no device is available.
     */
    virtual CARAPI GetOneTouchRecordSource(
        /* [in] */ Int32 recorderAddress,
        /* [out] */ IRecordSource** source) = 0;

    /**
     * Called when one touch record is started or failed during initialization.
     *
     * @param result result code. For more details, please look at all constants starting with
     *            "ONE_TOUCH_RECORD_". Only
     *            {@link HdmiControlManager#ONE_TOUCH_RECORD_RECORDING_CURRENTLY_SELECTED_SOURCE},
     *            {@link HdmiControlManager#ONE_TOUCH_RECORD_RECORDING_DIGITAL_SERVICE},
     *            {@link HdmiControlManager#ONE_TOUCH_RECORD_RECORDING_ANALOGUE_SERVICE}, and
     *            {@link HdmiControlManager#ONE_TOUCH_RECORD_RECORDING_EXTERNAL_INPUT} mean normal
     *            start of recording; otherwise, describes failure.
     */
    CARAPI OnOneTouchRecordResult(
        /* [in] */ Int32 result);

    /**
     * Called when timer recording is started or failed during initialization.
     *
     * @param data timer status data. For more details, look at {@link TimerStatusData}.
     */
    CARAPI OnTimerRecordingResult(
        /* [in] */ IHdmiRecordListenerTimerStatusData* data);

    /**
     * Called when receiving result for clear timer recording request.
     *
     * @param result result of clear timer. It should be one of
     *            {@link HdmiControlManager#CLEAR_TIMER_STATUS_TIMER_NOT_CLEARED_RECORDING}
     *            {@link HdmiControlManager#CLEAR_TIMER_STATUS_TIMER_NOT_CLEARED_NO_MATCHING},
     *            {@link HdmiControlManager#CLEAR_TIMER_STATUS_TIMER_NOT_CLEARED_NO_INFO_AVAILABLE},
     *            {@link HdmiControlManager#CLEAR_TIMER_STATUS_TIMER_CLEARED},
     *            {@link HdmiControlManager#CLEAR_TIMER_STATUS_CHECK_RECORDER_CONNECTION},
     *            {@link HdmiControlManager#CLEAR_TIMER_STATUS_FAIL_TO_CLEAR_SELECTED_SOURCE},
     *            {@link HdmiControlManager#CLEAR_TIMER_STATUS_CEC_DISABLE}.
     */
    CARAPI OnClearTimerRecordingResult(
        /* [in] */ Int32 result);
};

} //Hdmi
} //Hardware
} //Droid
} //Elastos

#endif  //__ELASTOS_DROID_HARDWARE_HDMI_HDMIRECORDLISTENER_H__
