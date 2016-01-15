
#ifndef __ELASTOS_DROID_VIEW_CDISPATCHERSTATE_H__
#define __ELASTOS_DROID_VIEW_CDISPATCHERSTATE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_View_CDispatcherState.h"
#include "elastos/droid/view/KeyEvent.h"

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CDispatcherState)
    , public KeyEvent::DispatcherState
{
public:
    CAR_OBJECT_DECL();
};

}   //namespace View
}   //namespace Droid
}   //namespace Elastos

#endif //__ELASTOS_DROID_VIEW_CDISPATCHERSTATE_H__
