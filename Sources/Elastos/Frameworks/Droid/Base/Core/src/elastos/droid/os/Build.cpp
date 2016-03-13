#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/CSystemProperties.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Os {

static String GetStringProperty(
    /* [in] */ const String& property,
    /* [in] */ const String& defaultValue)
{
    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    String value;
    sysProp->Get(property, defaultValue, &value);
    return value;
}

static String GetStringProperty(
    /* [in] */ const String& property)
{
    return GetStringProperty(property, String("unknown")/*UNKNOWN*/);
}

static AutoPtr<ArrayOf<String> > GetStringPropertyList(
    /* [in] */ const String& property,
    /* [in] */ const String& separator)
{
    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    String value;
    sysProp->Get(property, &value);

    AutoPtr<ArrayOf<String> > array;
    if (value.IsNullOrEmpty()) {
        array = ArrayOf<String>::Alloc(0);
    }
    else {
        StringUtils::Split(value, separator, (ArrayOf<String>**)&array);
    }
    return array;
}

static Int32 GetInt32Property(
    /* [in] */ const String& property)
{
    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    Int32 value;
    sysProp->GetInt32(property, 0, &value);
    return value;
}

static Int64 GetInt64Property(
    /* [in] */ const String& property)
{
    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    String retStr = GetStringProperty(property);
    sysProp->Get(property, String("unknown")/*UNKNOWN*/, &retStr);
    if (retStr.Equals("unknown"))
        return -1;

    return StringUtils::ParseInt64(retStr);
}

static AutoPtr<ArrayOf<String> > InitACTIVE_CODENAMES()
{
    AutoPtr<ArrayOf<String> > all = GetStringPropertyList(String("ro.build.version.all_codenames"), String(","));
    if (all != NULL && all->GetLength() > 0) {
        if ((*all)[0].Equals("REL")) {
            all = ArrayOf<String>::Alloc(0);
        }
    }

    return all;
}

static Int32 InitResourcesSDKInt()
{
    AutoPtr<ArrayOf<String> > array = InitACTIVE_CODENAMES();
    return GetInt32Property(String("ro.build.version.sdk"))
        +  array != NULL ? array->GetLength() : 0;
}

/**
 * Some devices split the fingerprint components between multiple
 * partitions, so we might derive the fingerprint at runtime.
 */
static String DeriveFingerprint() {
    String finger = GetStringProperty(String("ro.build.fingerprint"));
    if (finger.IsNullOrEmpty()) {
        StringBuilder sb;
        sb += GetStringProperty(String("ro.product.brand"));
        sb.AppendChar('/');
        sb += GetStringProperty(String("ro.product.name"));
        sb.AppendChar('/');
        sb += GetStringProperty(String("ro.product.device"));
        sb.AppendChar(':');
        sb += GetStringProperty(String("ro.build.version.release"));
        sb.AppendChar('/');
        sb += GetStringProperty(String("ro.build.id"));
        sb.AppendChar('/');
        sb += GetStringProperty(String("ro.build.version.incremental"));
        sb.AppendChar(':');
        sb += GetStringProperty(String("ro.build.type"));
        sb.AppendChar('/');
        sb += GetStringProperty(String("ro.build.tags"));
        finger = sb.ToString();
    }
    return finger;
}

const String Build::VERSION::INCREMENTAL = GetStringProperty(String("ro.build.version.incremental"));
const String Build::VERSION::RELEASE = GetStringProperty(String("ro.build.version.release"));
const String Build::VERSION::SDK = GetStringProperty(String("ro.build.version.sdk"));
const Int32 Build::VERSION::SDK_INT = GetInt32Property(String("ro.build.version.sdk"));
const String Build::VERSION::CODENAME = GetStringProperty(String("ro.build.version.codename"));
const Int32 Build::VERSION::RESOURCES_SDK_INT = InitResourcesSDKInt();

AutoPtr<ArrayOf<String> > Build::VERSION::ALL_CODENAMES
    = GetStringPropertyList(String("ro.build.version.all_codenames"), String(","));

AutoPtr<ArrayOf<String> > Build::VERSION::ACTIVE_CODENAMES = InitACTIVE_CODENAMES();

