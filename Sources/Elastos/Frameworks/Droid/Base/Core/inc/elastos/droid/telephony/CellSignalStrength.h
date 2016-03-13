
#ifndef __ELASTOS_DROID_TELEPHONY_CELLSIGNALSTRENGTH_H__
#define __ELASTOS_DROID_TELEPHONY_CELLSIGNALSTRENGTH_H__

#include "Elastos.Droid.Telephony.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Telephony {

class CellSignalStrength
    : public Object
    , public ICellSignalStrength
{
public:

    CAR_INTERFACE_DECL()

    CARAPI SetDefaultValues();

    CARAPI GetLevel(
        /* [out] */ Int32* level);

    CARAPI GetAsuLevel(
        /* [out] */ Int32* asulevel);

    CARAPI GetDbm(
        /* [out] */ Int32* dbm);

    CARAPI Copy(
        /* [out] */ ICellSignalStrength** css);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* res);

    static AutoPtr< ArrayOf<String> > SIGNAL_STRENGTH_NAMES;

};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CELLSIGNALSTRENGTH_H__
