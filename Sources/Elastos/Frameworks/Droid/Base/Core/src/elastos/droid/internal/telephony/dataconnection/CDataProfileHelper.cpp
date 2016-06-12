
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/internal/telephony/dataconnection/CDataProfile.h"
#include "elastos/droid/internal/telephony/dataconnection/CDataProfileHelper.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

CAR_INTERFACE_IMPL(CDataProfileHelper, Singleton, IDataProfileHelper)

CAR_SINGLETON_IMPL(CDataProfileHelper)

ECode CDataProfileHelper::ToParcel(
    /* [in] */ IParcel* pc,
    /* [in] */ ArrayOf<IDataProfile*>* dps,
    /* [out] */ IParcel** result)
{
    return CDataProfile::ToParcel(pc, dps, result);
}

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
