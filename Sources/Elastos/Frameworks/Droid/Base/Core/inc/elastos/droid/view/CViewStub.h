
#ifndef __ELASTOS_DROID_VIEW_CVIEWSTUB_H__
#define __ELASTOS_DROID_VIEW_CVIEWSTUB_H__

#include "_Elastos_Droid_View_CViewStub.h"
#include "elastos/droid/view/ViewStub.h"

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CViewStub)
    , public ViewStub
{
public:
    CAR_OBJECT_DECL();
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_CVIEWSTUB_H__
