
#ifndef __ELASTOS_DROID_MEDIA_CMINITHUMBFILEHELPER_H__
#define __ELASTOS_DROID_MEDIA_CMINITHUMBFILEHELPER_H__

#include "_Elastos_Droid_Media_CMiniThumbFileHelper.h"
#include "Elastos.Droid.Media.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CMiniThumbFileHelper)
    , public Singleton
    , public IMiniThumbFileHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * We store different types of thumbnails in different files. To remain backward compatibility,
     * we should hashcode of content://media/external/images/media remains the same.
     */
    CARAPI Reset();

    CARAPI Instance(
        /* [in] */ IUri* uri,
        /* [out] */ IMiniThumbFile** result);
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CMINITHUMBFILEHELPER_H__
