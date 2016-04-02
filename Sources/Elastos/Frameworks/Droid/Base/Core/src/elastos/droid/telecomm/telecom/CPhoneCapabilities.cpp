
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecomm/telecom/CPhoneCapabilities.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

//===============================================================
// CPhoneCapabilities::
//===============================================================
CAR_SINGLETON_IMPL(CPhoneCapabilities)

CAR_INTERFACE_IMPL(CPhoneCapabilities, Singleton, IPhoneCapabilities)

ECode CPhoneCapabilities::ToString(
    /* [in] */ Int32 capabilities,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    StringBuilder builder;
    builder.Append("[Capabilities:");
    if ((capabilities & HOLD) != 0) {
        builder.Append(" HOLD");
    }
    if ((capabilities & SUPPORT_HOLD) != 0) {
        builder.Append(" SUPPORT_HOLD");
    }
    if ((capabilities & MERGE_CONFERENCE) != 0) {
        builder.Append(" MERGE_CONFERENCE");
    }
    if ((capabilities & SWAP_CONFERENCE) != 0) {
        builder.Append(" SWAP_CONFERENCE");
    }
    if ((capabilities & ADD_CALL) != 0) {
        builder.Append(" ADD_CALL");
    }
    if ((capabilities & RESPOND_VIA_TEXT) != 0) {
        builder.Append(" RESPOND_VIA_TEXT");
    }
    if ((capabilities & MUTE) != 0) {
        builder.Append(" MUTE");
    }
    if ((capabilities & MANAGE_CONFERENCE) != 0) {
        builder.Append(" MANAGE_CONFERENCE");
    }
    if ((capabilities & SUPPORTS_VT_LOCAL) != 0) {
        builder.Append(" SUPPORTS_VT_LOCAL");
    }
    if ((capabilities & SUPPORTS_VT_REMOTE) != 0) {
        builder.Append(" SUPPORTS_VT_REMOTE");
    }
    if ((capabilities & VoLTE) != 0) {
        builder.Append(" VoLTE");
    }
    if ((capabilities & VoWIFI) != 0) {
        builder.Append(" VoWIFI");
    }
    builder.Append("]");
    return builder.ToString(result);
}

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos
