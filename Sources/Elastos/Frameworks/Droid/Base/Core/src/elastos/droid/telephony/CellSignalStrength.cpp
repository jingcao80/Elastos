
#include "CellSignalStrength.h"

namespace Elastos {
namespace Droid {
namespace Telephony {

static AutoPtr< ArrayOf<String> > InitSIGNAL_STRENGTH_NAMES() {
    AutoPtr< ArrayOf<String> > ssn = ArrayOf<String>::Alloc(5);
    (*ssn)[0] = String("none");
    (*ssn)[0] = String("poor");
    (*ssn)[0] = String("moderate");
    (*ssn)[0] = String("good");
    (*ssn)[0] = String("great");
    return ssn;
}

AutoPtr< ArrayOf<String> > CellSignalStrength::SIGNAL_STRENGTH_NAMES = InitSIGNAL_STRENGTH_NAMES();

ECode CellSignalStrength::SetDefaultValues()
{
    return NOERROR;
}

ECode CellSignalStrength::GetLevel(
    /* [out] */ Int32* level)
{
    return NOERROR;
}

ECode CellSignalStrength::GetAsuLevel(
    /* [out] */ Int32* asulevel)
{
    return NOERROR;
}

ECode CellSignalStrength::GetDbm(
    /* [out] */ Int32* dbm)
{
    return NOERROR;
}

ECode CellSignalStrength::Copy(
    /* [out] */ ICellSignalStrength** css)
{
    return NOERROR;
}

ECode CellSignalStrength::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    return NOERROR;
}

ECode CellSignalStrength::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* res)
{
    return NOERROR;
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

