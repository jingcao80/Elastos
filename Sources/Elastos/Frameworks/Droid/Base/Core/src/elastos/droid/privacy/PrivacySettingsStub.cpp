
#include "elastos/droid/privacy/PrivacySettingsStub.h"

namespace Elastos {
namespace Droid {
namespace Privacy {

const Boolean PrivacySettingsStub::sIsStub = TRUE;

CAR_INTERFACE_IMPL(PrivacySettingsStub, IInterface);

Boolean PrivacySettingsStub::IsStub()
{
    return sIsStub;
}

} // namespace Privacy
} // namespace Droid
} // namespace Elastos
