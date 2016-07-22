
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/UICCConfig.h"

#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                              UICCConfig
//=====================================================================
CAR_INTERFACE_IMPL(UICCConfig, Object, IUICCConfig);

ECode UICCConfig::GetImsi(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    if (mImsi.IsNull()) {
        Logd(String("Getting IMSI: null"));
    }
    else {
        Logd(String("Getting IMSI: ") + mImsi);
    }
    *result = mImsi;
    return NOERROR;
}

ECode UICCConfig::SetImsi(
    /* [in] */ const String& lImsi)
{
    Logd(String("Setting IMSI: ") + lImsi);
    mImsi = lImsi;
    return NOERROR;
}

ECode UICCConfig::GetMncLength(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Logd(String("Getting MncLength: ") + StringUtils::ToString(mMncLength));
    *result = mMncLength;
    return NOERROR;
}

ECode UICCConfig::SetMncLength(
    /* [in] */ Int32 lMncLength)
{
    Logd(String("Setting MncLength: ") + StringUtils::ToString(lMncLength));
    mMncLength = lMncLength;
    return NOERROR;
}

void UICCConfig::Logd(
    /* [in] */ const String& sLog)
{
    if (LOG_DEBUG) {
        Logger::D(TAG, sLog);
    }
}

void UICCConfig::Loge(
    /* [in] */ const String& sLog)
{
    Logger::E(TAG, sLog);
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
