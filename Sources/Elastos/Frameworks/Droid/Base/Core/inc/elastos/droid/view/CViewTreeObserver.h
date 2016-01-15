
#ifndef _ELASTOS_DROID_VIEW_CVIEWTREEOBSERVER_H_
#define _ELASTOS_DROID_VIEW_CVIEWTREEOBSERVER_H_

#include "_Elastos_Droid_View_CViewTreeObserver.h"
#include "elastos/droid/view/ViewTreeObserver.h"

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CViewTreeObserver)
    , public ViewTreeObserver
{
public:
    CAR_OBJECT_DECL()
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_VIEW_CVIEWTREEOBSERVER_H_

