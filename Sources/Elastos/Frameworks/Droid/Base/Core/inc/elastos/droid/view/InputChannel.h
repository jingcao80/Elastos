
#ifndef __ELASTOS_DROID_VIEW_INPUTCHANNEL_H__
#define __ELASTOS_DROID_VIEW_INPUTCHANNEL_H__

#include <elastos/core/Object.h>

using Elastos::Droid::View::IInputChannel;

namespace Elastos {
namespace Droid {
namespace View {

class InputChannel
    : public Object
    , public IInputChannel
    , public IParcelable
{
    friend class InputEventReceiver;
private:
    static const char* TAG;

    static const Boolean DEBUG = FALSE;

public:
    CAR_INTERFACE_DECL()

    /**
     * Creates an uninitialized input channel.
     * It can be initialized by reading from a Parcel or by transferring the state of
     * another input channel into this one.
     */
    InputChannel();

    ~InputChannel();

    CARAPI constructor();

    static CARAPI OpenInputChannelPair(
        /* [in] */ const String& name,
        /* [out, callee] */ ArrayOf<IInputChannel*>** icp);

    /**
     * Gets the name of the input channel.
     * @return The input channel name.
     */
    CARAPI GetName(
        /* [out] */ String* name);

    /**
     * Disposes the input channel.
     * Explicitly releases the reference this object is holding on the input channel.
     * When all references are released, the input channel will be closed.
     */
    CARAPI Dispose();

    CARAPI TransferTo(
        /* [in] */ IInputChannel* outParameter);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel *source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel *dest);

    CARAPI Dup(
        /* [out] */ IInputChannel** target);

    CARAPI GetNativeInputChannel(
        /* [out] */ Handle64* ptr);

private:
    static CARAPI NativeOpenInputChannelPair(
        /* [in] */ const String& name,
        /* [out] */ ArrayOf<IInputChannel*>** icp);

    CARAPI NativeDispose(
        /* [in] */ Boolean finalized);

    CARAPI NativeTransferTo(
        /* [in] */ IInputChannel* other);

    CARAPI NativeReadFromParcel(
        /* [in] */ IParcel* parcel);

    CARAPI NativeWriteToParcel(
        /* [in] */ IParcel* parcel);

    CARAPI NativeDup(
        /* [in] */ IInputChannel* target);

    CARAPI NativeGetName(
        /* [out] */ String* name);

public:
    Handle64 mNative; // used by native code
};

} // namespace View
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_CINPUTCHANNEL_H__
