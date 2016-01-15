
#ifndef __ELASTOS_DROID_APP_CFASTBITMAPDRAWABLE_H__
#define __ELASTOS_DROID_APP_CFASTBITMAPDRAWABLE_H__

#include "_Elastos_Droid_App_CFastBitmapDrawable.h"
#include "elastos/droid/app/FastBitmapDrawable.h"

namespace Elastos {
namespace Droid {
namespace App {

/**
 * Special drawable that draws a wallpaper as fast as possible.  Assumes
 * no scaling or placement off (0,0) of the wallpaper (this should be done
 * at the time the bitmap is loaded).
 */
CarClass(CFastBitmapDrawable)
    , public FastBitmapDrawable
{
public:
    CAR_OBJECT_DECL()
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_CFASTBITMAPDRAWABLE_H__
