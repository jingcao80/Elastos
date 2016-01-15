
#ifndef __ELASTOS_DROID_TELEPHONY_CELLSIGNALSTRENGTH_H__
#define __ELASTOS_DROID_TELEPHONY_CELLSIGNALSTRENGTH_H__

#include "Elastos.Droid.Core_server.h"

namespace Elastos {
namespace Droid {
namespace Telephony {

class CellSignalStrength
{
public:
    virtual CARAPI_(PInterface) Probe(
            /* [in]  */ REIID riid) = 0;

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
