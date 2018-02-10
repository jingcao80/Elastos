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
        /* [out] */ HANDLE* ptr);

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
    HANDLE mNative; // used by native code
};

} // namespace View
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_CINPUTCHANNEL_H__
