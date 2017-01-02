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

#ifndef __ELASTOS_DROID_HARDWARE_HDMI_CHDMIRECORDSOURCESHELPER_H__
#define __ELASTOS_DROID_HARDWARE_HDMI_CHDMIRECORDSOURCESHELPER_H__

#include "_Elastos_Droid_Hardware_Hdmi_CHdmiRecordSourcesHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Hdmi {

CarClass(CHdmiRecordSourcesHelper)
    , public Singleton
    , public IHdmiRecordSourcesHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI OfOwnSource(
        /* [out] */ IHdmiRecordSourcesOwnSource** source);

    CARAPI OfDigitalChannelId(
        /* [in] */ Int32 broadcastSystem,
        /* [in] */ IHdmiRecordSourcesDigitalChannelData* data,
        /* [out] */ IHdmiRecordSourcesDigitalServiceSource** source);

    CARAPI OfArib(
        /* [in] */ Int32 aribType,
        /* [in] */ IHdmiRecordSourcesAribData* data,
        /* [out] */ IHdmiRecordSourcesDigitalServiceSource** source);

    CARAPI OfAtsc(
        /* [in] */ Int32 atscType,
        /* [in] */ IHdmiRecordSourcesAtscData* data,
        /* [out] */ IHdmiRecordSourcesDigitalServiceSource** source);

    CARAPI OfDvb(
        /* [in] */ Int32 dvbType,
        /* [in] */ IHdmiRecordSourcesDvbData* data,
        /* [out] */ IHdmiRecordSourcesDigitalServiceSource** source);

    CARAPI OfAnalogue(
        /* [in] */ Int32 broadcastType,
        /* [in] */ Int32 frequency,
        /* [in] */ Int32 broadcastSystem,
        /* [out] */ IHdmiRecordSourcesAnalogueServiceSource** source);

    CARAPI OfExternalPlug(
        /* [in] */ Int32 plugNumber,
        /* [out] */ IHdmiRecordSourcesExternalPlugData** data);

    CARAPI OfExternalPhysicalAddress(
        /* [in] */ Int32 physicalAddress,
        /* [out] */ IHdmiRecordSourcesExternalPhysicalAddress** address);

    CARAPI CheckRecordSource(
        /* [in] */ ArrayOf<Byte>* recordSource,
        /* [out] */ Boolean* result);
};

} // namespace Hdmi
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_HDMI_CHDMIRECORDSOURCESHELPER_H__
