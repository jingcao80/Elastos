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

#ifndef __ELASTOS_DROID_SETTINGS_VOICE_VOICEINPUTHELPER_H__
#define __ELASTOS_DROID_SETTINGS_VOICE_VOICEINPUTHELPER_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Service.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Service::Voice::IVoiceInteractionServiceInfo;
using Elastos::Core::IComparable;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Voice {

class VoiceInputHelper
    : public Object
{
public:
    class BaseInfo
        : public Object
        , public IComparable
    {
    public:
        CAR_INTERFACE_DECL()

        BaseInfo();

        ~BaseInfo();

        CARAPI constructor(
            /* [in] */ IPackageManager* pm,
            /* [in] */ IServiceInfo* _service,
            /* [in] */ const String& _settings);

        //@Override
        CARAPI CompareTo(
            /* [in] */ IInterface* another,
            /* [out] */ Int32* result);

    public:
        AutoPtr<IServiceInfo> mService;
        AutoPtr<IComponentName> mComponentName;
        String mKey;
        AutoPtr<IComponentName> mSettings;
        AutoPtr<ICharSequence> mLabel;
        String mLabelStr;
        AutoPtr<ICharSequence> mApplabel;
    };

    class InteractionInfo
        : public BaseInfo
    {
    public:
        InteractionInfo();

        ~InteractionInfo();

        CARAPI constructor(
            /* [in] */ IPackageManager* pm,
            /* [in] */ IVoiceInteractionServiceInfo* _service);

    public:
        AutoPtr<IVoiceInteractionServiceInfo> mServiceInfo;
    };

    class RecognizerInfo
        : public BaseInfo
    {
    public:
        RecognizerInfo();

        ~RecognizerInfo();

        CARAPI constructor(
            /* [in] */ IPackageManager* pm,
            /* [in] */ IServiceInfo* _service,
            /* [in] */ const String& _settings);
    };

public:
    VoiceInputHelper(
        /* [in] */ IContext* context);

    CARAPI_(Boolean) HasItems();

    CARAPI BuildUi();

public:
    static const String TAG;
    AutoPtr<IContext> mContext;

    // final List<ResolveInfo> mAvailableVoiceInteractions;
    AutoPtr<IList> mAvailableVoiceInteractions;
    // final List<ResolveInfo> mAvailableRecognition;
    AutoPtr<IList> mAvailableRecognition;

    // final ArrayList<InteractionInfo> mAvailableInteractionInfos = new ArrayList<>();
    AutoPtr<IArrayList> mAvailableInteractionInfos;
    // final ArrayList<RecognizerInfo> mAvailableRecognizerInfos = new ArrayList<>();
    AutoPtr<IArrayList> mAvailableRecognizerInfos;

    AutoPtr<IComponentName> mCurrentVoiceInteraction;
    AutoPtr<IComponentName> mCurrentRecognizer;
};

} // namespace Voice
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_VOICE_VOICEINPUTHELPER_H__
