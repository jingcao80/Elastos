#ifndef __ELASTOS_DROID_MEDIA_AUDIOPOLICY_CAUDIOPOLICY_H__
#define __ELASTOS_DROID_MEDIA_AUDIOPOLICY_CAUDIOPOLICY_H__

#include "_Elastos_Droid_Media_AudioPolicy_CAudioPolicy.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace Media {
namespace AudioPolicy {

/**
 * @hide CANDIDATE FOR PUBLIC API
 * AudioPolicy provides access to the management of audio routing and audio focus.
 */
CarClass(CAudioPolicy)
    , public Object
    , public IAudioPolicy
{
public:
    CAudioPolicy();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    virtual ~CAudioPolicy();

    /**
     * The parameter is guaranteed non-null through the Builder
     */
    CARAPI constructor(
        /* [in] */ IAudioPolicyConfig * config);

    /** @hide */
    CARAPI Token(
        /* [out] */ IBinder ** result);

    /** @hide */
    CARAPI GetConfig(
        /* [out] */ IAudioPolicyConfig** result);

    CARAPI GetStatus(
        /* [out] */ Int32 * result);

    CARAPI ToString(
        /* [out] */ String * result);

private:
    CARAPI_(void) SetStatusListener(
        /* [in] */ IAudioPolicyStatusListener * l);

private:
    static String TAG; // = "AudioPolicy";
    Int32 mStatus;
    AutoPtr<IAudioPolicyStatusListener> mStatusListener;

    AutoPtr<IBinder> mToken;
    AutoPtr<IAudioPolicyConfig> mConfig;
};

} // namespace AudioPolicy
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_AUDIOPOLICY_CAUDIOPOLICY_H__
