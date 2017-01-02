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

#include "elastos/droid/view/NativeInputChannel.h"

namespace Elastos {
namespace Droid {
namespace View {

NativeInputChannel::NativeInputChannel(
    /* [in] */ const android::sp<android::InputChannel>& inputChannel) :
    mInputChannel(inputChannel),
    mDisposeCallback(NULL)
{
}

NativeInputChannel::~NativeInputChannel()
{
}

void NativeInputChannel::setDisposeCallback(
    /* [in] */ InputChannelObjDisposeCallback callback,
    /* [in] */ void* data)
{
    mDisposeCallback = callback;
    mDisposeData = data;
}

void NativeInputChannel::invokeAndRemoveDisposeCallback(
    /* [in] */ IInputChannel* obj)
{
    if (mDisposeCallback) {
        mDisposeCallback(obj, mInputChannel, mDisposeData);
        mDisposeCallback = NULL;
        mDisposeData = NULL;
    }
}

} // namespace View
} // namepsace Droid
} // namespace Elastos
