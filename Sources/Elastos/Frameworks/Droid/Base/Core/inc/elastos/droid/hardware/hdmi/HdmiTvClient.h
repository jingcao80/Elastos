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

#ifndef __ELASTOS_DROID_HARDWARE_HDMI_HDMITVCLIENT_H__
#define __ELASTOS_DROID_HARDWARE_HDMI_HDMITVCLIENT_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/hardware/hdmi/HdmiClient.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Hdmi {

class HdmiTvClient
    : public HdmiClient
    , public IHdmiTvClient
{
private:
    class MyCallback
        : public Object
        , public IIHdmiControlCallback
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        MyCallback(
            /* [in] */ IHdmiTvClientSelectCallback* _callback);

        CARAPI OnComplete(
            /* [in] */ Int32 result);

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        AutoPtr<IHdmiTvClientSelectCallback> mCallback;
    };

    class MyListener
        : public Object
        , public IIHdmiInputChangeListener
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        MyListener(
            /* [in] */ IHdmiTvClientInputChangeListener* listener);

        //@Override
        CARAPI OnChanged(
            /* [in] */ IHdmiDeviceInfo* info);

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        AutoPtr<IHdmiTvClientInputChangeListener> mListener;
    };

    class MyListener2
        : public Object
        , public IIHdmiRecordListener
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        MyListener2(
            /* [in] */ IHdmiRecordListener* listener);

        CARAPI GetOneTouchRecordSource(
            /* [in] */ Int32 recorderAddress,
            /* [out, callee] */ ArrayOf<Byte>** source);

        CARAPI OnOneTouchRecordResult(
            /* [in] */ Int32 result);

        CARAPI OnTimerRecordingResult(
            /* [in] */ Int32 result);

        CARAPI OnClearTimerRecordingResult(
            /* [in] */ Int32 result);

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        AutoPtr<IHdmiRecordListener> mListener;
    };

    class MyListener3
        : public Object
        , public IIHdmiMhlVendorCommandListener
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        MyListener3(
            /* [in] */ IHdmiTvClientHdmiMhlVendorCommandListener* listener);

        //@Override
        CARAPI OnReceived(
            /* [in] */ Int32 portId,
            /* [in] */ Int32 offset,
            /* [in] */ Int32 length,
            /* [in] */ ArrayOf<Byte>* data);

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        AutoPtr<IHdmiTvClientHdmiMhlVendorCommandListener> mListener;
    };

