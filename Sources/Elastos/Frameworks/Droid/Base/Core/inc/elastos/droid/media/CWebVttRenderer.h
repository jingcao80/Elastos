#ifndef __ELASTOS_DROID_MEDIA_CWEBVTTRENDERER_H__
#define __ELASTOS_DROID_MEDIA_CWEBVTTRENDERER_H__

#include "_Elastos_Droid_Media_CWebVttRenderer.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Media.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CWebVttRenderer)
    , public Object
    , public ISubtitleControllerRenderer
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CWebVttRenderer();

    virtual ~CWebVttRenderer();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI Supports(
        /* [in] */ IMediaFormat* format,
        /* [out] */ Boolean* result);

    CARAPI CreateTrack(
        /* [in] */ IMediaFormat* format,
        /* [out] */ ISubtitleTrack** result);

private:
    AutoPtr<IContext> mContext;
    AutoPtr<IWebVttRenderingWidget> mRenderingWidget;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CWEBVTTRENDERER_H__