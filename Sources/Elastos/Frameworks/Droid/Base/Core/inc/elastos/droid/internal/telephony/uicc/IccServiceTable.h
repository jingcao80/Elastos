#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCSERVICETABLE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCSERVICETABLE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

// import android.telephony.Rlog;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

/**
  * Wrapper class for an ICC EF containing a bit field of enabled services.
  */
class IccServiceTable
    : public Object
    , public IIccServiceTable
{
public:
    CAR_INTERFACE_DECL();

    IccServiceTable();

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* table);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

protected:
    // Get the class name to use for log strings
    virtual CARAPI_(String) GetTag() = 0;

    // Get the array of enums to use for toString
    virtual CARAPI_(AutoPtr<ArrayOf<IInterface*> >) GetValues() = 0;

    /**
      * Returns if the specified service is available.
      * @param service the service number as a zero-based offset (the enum ordinal)
      * @return true if the service is available; false otherwise
      */
    virtual CARAPI_(Boolean) IsAvailable(
        /* [in] */ Int32 service);

protected:
    /*const*/ AutoPtr< ArrayOf<Byte> > mServiceTable;
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCSERVICETABLE_H__

