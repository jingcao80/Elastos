
#ifndef __ELASTOS_DROID_MEDIA_CMEDIAROUTERSTATICAUDIOROUTESOBSERVER_H__
#define __ELASTOS_DROID_MEDIA_CMEDIAROUTERSTATICAUDIOROUTESOBSERVER_H__

#include "_Elastos_Droid_Media_CMediaRouterStaticAudioRoutesObserver.h"
#include "elastos/droid/media/CMediaRouter.h"

namespace Elastos{
namespace Droid {
namespace Media {

CarClass(CMediaRouterStaticAudioRoutesObserver)
    , public CMediaRouter::Static::MyAudioRoutesObserver
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Elastos
} // namespace Droid
} // namespace Media

#endif // __ELASTOS_DROID_MEDIA_CMEDIAROUTERSTATICAUDIOROUTESOBSERVER_H__
