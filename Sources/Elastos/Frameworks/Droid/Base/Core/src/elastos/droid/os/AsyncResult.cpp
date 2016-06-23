
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/os/AsyncResult.h"

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL(AsyncResult, Object, IAsyncResult)

/** please note, this sets m.obj to be this */
AsyncResult::AsyncResult(
    /* [in] */ IInterface* uo,
    /* [in] */ IInterface* r,
    /* [in] */ IThrowable* ex)
    : mUserObj(uo)
    , mException(ex)
    , mResult(r)
{
}

/** Saves and sets m.obj */
AutoPtr<AsyncResult> AsyncResult::ForMessage(
    /* [in] */ IMessage* m,
    /* [in] */ IInterface* r,
    /* [in] */ IThrowable* ex)
{
    AutoPtr<IInterface> obj;
    m->GetObj((IInterface**)&obj);
    AutoPtr<AsyncResult> ret = new AsyncResult(obj, r, ex);

    m->SetObj(TO_IINTERFACE(ret));

    return ret;
}

/** Saves and sets m.obj */
AutoPtr<AsyncResult> AsyncResult::ForMessage(
    /* [in] */ IMessage* m)
{
    AutoPtr<IInterface> obj;
    m->GetObj((IInterface**)&obj);
    AutoPtr<AsyncResult> ret = new AsyncResult(obj, NULL, NULL);

    m->SetObj(TO_IINTERFACE(ret));

    return ret;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
