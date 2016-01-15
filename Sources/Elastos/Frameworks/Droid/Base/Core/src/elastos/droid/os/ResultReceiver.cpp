
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/os/ResultReceiver.h"
#include <elastos/core/AutoLock.h>

using Elastos::Droid::Internal::Os::EIID_IIResultReceiver;

namespace Elastos {
namespace Droid {
namespace Os {

ResultReceiver::MyRunnable::MyRunnable(
    /* [in] */ Int32 resultCode,
    /* [in] */ IBundle* resultData,
    /* [in] */ IWeakReference* wr)
    : mResultCode(resultCode)
    , mResultData(resultData)
    , mWeakHost(wr)
{
}

ECode ResultReceiver::MyRunnable::Run()
{
    AutoPtr<IResultReceiver> rrObj;
    mWeakHost->Resolve(EIID_IResultReceiver, (IInterface**)&rrObj);
    if (rrObj == NULL) {
        return NOERROR;
    }

    ResultReceiver* rr = (ResultReceiver*)rrObj.Get();
    rr->OnReceiveResult(mResultCode, mResultData);
    return NOERROR;
}

CAR_INTERFACE_IMPL(ResultReceiver::MyResultReceiver, Object, IIResultReceiver)

ResultReceiver::MyResultReceiver::MyResultReceiver(
    /* [in] */ IWeakReference* wr)
    : mWeakHost(wr)
{
}

ECode ResultReceiver::MyResultReceiver::Send(
    /* [in] */ Int32 resultCode,
    /* [in] */ IBundle* resultData)
{
    AutoPtr<IResultReceiver> rrObj;
    mWeakHost->Resolve(EIID_IResultReceiver, (IInterface**)&rrObj);
    if (rrObj == NULL) {
        return NOERROR;
    }

    ResultReceiver* rr = (ResultReceiver*)rrObj.Get();
    if (rr->mHandler != NULL) {
        AutoPtr<IRunnable> mr = new MyRunnable(resultCode, resultData, mWeakHost);
        Boolean bval;
        rr->mHandler->Post(mr, &bval);
    }
    else {
        rr->OnReceiveResult(resultCode, resultData);
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL_2(ResultReceiver, Object, IResultReceiver, IParcelable)

ResultReceiver::ResultReceiver()
    : mLocal(FALSE)
{}

ECode ResultReceiver::constructor()
{
    return NOERROR;
}

ECode ResultReceiver::constructor(
    /* [in] */ IHandler* handler)
{
    mLocal = TRUE;
    mHandler = handler;
    return NOERROR;
}

ECode ResultReceiver::Send(
    /* [in] */ Int32 resultCode,
    /* [in] */ IBundle* resultData)
{
    if (mLocal) {
        if (mHandler != NULL) {
            AutoPtr<IWeakReference> wr;
            GetWeakReference((IWeakReference**)&wr);
            AutoPtr<IRunnable> mr = new MyRunnable(resultCode, resultData, wr);
            Boolean bval;
            mHandler->Post(mr, &bval);
        }
        else {
            OnReceiveResult(resultCode, resultData);
        }
        return NOERROR;
    }

    if (mReceiver != NULL) {
        // try {
            mReceiver->Send(resultCode, resultData);
        // } catch (RemoteException e) {
        // }
    }
    return NOERROR;
}

ECode ResultReceiver::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    AutoPtr<IInterface> obj;
    source->ReadInterfacePtr((Handle32*)(IInterface**)&obj);
    mReceiver = (MyResultReceiver*)IObject::Probe(obj);
    return NOERROR;
}

ECode ResultReceiver::WriteToParcel(
    /* [in] */ IParcel* out)
{
    synchronized(this) {
        if (mReceiver == NULL) {
            AutoPtr<IWeakReference> wr;
            GetWeakReference((IWeakReference**)&wr);
            mReceiver = new MyResultReceiver(wr);
        }
        out->WriteInterfacePtr(mReceiver);
    }
    return NOERROR;
}


} // namespace Os
} // namespace Droid
} // namespace Elastos
