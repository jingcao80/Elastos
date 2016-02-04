
#ifndef __ELASTOS_DROID_CONTENT_RES_CCOLORSTATELISTHELPER_H__
#define __ELASTOS_DROID_CONTENT_RES_CCOLORSTATELISTHELPER_H__

#include "_Elastos_Droid_Content_Res_CColorStateListHelper.h"
#include "elastos/droid/content/res/CColorStateList.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::Res::IColorStateList;
using Elastos::Droid::Content::Res::IResources;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

CarClass(CColorStateListHelper)
    , public Singleton
    , public IColorStateListHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI ValueOf(
        /* [in] */ Int32 color,
        /* [out] */ IColorStateList** ppList);

    CARAPI CreateFromXml(
        /* [in] */ IResources* pR,
        /* [in] */ IXmlPullParser* pParser,
        /* [out] */ IColorStateList** ppList);

};

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_RES_CCOLORSTATELISTHELPER_H__
