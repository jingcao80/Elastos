
#ifndef __ELASTOS_DROID_WIDGET_CSPELLCHECKER_H__
#define __ELASTOS_DROID_WIDGET_CSPELLCHECKER_H__

#include "_Elastos_Droid_Widget_CSpellChecker.h"
#include "elastos/droid/widget/SpellChecker.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CSpellChecker)
    , public SpellChecker
{
public:
    CAR_OBJECT_DECL()
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_CSPELLCHECKER_H__

