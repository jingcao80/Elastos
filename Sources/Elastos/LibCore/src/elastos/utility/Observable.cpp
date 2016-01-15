
#include "Observable.h"
#include "CArrayList.h"
#include "AutoLock.h"

namespace Elastos {
namespace Utility {

CAR_INTERFACE_IMPL(Observable, Object, IObservable)

Observable::Observable()
    : mChanged(FALSE)
{}

Observable::~Observable()
{
}

ECode Observable::constructor()
{
    AutoPtr<IArrayList> outlist;
    FAIL_RETURN(CArrayList::New((IArrayList**)&outlist));
    mObservers = IList::Probe(outlist);
    mChanged = FALSE;
    return NOERROR;
}

ECode Observable::AddObserver(
    /* [in] */ IObserver* observer)
{
    if (observer == NULL) {
        // throw new NullPointerException("observer == null");
        return E_NULL_POINTER_EXCEPTION;
    }

    {
        AutoLock lock(this);
        Boolean isflag = FALSE;
        if (!(mObservers->Contains(observer, &isflag), isflag))
            mObservers->Add(observer, &isflag);
    }
    return NOERROR;
}

ECode Observable::CountObservers(
    /* [out] */ Int32* value)
{
    return mObservers->GetSize(value);
}

ECode Observable::DeleteObserver(
    /* [in] */ IObserver* observer)
{
    AutoLock lock(this);
    return mObservers->Remove(observer);
}

ECode Observable::DeleteObservers()
{
    AutoLock lock(this);
    return mObservers->Clear();
}

ECode Observable::HasChanged(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    *value = mChanged;
    return NOERROR;
}

ECode Observable::NotifyObservers()
{
    return NotifyObservers(NULL);
}

ECode Observable::NotifyObservers(
    /* [in] */ IInterface* data)
{
    Int32 size = 0;
    AutoPtr< ArrayOf<IInterface*> > arrays;
    {
        AutoLock lock(this);
        Boolean isflag = FALSE;
        if (HasChanged(&isflag), isflag) {
            ClearChanged();
            mObservers->GetSize(&size);
            mObservers->ToArray((ArrayOf<IInterface*>**)&arrays);
        }
    }
    if (arrays != NULL) {
        for (Int32 i = 0; i < arrays->GetLength(); i++) {
            AutoPtr<IObserver> observer = IObserver::Probe((*arrays)[i]);
            observer->Update(THIS_PROBE(IObservable), data);
        }
    }
    return NOERROR;
}

ECode Observable::ClearChanged()
{
    mChanged = FALSE;
    return NOERROR;
}

ECode Observable::SetChanged()
{
    mChanged = TRUE;
    return NOERROR;
}

} // namespace Utility
} // namespace Elastos
