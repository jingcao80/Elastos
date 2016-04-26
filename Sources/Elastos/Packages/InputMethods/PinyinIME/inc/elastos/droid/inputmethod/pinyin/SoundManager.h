
#ifndef __ELASTOS_DROID_INPUTMETHOD_PINYIN_SOUNDMANAGER_H__
#define __ELASTOS_DROID_INPUTMETHOD_PINYIN_SOUNDMANAGER_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Media.h"
#include "_Elastos.Droid.InputMethod.Pinyin.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

/**
 * Class used to manage related sound resources.
 */
class SoundManager
    : public Object
{
public:
    SoundManager(
        /* [in] */ IContext* ctx);

    CARAPI_(void) UpdateRingerMode();

    static CARAPI_(AutoPtr<SoundManager>) GetInstance(
        /* [in] */ IContext* context);

    CARAPI_(void) PlayKeyDown();

private:
    static AutoPtr<SoundManager> sInstance;
    AutoPtr<IContext> mContext;
    AutoPtr<IAudioManager> mAudioManager;
    // Align sound effect volume on music volume
    const Float FX_VOLUME;
    Boolean mSilentMode;
};

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_INPUTMETHOD_PINYIN_SOUNDMANAGER_H__
