
#include "Elastos.Droid.Content.h"
#include "elastos/droid/os/CSystemProperties.h"
#include "elastos/droid/os/SystemProperties.h"

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL(CSystemProperties, Singleton, ISystemProperties)

CAR_SINGLETON_IMPL(CSystemProperties)

ECode CSystemProperties::Get(
    /* [in] */ const String& key,
    /* [out] */ String* value)
{
    return SystemProperties::Get(key, value);
}

ECode CSystemProperties::Get(
    /* [in] */ const String& key,
    /* [in] */ const String& def,
    /* [out] */ String* value)
{
    return SystemProperties::Get(key, def, value);
}

ECode CSystemProperties::GetInt32(
    /* [in] */ const String& key,
    /* [in] */ Int32 def,
    /* [out] */ Int32* value)
{
    return SystemProperties::GetInt32(key, def, value);
}

ECode CSystemProperties::GetInt64(
    /* [in] */ const String& key,
    /* [in] */ Int64 def,
    /* [out] */ Int64* value)
{
    return SystemProperties::GetInt64(key, def, value);
}

ECode CSystemProperties::GetBoolean(
    /* [in] */ const String& key,
    /* [in] */ Boolean def,
    /* [out] */ Boolean* value)
{
    return SystemProperties::GetBoolean(key, def, value);
}

ECode CSystemProperties::Set(
    /* [in] */ const String& key,
    /* [in] */ const String& val)
{
    return SystemProperties::Set(key, val);
}

ECode CSystemProperties::AddChangeCallback(
    /* [in] */ IRunnable* cb)
{
    return SystemProperties::AddChangeCallback(cb);
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
