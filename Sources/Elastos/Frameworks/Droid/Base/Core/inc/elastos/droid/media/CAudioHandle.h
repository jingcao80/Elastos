#ifndef __ELASTOS_DROID_MEDIA_CAUDIOHANDLE_H__
#define __ELASTOS_DROID_MEDIA_CAUDIOHANDLE_H__

#include "_Elastos_Droid_Media_CAudioHandle.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * Class to provide information about the audio devices.
 * @hide
 */
CarClass(CAudioHandle)
    , public Object
    , public IAudioHandle
{
public:
    CAudioHandle();

    virtual ~CAudioHandle();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Int32 id);

    CARAPI Id(
        /* [out] */ Int32* id);

    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* result);

private:
    Int32 mId;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_CAUDIOHANDLE_H__
