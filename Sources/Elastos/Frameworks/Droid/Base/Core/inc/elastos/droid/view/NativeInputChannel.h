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
