
#ifndef __ELASTOS_DROID_CONTENT_RES_CSYSTEMAPPTHEME_H__
#define __ELASTOS_DROID_CONTENT_RES_CSYSTEMAPPTHEME_H__

#include "_Elastos_Droid_Content_Res_CSystemAppTheme.h"
#include "elastos/droid/content/res/CThemeConfig.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

CarClass(CSystemAppTheme)
    , public ThemeConfig::AppTheme
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI ToString(
        /* [out] */ String* str);
};

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_RES_CSYSTEMAPPTHEME_H__
