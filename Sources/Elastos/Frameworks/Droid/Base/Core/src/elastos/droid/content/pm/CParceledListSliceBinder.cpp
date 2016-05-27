
#include "elastos/droid/content/pm/CParceledListSliceBinder.h"
#include "elastos/droid/content/pm/CParceledListSlice.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CAR_OBJECT_IMPL(CParceledListSliceBinder)
CParceledListSliceBinder::CParceledListSliceBinder()
    : mN(0)
{}

ECode CParceledListSliceBinder::constructor(
    /* [in] */ IInterface* obj,
    /* [in] */ IList* list,
    /* [in] */ Int32 N)
{
    mObj = obj;
    mList = list;
    mN = N;
    return NOERROR;
}

ECode CParceledListSliceBinder::Transact(
    /* [in] */ Int32 code,
    /* [in] */ IParcel* data,
    /* [in] */ IParcel* reply) /*throws RemoteException*/
{
    // if (code != FIRST_CALL_TRANSACTION) {
    //     return super.onTransact(code, data, reply, flags);
    // }
    Int32 i = 0, dataSize = 0;
    data->ReadInt32(&i);
    if (CParceledListSlice::DEBUG) {
        Logger::D(CParceledListSlice::TAG, "Writing more @%d of %d", i, mN);
    }
    reply->GetElementSize(&dataSize);
    while (i < mN && dataSize < CParceledListSlice::MAX_IPC_SIZE) {
        reply->WriteInt32(1);
        AutoPtr<IInterface> parcelable;
        mList->Get(i, (IInterface**)&parcelable);
        if (FAILED(CParceledListSlice::VerifySameType(mObj, parcelable))) {
            assert(0 && "Can't unparcel type.");
        }
        IParcelable::Probe(parcelable)->WriteToParcel(reply);
        if (CParceledListSlice::DEBUG) {
            Logger::D(CParceledListSlice::TAG, "Wrote extra #%d: %s", i, TO_CSTR(parcelable));
        }
        i++;
    }
    if (i < mN) {
        if (CParceledListSlice::DEBUG) {
            Logger::D(CParceledListSlice::TAG, "Breaking @%d of %d", i, mN);
        }
        reply->WriteInt32(0);
    }
    return NOERROR;
}

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos
