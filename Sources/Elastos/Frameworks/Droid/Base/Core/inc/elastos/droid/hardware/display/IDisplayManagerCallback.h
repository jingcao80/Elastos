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

#ifndef __ELASTOS_DROID_HARDWARE_DISPLAY_IDISPLAYMANAGERCALLBACK_H__
#define __ELASTOS_DROID_HARDWARE_DISPLAY_IDISPLAYMANAGERCALLBACK_H__

#include "Elastos.Droid.Hardware.h"
#include <binder/Binder.h>
#include <binder/Parcel.h>

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Display {

class IDisplayManagerCallbackStub
    : public android::BBinder
{
public:
    IDisplayManagerCallbackStub(
        /* [in] */ IIDisplayManagerCallback* callback);

private:
    void onLastStrongRef(
        /* [in] */ const void* id);

    android::status_t onTransact(
        /* [in] */ uint32_t code,
        /* [in] */ const android::Parcel& data,
        /* [out] */ android::Parcel* reply,
        /* [in] */ uint32_t flags = 0);

private:
    static const Boolean DEBUG;
    static const String TAG;
    static const String DESCRIPTOR;
    static const Int32 TRANSACTION_onDisplayEvent;

    AutoPtr<IIDisplayManagerCallback> mCallback;
};

} // namespace Display
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_DISPLAY_IDISPLAYMANAGERCALLBACK_H__
