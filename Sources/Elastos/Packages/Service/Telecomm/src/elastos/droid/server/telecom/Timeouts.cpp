
#include "elastos/droid/server/telecom/Timeouts.h"
#include <elastos/droid/provider/Settings.h>

using Elastos::Droid::Provider::Settings;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

const String Timeouts::PREFIX("telecom.");

Timeouts::Timeouts()
{}

ECode Timeouts::Get(
    /* [in] */ IContentResolver* contentResolver,
    /* [in] */ const String& key,
    /* [in] */ Int64 defaultValue,
    /* [out] */ Int64* result)
{
    return Settings::Secure::GetInt64(contentResolver, PREFIX + key, defaultValue, result);
}

ECode Timeouts::GetDirectToVoicemailMillis(
    /* [in] */ IContentResolver* contentResolver,
    /* [out] */ Int64* result)
{
    return Get(contentResolver, String("direct_to_voicemail_ms"), 500L, result);
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
