#include "elastos/droid/media/tv/CTvInputInfoHelper.h"
#include "elastos/droid/media/tv/CTvInputInfo.h"

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

CAR_INTERFACE_IMPL(CTvInputInfoHelper, Singleton, ITvInputInfoHelper)

CAR_SINGLETON_IMPL(CTvInputInfoHelper)

ECode CTvInputInfoHelper::CreateTvInputInfo(
    /* [in] */ IContext* context,
    /* [in] */ IResolveInfo* service,
    /* [out] */ ITvInputInfo** result)
{
    return CTvInputInfo::CreateTvInputInfo(context, service, result);
}

ECode CTvInputInfoHelper::CreateTvInputInfo(
    /* [in] */ IContext* context,
    /* [in] */ IResolveInfo* service,
    /* [in] */ IHdmiDeviceInfo* hdmiDeviceInfo,
    /* [in] */ const String& parentId,
    /* [in] */ const String& label,
    /* [in] */ IUri* iconUri,
    /* [out] */ ITvInputInfo** result)
{
    return CTvInputInfo::CreateTvInputInfo(context, service,
            hdmiDeviceInfo, parentId, label, iconUri,result);
}

ECode CTvInputInfoHelper::CreateTvInputInfo(
    /* [in] */ IContext* context,
    /* [in] */ IResolveInfo* service,
    /* [in] */ ITvInputHardwareInfo* hardwareInfo,
    /* [in] */ const String& label,
    /* [in] */ IUri* iconUri,
    /* [out] */ ITvInputInfo** result)
{
    return CTvInputInfo::CreateTvInputInfo(context, service,
            hardwareInfo, label, iconUri,result);
}

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos
