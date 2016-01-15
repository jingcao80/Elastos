#ifndef __ELASTOS_DROID_MEDIA_CWEBVTTRENDERINGWIDGET_H__
#define __ELASTOS_DROID_MEDIA_CWEBVTTRENDERINGWIDGET_H__

#include "_Elastos_Droid_Media_CWebVttRenderingWidget.h"
#include "elastos/droid/media/WebVttRenderingWidget.h"

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CWebVttRenderingWidget)
    , public WebVttRenderingWidget
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CWEBVTTRENDERINGWIDGET_H__