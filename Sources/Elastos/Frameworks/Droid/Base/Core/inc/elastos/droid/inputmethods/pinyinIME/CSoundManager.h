
#ifndef  __ELASTOS_DROID_INPUTMETHODS_PINYINIME_CSOUNDMANAGER_H__
#define  __ELASTOS_DROID_INPUTMETHODS_PINYINIME_CSOUNDMANAGER_H__

#include "_Elastos_Droid_Inputmethods_PinyinIME_CSoundManager.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Media::IAudioManager;

namespace Elastos {
namespace Droid {
namespace Inputmethods {
namespace PinyinIME {

/**
 * Class used to manage related sound resources.
 */
CarClass(CSoundManager)
    , public Object
    , public ISoundManager
{
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CSoundManager();

    CARAPI constructor(
        /* [in] */ IContext* ctx);

    CARAPI UpdateRingerMode();

    CARAPI PlayKeyDown();

private:
    AutoPtr<IContext> mContext;
    AutoPtr<IAudioManager> mAudioManager;
    // Align sound effect volume on music volume
    const Float FX_VOLUME;
    Boolean mSilentMode;
};

} // namespace PinyinIME
} // namespace Inputmethods
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_INPUTMETHODS_PINYINIME_CSOUNDMANAGER_H__
