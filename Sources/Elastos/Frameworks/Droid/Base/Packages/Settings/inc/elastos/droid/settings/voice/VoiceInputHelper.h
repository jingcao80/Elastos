
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
