
#include "CViewDragHelperHelper.h"
#include "ViewDragHelper.h"

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace Widget {

CAR_INTERFACE_IMPL(CViewDragHelperHelper, Singleton, IViewDragHelperHelper)

CAR_SINGLETON_IMPL(CViewDragHelperHelper)

ECode CViewDragHelperHelper::Create(
    /* [in] */ IViewGroup* forParent,
    /* [in] */ IViewDragHelperCallback* cb,
    /* [out] */ IViewDragHelper** result)
{
    VALUE_NOT_NULL(result);

    if (forParent == NULL) {
        Logger::E(String("CViewDragHelperHelper"), "Parent view may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("Parent view may not be null");
    }
    if (cb == NULL) {
        Logger::E(String("CViewDragHelperHelper"), "Callback may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("Callback may not be null");
    }

    AutoPtr<IViewDragHelper> helper = ViewDragHelper::Create(forParent, cb);
    *result = helper;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CViewDragHelperHelper::Create(
    /* [in] */ IViewGroup* forParent,
    /* [in] */ Float sensitivity,
    /* [in] */ IViewDragHelperCallback* cb,
    /* [out] */ IViewDragHelper** result)
{
    VALUE_NOT_NULL(result);

    if (forParent == NULL) {
        Logger::E(String("CViewDragHelperHelper"), "Parent view may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("Parent view may not be null");
    }
    if (cb == NULL) {
        Logger::E(String("CViewDragHelperHelper"), "Callback may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("Callback may not be null");
    }

    AutoPtr<IViewDragHelper> helper = ViewDragHelper::Create(forParent, sensitivity, cb);
    *result = helper;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // Widget
} // Dialer
} // Apps
} // Elastos
