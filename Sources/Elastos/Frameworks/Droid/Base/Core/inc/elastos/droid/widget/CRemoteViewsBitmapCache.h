#ifndef __ELASTOS_DROID_WIDGET_CREMOTEVIEWSBITMAPCACHE_H__
#define __ELASTOS_DROID_WIDGET_CREMOTEVIEWSBITMAPCACHE_H__

#include "_Elastos_Droid_Widget_CRemoteViewsBitmapCache.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::Droid::Graphics::IBitmap;

namespace Elastos {
namespace Droid {
namespace Widget {

class MemoryUsageCounter;

CarClass(CRemoteViewsBitmapCache)
{
public:
    CARAPI constructor();

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetBitmapId(
        /* [in] */ IBitmap* b,
        /* [out] */ Int32* id);

    CARAPI GetBitmapForId(
        /* [in] */ Int32 id,
        /* [out] */ IBitmap** bitmap);

    CARAPI Assimilate(
        /* [in] */ IRemoteViewsBitmapCache* bitmapCache);

    CARAPI_(void) AddBitmapMemory(
        /* [[in] */ MemoryUsageCounter* memoryCounter);

private:
    List<AutoPtr<IBitmap> > mBitmaps;
};


}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_CREMOTEVIEWSBITMAPCACHE_H__
