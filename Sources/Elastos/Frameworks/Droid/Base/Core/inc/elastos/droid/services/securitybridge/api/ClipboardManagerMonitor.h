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

#ifndef _ELASTOS_DROID_SERVICES_SECURITYBRIDGE_API_CLIPBOARDMANAGERMONITOR_H__
#define _ELASTOS_DROID_SERVICES_SECURITYBRIDGE_API_CLIPBOARDMANAGERMONITOR_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.SecurityBridge.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IClipData;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Services {
namespace SecurityBridge {
namespace Api {

class ClipboardManagerMonitor
    : public Object
    , public IClipboardManagerMonitor
{
public:
    ClipboardManagerMonitor();

    virtual ~ClipboardManagerMonitor();

    CARAPI constructor();

    CAR_INTERFACE_DECL()

    CARAPI ApprovePasteRequest(
        /* [in] */ Int32 appID,
        /* [in] */ IClipData* clipData,
        /* [out] */ Boolean* result);

    CARAPI NotifyCopy(
        /* [in] */ Int32 appID,
        /* [in] */ IClipData* clipData);
};

} // namespace Api
} // namespace SecurityBridge
} // namespace Services
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_SERVICES_SECURITYBRIDGE_API_CLIPBOARDMANAGERMONITOR_H__