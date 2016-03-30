#include "elastos/droid/internal/utility/cm/QSUtils.h"
#include "elastos/droid/internal/utility/cm/CQSUtils.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {
namespace Cm {

CAR_INTERFACE_IMPL(CQSUtils, Singleton, IQSUtils)

CAR_SINGLETON_IMPL(CQSUtils)

ECode CQSUtils::GetAvailableTiles(
    /* [in] */ IContext* context,
    /* [out] */ IList** result)
{
	return QSUtils::GetAvailableTiles(context, result);
}

ECode CQSUtils::GetDefaultTiles(
    /* [in] */ IContext* context,
    /* [out] */ IList** result)
{
	return QSUtils::GetDefaultTiles(context, result);
}

ECode CQSUtils::GetDefaultTilesAsString(
    /* [in] */ IContext* context,
    /* [out] */ String* result)
{
	return QSUtils::GetDefaultTilesAsString(context, result);
}

ECode CQSUtils::DeviceSupportsLte(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
	return QSUtils::DeviceSupportsLte(context, result);
}

ECode CQSUtils::DeviceSupportsDdsSupported(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
	return QSUtils::DeviceSupportsDdsSupported(context, result);
}

ECode CQSUtils::DeviceSupportsMobileData(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
	return QSUtils::DeviceSupportsMobileData(context, result);
}

ECode CQSUtils::DeviceSupportsBluetooth(
    /* [out] */ Boolean* result)
{
	return QSUtils::DeviceSupportsBluetooth(result);
}

ECode CQSUtils::DeviceSupportsNfc(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
	return QSUtils::DeviceSupportsNfc(context, result);
}

ECode CQSUtils::DeviceSupportsFlashLight(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
	return QSUtils::DeviceSupportsFlashLight(context, result);
}

ECode CQSUtils::DeviceSupportsCompass(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
	return QSUtils::DeviceSupportsCompass(context, result);
}

} //namespace Cm
} //namespace Utility
} //namespace Internal
} //namespace Droid
} //namespace Elastos
