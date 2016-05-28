
#ifndef __ELASTOS_DROID_OS_REGISTRANTLIST_H__
#define __ELASTOS_DROID_OS_REGISTRANTLIST_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/os/Registrant.h"

using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Os {

/** @hide */
class RegistrantList
    : public Object
{
public:
    RegistrantList();

    //synchronized
    CARAPI_(void) Add(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    //synchronized
    CARAPI_(void) AddUnique(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    //synchronized
    CARAPI_(void) Add(
        /* [in] */ Registrant* r);

    //synchronized
    CARAPI_(void) RemoveCleared();

    //synchronized
    CARAPI_(Int32) GetSize();

    //synchronized
    CARAPI_(AutoPtr<IInterface>) Get(
        /* [in] */ Int32 index);

    CARAPI_(void) NotifyRegistrants();

    CARAPI_(void) NotifyException(
        /* [in] */ IThrowable* exception);

    CARAPI_(void) NotifyResult(
        /* [in] */ IInterface* result);

    CARAPI_(void) NotifyRegistrants(
        /* [in] */ AsyncResult* ar);

    // synchronized
    CARAPI_(void) Remove(
        /* [in] */ IHandler* h);

private:
    // synchronized
    CARAPI_(void) InternalNotifyRegistrants(
        /* [in] */ IInterface* result,
        /* [in] */ IThrowable* exception);

private:
    AutoPtr<IArrayList> mRegistrants;      // of Registrant
    Object mLock;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_REGISTRANTLIST_H__
