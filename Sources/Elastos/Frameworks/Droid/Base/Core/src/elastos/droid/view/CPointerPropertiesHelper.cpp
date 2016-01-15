
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/CPointerPropertiesHelper.h"
#include "elastos/droid/view/MotionEvent.h"

namespace Elastos {
namespace Droid {
namespace View {

CAR_INTERFACE_IMPL(CPointerPropertiesHelper, Singleton, IPointerPropertiesHelper);

CAR_SINGLETON_IMPL(CPointerPropertiesHelper);

ECode CPointerPropertiesHelper::CreateArray(
    /* [in] */ Int32 size,
    /* [out, callee] */ ArrayOf<IPointerProperties*>** array)
{
    VALIDATE_NOT_NULL(array);
    AutoPtr< ArrayOf<IPointerProperties*> > _array = MotionEvent::PointerProperties::CreateArray(size);
    *array = _array;
    REFCOUNT_ADD(*array);
    return NOERROR;
}

} // View
} // Droid
} // Elastos
