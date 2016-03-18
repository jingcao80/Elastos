
#ifndef __ELASTOS_DROID_CONTENT_RES_CAPPTHEME_H__
#define __ELASTOS_DROID_CONTENT_RES_CAPPTHEME_H__

#include "_Elastos_Droid_Content_Res_CAppTheme.h"
#include "elastos/droid/content/res/ThemeConfig.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

CarClass(CAppTheme)
    , public ThemeConfig::AppTheme
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_RES_CAPPTHEME_H__
