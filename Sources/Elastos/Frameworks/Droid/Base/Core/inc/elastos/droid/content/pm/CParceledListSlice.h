
#ifndef __ELASTOS_DROID_CONTENT_PM_CPARCELEDLISTSLICE_H__
#define __ELASTOS_DROID_CONTENT_PM_CPARCELEDLISTSLICE_H__

#include "_Elastos_Droid_Content_Pm_CParceledListSlice.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

/**
 * Transfer a large list of Parcelable objects across an IPC.  Splits into
 * multiple transactions if needed.
 *
 * Caveat: for efficiency and security, all elements must be the same concrete type.
 * In order to avoid writing the class name of each object, we must ensure that
 * each object is the same type, or else unparceling then reparceling the data may yield
 * a different result if the class name encoded in the Parcelable is a Base type.
 * See b/17671747.
 *
 * @hide
 */
CarClass(CParceledListSlice)
    , public Object
    , public IParceledListSlice
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CParceledListSlice();

    virtual ~CParceledListSlice();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IList* list);

    CARAPI constructor(
        /* [in] */ IParcel* p,
        /* [in] */ Int32 numItems,
        /* [in] */ Boolean lastSlice);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    /**
     * Write this to another Parcel. Note that this discards the internal Parcel
     * and should not be used anymore. This is so we can pass this to a Binder
     * where we won't have a chance to call recycle on this.
     */
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetList(
        /* [out] */ IList** list);

private:
    static CARAPI VerifySameType(
        /* [in] */ IInterface* expected,
        /* [in] */ IInterface* actual);

private:
    static String TAG;// = "ParceledListSlice";
    static Boolean DEBUG;// = FALSE;

    /*
     * TODO get this number from somewhere else. For now set it to a quarter of
     * the 1MB limit.
     */
    static const Int32 MAX_IPC_SIZE;// = 256 * 1024;
    static const Int32 MAX_FIRST_IPC_SIZE;// = MAX_IPC_SIZE / 2;

    Int32 mNumItems;

    Boolean mIsLastSlice;

    AutoPtr<IList> mList;
    friend class CParceledListSliceBinder;
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_CPARCELEDLISTSLICE_H__
