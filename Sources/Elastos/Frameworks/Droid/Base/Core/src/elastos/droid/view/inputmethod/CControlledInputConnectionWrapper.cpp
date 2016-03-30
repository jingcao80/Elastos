
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/view/inputmethod/CControlledInputConnectionWrapper.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::View::EIID_IIInputContext;
using Elastos::Droid::Os::EIID_IBinder;

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace View {
namespace InputMethod {

//========================================================================================
//              CControlledInputConnectionWrapper::
//========================================================================================

CAR_OBJECT_IMPL(CControlledInputConnectionWrapper)

CControlledInputConnectionWrapper::CControlledInputConnectionWrapper()
    : mActive(FALSE)
{
}

ECode CControlledInputConnectionWrapper::constructor(
    /* [in] */ ILooper* mainLooper,
    /* [in] */ IInputConnection* conn,
    /* [in] */ IInputMethodManager* agmer)
{
    mParentInputMethodManager = (CInputMethodManager*)agmer;
    mActive = TRUE;
    return IInputConnectionWrapper::constructor(mainLooper, conn);
}

ECode CControlledInputConnectionWrapper::IsActive(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mParentInputMethodManager->mActive && mActive;
    return NOERROR;
}

ECode CControlledInputConnectionWrapper::Deactivate()
{
    mActive = FALSE;
    return NOERROR;
}


} // namespace InputMethod
} // namespace View
} // namespace Droid
} // namespace Elastos
