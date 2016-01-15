
#ifndef __ELASTOS_DROID_MEDIA_CAUDIOMANAGERAUDIOFOCUSDISPATCHER_H__
#define __ELASTOS_DROID_MEDIA_CAUDIOMANAGERAUDIOFOCUSDISPATCHER_H__

#include "_Elastos_Droid_Media_CAudioManagerAudioFocusDispatcher.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CAudioManagerAudioFocusDispatcher)
    , public Object
    , public IIAudioFocusDispatcher
    , public IBinder
{
public:
    CAudioManagerAudioFocusDispatcher();

    virtual ~CAudioManagerAudioFocusDispatcher();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IAudioManager* owner);

    CARAPI DispatchAudioFocusChange(
        /* [in] */ Int32 focusChange,
        /* [in] */ const String& id);

    CARAPI ToString(
        /* [out] */ String* result);

private:
    AutoPtr<IWeakReference> mOwnerWeak;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CAUDIOMANAGERAUDIOFOCUSDISPATCHER_H__
