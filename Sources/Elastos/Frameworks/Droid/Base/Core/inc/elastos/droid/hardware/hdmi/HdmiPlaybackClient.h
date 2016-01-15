
#ifndef __ELASTOS_DROID_HARDWARE_HDMI_HDMIPLAYBACKCLIENT_H__
#define __ELASTOS_DROID_HARDWARE_HDMI_HDMIPLAYBACKCLIENT_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/hardware/hdmi/HdmiClient.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Hdmi {

class HdmiPlaybackClient
    : public HdmiClient
    , public IHdmiPlaybackClient
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
            /* [in] */ IHdmiPlaybackClientOneTouchPlayCallback* _callback);

        //@Override
        CARAPI OnComplete(
            /* [in] */ Int32 result);

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        AutoPtr<IHdmiPlaybackClientOneTouchPlayCallback> mCallback;
    };

    class MyCallback2
        : public Object
        , public IIHdmiControlCallback
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        MyCallback2(
            /* [in] */ IHdmiPlaybackClientDisplayStatusCallback* _callback);

        //@Override
        CARAPI OnComplete(
            /* [in] */ Int32 result);

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        AutoPtr<IHdmiPlaybackClientDisplayStatusCallback> mCallback;
    };

public:
    CAR_INTERFACE_DECL()

    HdmiPlaybackClient();

    virtual ~HdmiPlaybackClient() {}

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IIHdmiControlService* service);

    /**
     * Perform the feature 'one touch play' from playback device to turn on display
     * and switch the input.
     *
     * @param callback {@link OneTouchPlayCallback} object to get informed
     *         of the result
     */
    CARAPI OneTouchPlay(
        /* [in] */ IHdmiPlaybackClientOneTouchPlayCallback* _callback);

    //@Override
    CARAPI GetDeviceType(
        /* [out] */ Int32* type);

    /**
     * Get the status of display device connected through HDMI bus.
     *
     * @param callback {@link DisplayStatusCallback} object to get informed
     *         of the result
     */
    CARAPI QueryDisplayStatus(
        /* [in] */ IHdmiPlaybackClientDisplayStatusCallback* _callback);

private:
    CARAPI_(AutoPtr<IIHdmiControlCallback>) GetCallbackWrapper(
        /* [in] */ IHdmiPlaybackClientOneTouchPlayCallback* callback);

    CARAPI_(AutoPtr<IIHdmiControlCallback>) GetCallbackWrapper(
        /* [in] */ IHdmiPlaybackClientDisplayStatusCallback* callback);

private:
    static const String TAG;
};

} //Hdmi
} //Hardware
} //Droid
} //Elastos

#endif  //__ELASTOS_DROID_HARDWARE_HDMI_HDMIPLAYBACKCLIENT_H__