const Int32 Build::VERSION_CODES::CUR_DEVELOPMENT;
const Int32 Build::VERSION_CODES::BASE;
const Int32 Build::VERSION_CODES::BASE_1_1;
const Int32 Build::VERSION_CODES::CUPCAKE;
const Int32 Build::VERSION_CODES::DONUT;
const Int32 Build::VERSION_CODES::ECLAIR;
const Int32 Build::VERSION_CODES::ECLAIR_0_1;
const Int32 Build::VERSION_CODES::ECLAIR_MR1;
const Int32 Build::VERSION_CODES::FROYO;
const Int32 Build::VERSION_CODES::GINGERBREAD;
const Int32 Build::VERSION_CODES::GINGERBREAD_MR1;
const Int32 Build::VERSION_CODES::HONEYCOMB;
const Int32 Build::VERSION_CODES::HONEYCOMB_MR1;
const Int32 Build::VERSION_CODES::HONEYCOMB_MR2;
const Int32 Build::VERSION_CODES::ICE_CREAM_SANDWICH;
const Int32 Build::VERSION_CODES::ICE_CREAM_SANDWICH_MR1;
const Int32 Build::VERSION_CODES::JELLY_BEAN;
const Int32 Build::VERSION_CODES::JELLY_BEAN_MR1;
const Int32 Build::VERSION_CODES::JELLY_BEAN_MR2;
const Int32 Build::VERSION_CODES::KITKAT;
const Int32 Build::VERSION_CODES::KITKAT_WATCH;
const Int32 Build::VERSION_CODES::L;
const Int32 Build::VERSION_CODES::LOLLIPOP;

const String Build::TAG("Build");
const String Build::UNKNOWN("unknown");
const String Build::ID = GetStringProperty(String("ro.build.id"));
const String Build::DISPLAY = GetStringProperty(String("ro.build.display.id"));
const String Build::PRODUCT = GetStringProperty(String("ro.product.name"));
const String Build::DEVICE = GetStringProperty(String("ro.product.device"));
const String Build::BOARD = GetStringProperty(String("ro.product.board"));
const String Build::CPU_ABI;
const String Build::CPU_ABI2;
const String Build::MANUFACTURER = GetStringProperty(String("ro.product.manufacturer"));
const String Build::BRAND = GetStringProperty(String("ro.product.brand"));
const String Build::MODEL = GetStringProperty(String("ro.product.model"));
const String Build::BOOTLOADER = GetStringProperty(String("ro.bootloader"));
const String Build::RADIO = GetStringProperty(String("gsm.version.baseband")); ///TelephonyProperties.PROPERTY_BASEBAND_VERSION
const String Build::HARDWARE = GetStringProperty(String("ro.hardware"));
const String Build::SERIAL = GetStringProperty(String("ro.serialno"));
const String Build::TYPE = GetStringProperty(String("user")); //GetStringProperty(String("ro.build.type");
const String Build::TAGS = GetStringProperty(String("ro.build.tags"));
const String Build::FINGERPRINT = DeriveFingerprint();

const Int64 Build::TIME = GetInt64Property(String("ro.build.date.utc")) * 1000;
const String Build::USER = GetStringProperty(String("ro.build.user"));
const String Build::HOST = GetStringProperty(String("ro.build.host"));

AutoPtr<ArrayOf<String> > Build::SUPPORTED_ABIS
    = GetStringPropertyList(String("ro.product.cpu.abilist"), String(","));

AutoPtr<ArrayOf<String> > Build::SUPPORTED_32_BIT_ABIS
    = GetStringPropertyList(String("ro.product.cpu.abilist32"), String(","));

AutoPtr<ArrayOf<String> > Build::SUPPORTED_64_BIT_ABIS
    = GetStringPropertyList(String("ro.product.cpu.abilist64"), String(","));

const Boolean Build::IS_DEBUGGABLE = GetInt32Property(String("ro.debuggable")) == 1;

ECode Build::EnsureFingerprintProperty()
{
    String value = GetStringProperty(String("ro.build.fingerprint"));
    if (value.IsNullOrEmpty()) {
        // try {
        AutoPtr<ISystemProperties> sysProp;
        CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
        sysProp->Set(String("ro.build.fingerprint"), FINGERPRINT);
        // } catch (IllegalArgumentException e) {
        //     Slog.e(TAG, "Failed to set fingerprint property", e);
        // }
    }
    return NOERROR;
}

String Build::GetRadioVersion()
{
    String value = GetStringProperty(
        String("gsm.version.baseband")/*TelephonyProperties.PROPERTY_BASEBAND_VERSION*/,
        String(NULL));
    return value;
}

String Build::GetString(
    /* [in] */ const char* property)
{
    String str(property);
    return GetStringProperty(str, UNKNOWN);
}

Int64 Build::GetInt64(
    /* [in] */ const char* property)
{
    String str(property);
    return GetInt64Property(str);
}

String Build::GetString(
    /* [in] */ const String& property)
{
    return GetStringProperty(property, UNKNOWN);
}

Int64 Build::GetInt64(
    /* [in] */ const String& property)
{
    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    String retStr = GetStringProperty(property);
    sysProp->Get(property, UNKNOWN, &retStr);
    if (retStr.Equals(UNKNOWN))
        return -1;

    return StringUtils::ParseInt64(retStr);
}


} // namespace Os
} // namespace Droid
} // namespace Elastos
