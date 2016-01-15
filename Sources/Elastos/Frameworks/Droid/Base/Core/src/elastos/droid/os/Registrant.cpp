
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/os/Registrant.h"
#include "elastos/droid/os/CMessage.h"

namespace Elastos {
namespace Droid {
namespace Os {

Registrant::Registrant(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    IWeakReferenceSource* wrs = IWeakReferenceSource::Probe(h);
    assert(wrs);
    wrs->GetWeakReference((IWeakReference**)&mRefH);
    mWhat = what;
    mUserObj = obj;
}

void Registrant::Clear()
{
    mRefH = NULL;
    mUserObj = NULL;
}

void Registrant::NotifyRegistrant()
{
    InternalNotifyRegistrant (NULL, NULL);
}

void Registrant::NotifyResult(
    /* [in] */ IInterface* result)
{
    InternalNotifyRegistrant (result, NULL);
}

void Registrant::NotifyException(
    /* [in] */ IThrowable* exception)
{
    InternalNotifyRegistrant (NULL, exception);
}

/**
 * This makes a copy of @param ar
 */
void Registrant::NotifyRegistrant(
    /* [in] */ AsyncResult* ar)
{
    InternalNotifyRegistrant (ar->mResult, ar->mException);
}

/*package*/
void Registrant::InternalNotifyRegistrant(
    /* [in] */ IInterface* result,
    /* [in] */ IThrowable* exception)
{
    AutoPtr<IHandler> h = GetHandler();

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
AutoPtr<IMessage> Registrant::MessageForRegistrant()
{
    AutoPtr<IHandler> h = GetHandler();

    if (h == NULL) {
        Clear();

        return NULL;
    }
    else {
        AutoPtr<IMessage> msg;
        h->ObtainMessage((IMessage**)&msg);

        msg->SetWhat(mWhat);
        msg->SetObj(mUserObj);

        return msg;
    }
}

AutoPtr<IHandler> Registrant::GetHandler()
{
    if (mRefH == NULL)
        return NULL;

    AutoPtr<IHandler> handler;
    mRefH->Resolve(EIID_IHandler, (IInterface**)&handler);
    return handler;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
