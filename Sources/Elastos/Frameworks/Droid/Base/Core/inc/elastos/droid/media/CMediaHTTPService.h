#ifndef __ELASTOS_DROID_MEDIA_CMEDIAHTTPSERVICE_H__
#define __ELASTOS_DROID_MEDIA_CMEDIAHTTPSERVICE_H__

#include "_Elastos_Droid_Media_CMediaHTTPService.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * @hide
 */
CarClass(CMediaHTTPService)
    , public Object
    , public IIMediaHTTPService
    , public IBinder
{
public:
    CMediaHTTPService();

    virtual ~CMediaHTTPService();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI MakeHTTPConnection(
        /* [out] */ IIMediaHTTPConnection** result);

    static CARAPI CreateHttpServiceBinderIfNecessary(
        /* [in] */ const String& path,
        /* [out] */ IBinder** result);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    static const String TAG; // = "MediaHTTPService";
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_CMEDIAHTTPSERVICE_H__
