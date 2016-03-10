
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/res/CThemeConfig.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Core::EIID_ICloneable;
using Elastos::Core::EIID_IComparable;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

CAR_INTERFACE_IMPL_4(CThemeConfig, Object, IThemeConfig, ICloneable, IParcelable, IComparable)

CAR_OBJECT_IMPL(CThemeConfig)

ECode CThemeConfig::constructor()
{
    return NOERROR;
}

ECode CThemeConfig::Clone(
    /* [out] */ IInterface** object)
{
    return NOERROR;
}

ECode CThemeConfig::CompareTo(
    /* [in] */ IInterface* another,
    /* [out] */ Int32* result)
{
    return NOERROR;
}

ECode CThemeConfig::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return NOERROR;
}

ECode CThemeConfig::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return NOERROR;
}

AutoPtr<IThemeConfig> CThemeConfig::FromJson(
    /* [in] */ const String& json)
{
    return NULL;
}

}
}
}
}