public:
    CAR_INTERFACE_DECL()

    HdmiTvClient();

    virtual ~HdmiTvClient() {}

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IIHdmiControlService* service);

    // Factory method for HdmiTvClient.
    // Declared package-private. Accessed by HdmiControlManager only.
    static CARAPI Create(
        /* [in] */ IIHdmiControlService* service,
        /* [out] */ IHdmiTvClient** client);

    CARAPI GetDeviceType(
        /* [out] */ Int32 * type);

    /**
     * Select a CEC logical device to be a new active source.
     *
     * @param logicalAddress logical address of the device to select
     * @param callback callback to get the result with
     * @throws {@link IllegalArgumentException} if the {@code callback} is null
     */
    CARAPI DeviceSelect(
        /* [in] */ Int32 logicalAddress,
        /* [in] */ IHdmiTvClientSelectCallback* _callback);

    /**
     * Select a HDMI port to be a new route path.
     *
     * @param portId HDMI port to select
     * @param callback callback to get the result with
     * @throws {@link IllegalArgumentException} if the {@code callback} is null
     */
    CARAPI PortSelect(
        /* [in] */ Int32 portId,
        /* [in] */ IHdmiTvClientSelectCallback* _callback);

    /**
     * Set the listener used to get informed of the input change event.
     *
     * @param listener listener object
     */
    CARAPI SetInputChangeListener(
        /* [in] */ IHdmiTvClientInputChangeListener* listener);

    /**
     * Set system audio volume
     *
     * @param oldIndex current volume index
     * @param newIndex volume index to be set
     * @param maxIndex maximum volume index
     */
    CARAPI SetSystemAudioVolume(
        /* [in] */ Int32 oldIndex,
        /* [in] */ Int32 newIndex,
        /* [in] */ Int32 maxIndex);

    /**
     * Set system audio mute status
     *
     * @param mute {@code true} if muted; otherwise, {@code false}
     */
    CARAPI SetSystemAudioMute(
        /* [in] */ Boolean mute);

    /**
     * Set record listener
     *
     * @param listener
     */
    CARAPI SetRecordListener(
        /* [in] */ IHdmiRecordListener* listener);

    /**
     * Start one touch recording with the given recorder address and recorder source.
     * <p>
     * Usage
     * <pre>
     * HdmiTvClient tvClient = ....;
     * // for own source.
     * OwnSource ownSource = ownHdmiRecordSources.ownSource();
     * tvClient.startOneTouchRecord(recorderAddress, ownSource);
     * </pre>
     */
    CARAPI StartOneTouchRecord(
        /* [in] */ Int32 recorderAddress,
        /* [in] */ IRecordSource* source);

    /**
     * Stop one touch record.
     *
     * @param recorderAddress recorder address where recoding will be stopped
     */
    CARAPI StopOneTouchRecord(
        /* [in] */ Int32 recorderAddress);

    /**
     * Start timer recording with the given recoder address and recorder source.
     * <p>
     * Usage
     * <pre>
     * HdmiTvClient tvClient = ....;
     * // create timer info
     * TimerInfo timerInfo = HdmiTimerRecourdSources.timerInfoOf(...);
     * // for digital source.
     * DigitalServiceSource recordSource = HdmiRecordSources.ofDigitalService(...);
     * // create timer recording source.
     * TimerRecordSource source = HdmiTimerRecourdSources.ofDigitalSource(timerInfo, recordSource);
     * tvClient.startTimerRecording(recorderAddress, source);
     * </pre>
     *
     * @param recorderAddress target recorder address
     * @param sourceType type of record source. It should be one of
     *          {@link HdmiControlManager#TIMER_RECORDING_TYPE_DIGITAL},
     *          {@link HdmiControlManager#TIMER_RECORDING_TYPE_ANALOGUE},
     *          {@link HdmiControlManager#TIMER_RECORDING_TYPE_EXTERNAL}.
     * @param source record source to be used
     */
    CARAPI StartTimerRecording(
        /* [in] */ Int32 recorderAddress,
        /* [in] */ Int32 sourceType,
        /* [in] */ ITimerRecordSource* source);

    /**
     * Clear timer recording with the given recorder address and recording source.
     * For more details, please refer {@link #startTimerRecording(int, int, TimerRecordSource)}.
     */
    CARAPI ClearTimerRecording(
        /* [in] */ Int32 recorderAddress,
        /* [in] */ Int32 sourceType,
        /* [in] */ ITimerRecordSource* source);

    /**
     * Set {@link HdmiMhlVendorCommandListener} to get incoming MHL vendor command.
     *
     * @param listener to receive incoming MHL vendor command
     */
    CARAPI SetHdmiMhlVendorCommandListener(
        /* [in] */ IHdmiTvClientHdmiMhlVendorCommandListener* listener);

    /**
     * Send MHL vendor command to the device connected to a port of the given portId.
     *
     * @param portId id of port to send MHL vendor command
     * @param offset offset in the in given data
     * @param length length of data. offset + length should be bound to length of data.
     * @param data container for vendor command data. It should be 16 bytes.
     * @throws IllegalArgumentException if the given parameters are invalid
     */
    CARAPI SendMhlVendorCommand(
        /* [in] */ Int32 portId,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [in] */ ArrayOf<Byte>* data);

private:
    static CARAPI_(AutoPtr<IIHdmiControlCallback>) GetCallbackWrapper(
        /* [in] */ IHdmiTvClientSelectCallback* _callback);

    static CARAPI_(AutoPtr<IIHdmiInputChangeListener>) GetListenerWrapper(
        /* [in] */ IHdmiTvClientInputChangeListener* listener);

    static CARAPI_(AutoPtr<IIHdmiRecordListener>) GetListenerWrapper(
        /* [in] */ IHdmiRecordListener* listener);

    CARAPI CheckTimerRecordingSourceType(
        /* [in] */ Int32 sourceType);

    CARAPI_(AutoPtr<IIHdmiMhlVendorCommandListener>) GetListenerWrapper(
        /* [in] */ IHdmiTvClientHdmiMhlVendorCommandListener* listener);

private:
    static const String TAG;
};

} //Hdmi
} //Hardware
} //Droid
} //Elastos

#endif  //__ELASTOS_DROID_HARDWARE_HDMI_HDMITVCLIENT_H__
