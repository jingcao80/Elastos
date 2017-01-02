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

#ifndef __ELASTOS_DROID_SERVICE_VOICE_VOICEINTERACTIONSERVICEINFO_H__
#define __ELASTOS_DROID_SERVICE_VOICE_VOICEINTERACTIONSERVICEINFO_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Service.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Voice {

class VoiceInteractionServiceInfo
    : public Object
    , public IVoiceInteractionServiceInfo
{
public:
    CAR_INTERFACE_DECL()

    VoiceInteractionServiceInfo();

    CARAPI constructor(
        /* [in] */ IPackageManager* pm,
        /* [in] */ IComponentName* comp);

    CARAPI constructor(
        /* [in] */ IPackageManager* pm,
        /* [in] */ IComponentName* comp,
        /* [in] */ Int32 userHandle);

    CARAPI constructor(
        /* [in] */ IPackageManager* pm,
        /* [in] */ IServiceInfo* si);

    CARAPI GetParseError(
        /* [out] */ String* error);

    CARAPI GetServiceInfo(
        /* [out] */ IServiceInfo** info);

    CARAPI GetSessionService(
        /* [out] */ String* service);

    CARAPI GetRecognitionService(
        /* [out] */ String* service);

    CARAPI GetSettingsActivity(
        /* [out] */ String* activity);

protected:
    const static String TAG;

private:
    String mParseError;

    AutoPtr<IServiceInfo> mServiceInfo;
    String mSessionService;
    String mRecognitionService;
    String mSettingsActivity;
};

} // namespace Voice
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_VOICE_VOICEINTERACTIONSERVICEINFO_H__
