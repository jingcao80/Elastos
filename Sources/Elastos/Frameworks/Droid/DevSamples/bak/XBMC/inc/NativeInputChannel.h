
#ifndef __NATIVEINPUTCHANNEL_H__
#define __NATIVEINPUTCHANNEL_H__

#include <androidfw/InputTransport.h>
#include <Elastos.Droid.Core.h>

using Elastos::Droid::View::IInputChannel;

typedef void (*InputChannelObjDisposeCallback)(
    /* [in] */ IInputChannel* inputChannelObj,
    /* [in] */ const android::sp<android::InputChannel>& inputChannel,
    /* [in] */ void* data);

class NativeInputChannel
{
public:
    NativeInputChannel(
        /* [in] */ const android::sp<android::InputChannel>& inputChannel);

    ~NativeInputChannel();

    inline android::sp<android::InputChannel> getInputChannel()
    { return mInputChannel; }

    void setDisposeCallback(
        /* [in] */ InputChannelObjDisposeCallback callback,
        /* [in] */ void* data);

    void invokeAndRemoveDisposeCallback(
        /* [in] */ IInputChannel* obj);

private:
    android::sp<android::InputChannel> mInputChannel;
    InputChannelObjDisposeCallback mDisposeCallback;
    void* mDisposeData;
};

#endif //__NATIVEINPUTCHANNEL_H__
