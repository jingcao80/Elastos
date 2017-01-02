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

#ifndef __ELASTOS_DROID_MEDIA_TV_CTVINPUTHARDWAREINFOBUILDER_H__
#define __ELASTOS_DROID_MEDIA_TV_CTVINPUTHARDWAREINFOBUILDER_H__

#include "_Elastos_Droid_Media_Tv_CTvInputHardwareInfoBuilder.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::IInteger32;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

CarClass(CTvInputHardwareInfoBuilder)
    , public Object
    , public ITvInputHardwareInfoBuilder
{
public:
    CTvInputHardwareInfoBuilder();

    virtual ~CTvInputHardwareInfoBuilder();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI DeviceId(
        /* [in] */ Int32 deviceId);

    CARAPI Type(
        /* [in] */ Int32 type);

    CARAPI AudioType(
        /* [in] */ Int32 audioType);

    CARAPI AudioAddress(
        /* [in] */ const String& audioAddress);

    CARAPI HdmiPortId(
        /* [in] */ Int32 hdmiPortId);

    CARAPI Build(
        /* [out] */ ITvInputHardwareInfo** result);

private:
    String mAudioAddress;
    Int32 mAudioType;
    AutoPtr<IInteger32> mDeviceId;
    AutoPtr<IInteger32> mType;
    AutoPtr<IInteger32> mHdmiPortId;
};

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_TV_CTVINPUTHARDWAREINFOBUILDER_H__
