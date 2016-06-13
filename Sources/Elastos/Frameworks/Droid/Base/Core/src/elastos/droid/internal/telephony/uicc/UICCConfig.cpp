
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/UICCConfig.h"

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
    // ==================before translated======================
    // if (mImsi == null) {
    //     logd("Getting IMSI: null");
    // } else {
    //     logd("Getting IMSI: " + mImsi);
    // }
    // return mImsi;
    assert(0);
    return NOERROR;
}

ECode UICCConfig::SetImsi(
    /* [in] */ const String& lImsi)
{
    // ==================before translated======================
    // logd("Setting IMSI: " + lImsi);
    // mImsi = lImsi;
    assert(0);
    return NOERROR;
}

ECode UICCConfig::GetMncLength(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // logd("Getting MncLength: " + Integer.toString(mMncLength));
    // return mMncLength;
    assert(0);
    return NOERROR;
}

ECode UICCConfig::SetMncLength(
    /* [in] */ Int32 lMncLength)
{
    // ==================before translated======================
    // logd("Setting MncLength: " + Integer.toString(lMncLength));
    // mMncLength = lMncLength;
    assert(0);
    return NOERROR;
}

void UICCConfig::Logd(
    /* [in] */ const String& sLog)
{
    // ==================before translated======================
    // if (LOG_DEBUG) {
    //     Rlog.d(TAG, sLog);
    // }
    assert(0);
}

void UICCConfig::Loge(
    /* [in] */ const String& sLog)
{
    // ==================before translated======================
    // Rlog.e(TAG, sLog);
    assert(0);
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
