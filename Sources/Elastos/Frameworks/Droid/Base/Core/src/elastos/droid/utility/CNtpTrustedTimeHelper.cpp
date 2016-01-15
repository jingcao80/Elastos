
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/utility/CNtpTrustedTimeHelper.h"
#include "elastos/droid/utility/NtpTrustedTime.h"

namespace Elastos {
namespace Droid {
namespace Utility {

CAR_INTERFACE_IMPL(CNtpTrustedTimeHelper, Singleton, INtpTrustedTimeHelper)

CAR_SINGLETON_IMPL(CNtpTrustedTimeHelper)

ECode CNtpTrustedTimeHelper::GetInstance(
    /* [in] */ IContext* context,
    /* [out] */ INtpTrustedTime** instance)
{
    return NtpTrustedTime::GetInstance(context, instance);
}

} // namespace Utility
} // namespace Droid
} // namespace Elastos
