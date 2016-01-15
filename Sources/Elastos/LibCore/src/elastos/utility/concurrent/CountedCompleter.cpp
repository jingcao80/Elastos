
#include "CountedCompleter.h"

namespace Elastos {
namespace Utility {
namespace Concurrent {

//====================================================================
// CountedCompleter::
//====================================================================
CAR_INTERFACE_IMPL(CountedCompleter, ForkJoinTask, ICountedCompleter)

CountedCompleter::CountedCompleter(
    /* [in] */ ICountedCompleter* completer,
    /* [in] */ Int32 initialPendingCount)
{
    mCompleter = completer;
    mPending = initialPendingCount;
}

CountedCompleter::CountedCompleter(
    /* [in] */ ICountedCompleter* completer)
{
    mCompleter = completer;
}

CountedCompleter::CountedCompleter()
{
    mCompleter = NULL;
}

ECode CountedCompleter::OnCompletion(
    /* [in] */ ICountedCompleter* caller)
{
    return NOERROR;
}

ECode CountedCompleter::OnExceptionalCompletion(
    /* [in] */ IThrowable* ex,
    /* [in] */ ICountedCompleter* caller,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    *res = TRUE;
    return NOERROR;
}

ECode CountedCompleter::GetCompleter(
    /* [out] */ ICountedCompleter** res)
{
    VALIDATE_NOT_NULL(res)

    *res = mCompleter;
    REFCOUNT_ADD(*res)
    return NOERROR;
}

ECode CountedCompleter::GetPendingCount(
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    *res = mPending;
    return NOERROR;
}

ECode CountedCompleter::SetPendingCount(
    /* [in] */ Int32 count)
{
    mPending = count;
    return NOERROR;
}

ECode CountedCompleter::AddToPendingCount(
    /* [in] */ Int32 delta)
{
//    Int32 c;
//    do {} while (!U.compareAndSwapInt(this, PENDING, c = pending, c+delta));
    return NOERROR;
}

ECode CountedCompleter::CompareAndSetPendingCount(
    /* [in] */ Int32 expected,
    /* [in] */ Int32 count,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    *res = FALSE;
    return NOERROR;
//    return U.compareAndSwapInt(this, PENDING, expected, count);
}

ECode CountedCompleter::DecrementPendingCountUnlessZero(
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    Int32 c = 0;
    // do {} while ((c = pending) != 0 &&
    //              !U.compareAndSwapInt(this, PENDING, c, c - 1));
    *res = c;
    return NOERROR;
}

ECode CountedCompleter::GetRoot(
    /* [out] */ ICountedCompleter** res)
{
    VALIDATE_NOT_NULL(res)

    AutoPtr<CountedCompleter> a = this, p;
    while ((p = (CountedCompleter*)(a->mCompleter.Get())) != NULL)
        a = p;
    *res = ICountedCompleter::Probe(a);
    REFCOUNT_ADD(*res)
    return NOERROR;
}

ECode CountedCompleter::TryComplete()
{
    AutoPtr<CountedCompleter> a = this, s = a;
    for (Int32 c;;) {
        if ((c = a->mPending) == 0) {
            a->OnCompletion(s);
            if ((a = (CountedCompleter*)((s = a)->mCompleter.Get())) == NULL) {
                s->QuietlyComplete();
                return NOERROR;
            }
        }
        // else if (U.compareAndSwapInt(a, PENDING, c, c - 1))
        //     return;
    }
    return NOERROR;
}

ECode CountedCompleter::PropagateCompletion()
{
    AutoPtr<CountedCompleter> a = this, s = a;
    for (Int32 c;;) {
        if ((c = a->mPending) == 0) {
            if ((a = (CountedCompleter*)((s = a)->mCompleter.Get())) == NULL) {
                s->QuietlyComplete();
                return NOERROR;
            }
        }
        // else if (U.compareAndSwapInt(a, PENDING, c, c - 1))
        //     return;
    }
    return NOERROR;
}

ECode CountedCompleter::Complete(
    /* [in] */ IInterface* value)
{
    AutoPtr<CountedCompleter> p;
    SetRawResult(value);
    OnCompletion(this);
    QuietlyComplete();
    if ((p = (CountedCompleter*)mCompleter.Get()) != NULL)
        p->TryComplete();
    return NOERROR;
}

ECode CountedCompleter::FirstComplete(
    /* [out] */ ICountedCompleter** res)
{
    VALIDATE_NOT_NULL(res)

    for (Int32 c;;) {
        if ((c = mPending) == 0) {
            *res = this;
            REFCOUNT_ADD(*res)
            return NOERROR;
        }
        // else if (U.compareAndSwapInt(this, PENDING, c, c - 1))
        //     return NULL;
    }
    return NOERROR;
}

ECode CountedCompleter::NextComplete(
    /* [out] */ ICountedCompleter** res)
{
    VALIDATE_NOT_NULL(res)

    AutoPtr<ICountedCompleter> p;
    if ((p = mCompleter) != NULL) {
        return p->FirstComplete(res);
    }
    else {
        QuietlyComplete();
        *res = NULL;
        return NOERROR;
    }
}

ECode CountedCompleter::QuietlyCompleteRoot()
{
    for (AutoPtr<CountedCompleter> a = this, p;;) {
        if ((p = (CountedCompleter*)(a->mCompleter.Get())) == NULL) {
            return a->QuietlyComplete();
        }
        a = p;
    }
    return NOERROR;
}

void CountedCompleter::InternalPropagateException(
    /* [in] */ IThrowable* ex)
{
    AutoPtr<CountedCompleter> a = this, s = a;
    Boolean b = FALSE;
    while ((a->OnExceptionalCompletion(ex, s, &b), b) &&
           (a = (CountedCompleter*)((s = a)->mCompleter.Get())) != NULL && a->mStatus >= 0 &&
           a->RecordExceptionalCompletion(ex) == EXCEPTIONAL)
        ;
}

ECode CountedCompleter::Exec(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    Compute();
    *res = FALSE;
    return NOERROR;
}

ECode CountedCompleter::GetRawResult(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)

    *outface = NULL;
    return NOERROR;
}

ECode CountedCompleter::SetRawResult(
    /* [in] */ IInterface* value)
{
    return NOERROR;
}

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
