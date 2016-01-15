
#ifndef __ELASTOS_DROID_VIEW_CKEYCHARACTERMAP_H__
#define __ELASTOS_DROID_VIEW_CKEYCHARACTERMAP_H__

#include "_Elastos_Droid_View_CKeyCharacterMap.h"
#include "elastos/droid/view/KeyCharacterMap.h"

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CKeyCharacterMap)
    , public KeyCharacterMap
{
public:
    CAR_OBJECT_DECL();
};

}   //namespace View
}   //namespace Droid
}   //namespace Elastos

#endif //__ELASTOS_DROID_VIEW_CKEYCHARACTERMAP_H__
