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

#ifndef __ELASTOS_DROID_HARDWARE_DISPLAY_WIFIDISPLAYSESSIONINFO_H__
#define __ELASTOS_DROID_HARDWARE_DISPLAY_WIFIDISPLAYSESSIONINFO_H__

#include "Elastos.Droid.Hardware.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Display {

/**
 * Describes how the pixels of physical display device reflects the content of
 * a logical display.
 * <p>
 * This information is used by the input system to translate touch input from
 * physical display coordinates into logical display coordinates.
 * </p>
 *
 * @hide Only for use within the system server.
 */
class WifiDisplaySessionInfo
    : public Object
    , public IWifiDisplaySessionInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    WifiDisplaySessionInfo();

    virtual ~WifiDisplaySessionInfo() {}

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Boolean client,
        /* [in] */ Int32 session,
        /* [in] */ const String& group,
        /* [in] */ const String& pp,
        /* [in] */ const String& ip);

    CARAPI IsClient(
        /* [out] */ Boolean* result);

    CARAPI GetSessionId(
        /* [out] */ Int32* id);

    CARAPI GetGroupId(
        /* [out] */ String* gId);

    CARAPI GetPassphrase(
        /* [out] */ String* result);

    CARAPI GetIP(
        /* [out] */ String* ip);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    //@Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ToString(
        /* [out] */ String* result);

private:
    Boolean mClient;
    Int32 mSessionId;
    String mGroupId;
    String mPassphrase;
    String mIP;
};

} // namespace Display
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_DISPLAY_WIFIDISPLAYSESSIONINFO_H__
