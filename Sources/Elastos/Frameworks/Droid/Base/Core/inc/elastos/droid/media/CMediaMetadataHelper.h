#ifndef __ELASTOS_DROID_MEDIA_CMEDIAMETADATAHELPER_H__
#define __ELASTOS_DROID_MEDIA_CMEDIAMETADATAHELPER_H__

#include "_Elastos_Droid_Media_CMediaMetadataHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CMediaMetadataHelper)
    , public Singleton
    , public IMediaMetadataHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Helper for getting the String key used by {@link MediaMetadata} from the
     * integer key that {@link MediaMetadataEditor} uses.
     *
     * @param editorKey The key used by the editor
     * @return The key used by this class or null if no mapping exists
     * @hide
     */
    CARAPI GetKeyFromMetadataEditorKey(
        /* [in] */ Int32 editorKey,
        /* [out] */ String* result);
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_CMEDIAMETADATAHELPER_H__
