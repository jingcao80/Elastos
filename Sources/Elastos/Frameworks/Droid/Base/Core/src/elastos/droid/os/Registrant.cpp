
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/os/Registrant.h"
#include "elastos/droid/os/CMessage.h"

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL(Registrant, Object, IRegistrant)

ECode Registrant::constructor(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    IWeakReferenceSource* wrs = IWeakReferenceSource::Probe(h);
    assert(wrs);
    wrs->GetWeakReference((IWeakReference**)&mRefH);
    mWhat = what;
    mUserObj = obj;
    return NOERROR;
}

ECode Registrant::Clear()
{
    mRefH = NULL;
    mUserObj = NULL;
    return NOERROR;
}

ECode Registrant::NotifyRegistrant()
{
    InternalNotifyRegistrant (NULL, NULL);
    return NOERROR;
}

ECode Registrant::NotifyResult(
    /* [in] */ IInterface* result)
{
    InternalNotifyRegistrant (result, NULL);
    return NOERROR;
}

ECode Registrant::NotifyException(
    /* [in] */ IThrowable* exception)
{
    InternalNotifyRegistrant (NULL, exception);
    return NOERROR;
}

/**
 * This makes a copy of @param ar
 */
ECode Registrant::NotifyRegistrant(
    /* [in] */ IAsyncResult* ar)
{
    InternalNotifyRegistrant (((AsyncResult*) ar)->mResult, ((AsyncResult*) ar)->mException);
    return NOERROR;
}

/*package*/
void Registrant::InternalNotifyRegistrant(
    /* [in] */ IInterface* result,
    /* [in] */ IThrowable* exception)
{
    AutoPtr<IHandler> h;
    GetHandler((IHandler**)&h);

    if (h == NULL) {
        Clear();
    }
    else {
        AutoPtr<IMessage> msg = CMessage::Obtain();

        msg->SetWhat(mWhat);

        AutoPtr<AsyncResult> ar = new AsyncResult(mUserObj, result, exception);
        msg->SetObj(TO_IINTERFACE(ar));

        Boolean result;
        h->SendMessage(msg, &result);
    }
}

/**
 * NOTE: May return NULL if weak reference has been collected
 */
ECode Registrant::MessageForRegistrant(
    /* [out] */ IMessage** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IHandler> h;
    GetHandler((IHandler**)&h);

    if (h == NULL) {
        Clear();

        *result = NULL;
        return NOERROR;
    }
    else {
        AutoPtr<IMessage> msg;
        h->ObtainMessage((IMessage**)&msg);

        msg->SetWhat(mWhat);
        msg->SetObj(mUserObj);

        *result = msg;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    return NOERROR;
}

ECode Registrant::GetHandler(
    /* [out] */ IHandler** result)
{
    VALIDATE_NOT_NULL(result)

    if (mRefH == NULL) {
        *result = NULL;
        return NOERROR;
    }

    AutoPtr<IHandler> handler;
    mRefH->Resolve(EIID_IHandler, (IInterface**)&handler);
    *result = handler;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
