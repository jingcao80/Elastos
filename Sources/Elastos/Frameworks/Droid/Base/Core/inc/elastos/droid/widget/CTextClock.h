#ifndef __ELASTOS_DROID_WIDGET_CTEXTCLOCK_H__
#define __ELASTOS_DROID_WIDGET_CTEXTCLOCK_H__

#include "_Elastos_Droid_Widget_CTextClock.h"
#include "elastos/droid/widget/TextClock.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CTextClock), public TextClock
{
public:
    CAR_OBJECT_DECL();
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_CTEXTCLOCK_H__
