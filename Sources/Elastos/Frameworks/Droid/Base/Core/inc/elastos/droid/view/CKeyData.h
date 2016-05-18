
#ifndef __ELASTOS_DROID_VIEW_CKEYDATA_H__
#define __ELASTOS_DROID_VIEW_CKEYDATA_H__

#include "_Elastos_Droid_View_CKeyData.h"
#include "elastos/droid/view/KeyCharacterMap.h"

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CKeyData)
    , public KeyCharacterMap::KeyData
{
public:
    CAR_OBJECT_DECL()
};

}   //namespace View
}   //namespace Droid
}   //namespace Elastos

#endif //__ELASTOS_DROID_VIEW_CKEYDATA_H__
