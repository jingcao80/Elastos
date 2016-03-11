
#ifndef __ELASTOS_DROID_VIEW_NATIVEINPUTCHANNEL_H__
#define __ELASTOS_DROID_VIEW_NATIVEINPUTCHANNEL_H__

#include <input/InputTransport.h>
#include <elastos/core/Object.h>

using namespace Elastos;

namespace Elastos {
namespace Droid {
namespace View {

interface IInputChannel;

typedef void (*InputChannelObjDisposeCallback)(
    /* [in] */ IInputChannel* inputChannelObj,
    /* [in] */ const android::sp<android::InputChannel>& inputChannel,
    /* [in] */ void* data);

class ECO_PUBLIC NativeInputChannel
    : public Object
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

} // namespace View
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_NATIVEINPUTCHANNEL_H__
