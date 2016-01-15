
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/CPointerCoordsHelper.h"
#include "elastos/droid/view/MotionEvent.h"

namespace Elastos {
namespace Droid {
namespace View {

CAR_INTERFACE_IMPL(CPointerCoordsHelper, Singleton, IPointerCoordsHelper);

CAR_SINGLETON_IMPL(CPointerCoordsHelper);

ECode CPointerCoordsHelper::CreateArray(
    /* [in] */ Int32 size,
    /* [out, callee] */ ArrayOf<IPointerCoords*>** array)
{
    VALIDATE_NOT_NULL(array);
    AutoPtr< ArrayOf<IPointerCoords*> > _array = MotionEvent::PointerCoords::CreateArray(size);
    *array = _array;
    REFCOUNT_ADD(*array);
    return NOERROR;
}

} // View
} // Droid
} // Elastos
