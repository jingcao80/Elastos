
#ifndef __ELASTOS_DROID_MEDIA_CMEDIAROUTERSTATICCLIENT_H__
#define __ELASTOS_DROID_MEDIA_CMEDIAROUTERSTATICCLIENT_H__

#include "_Elastos_Droid_Media_CMediaRouterStaticClient.h"
#include "elastos/droid/media/CMediaRouter.h"

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CMediaRouterStaticClient)
    , public CMediaRouter::Static::MyStaticClient
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Elastos
} // namespace Droid
} // namespace Media

#endif // __ELASTOS_DROID_MEDIA_CMEDIAROUTERSTATICCLIENT_H__
