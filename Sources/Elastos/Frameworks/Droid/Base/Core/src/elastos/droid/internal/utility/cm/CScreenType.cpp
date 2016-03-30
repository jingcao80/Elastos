#include "elastos/droid/internal/utility/cm/ScreenType.h"
#include "elastos/droid/internal/utility/cm/CScreenType.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {
namespace Cm {

CAR_INTERFACE_IMPL(CScreenType, Singleton, IScreenType)

CAR_SINGLETON_IMPL(CScreenType)

ECode CScreenType::IsPhone(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
	return ScreenType::IsPhone(context, result);
}

ECode CScreenType::IsHybrid(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
	return ScreenType::IsHybrid(context, result);
}

ECode CScreenType::IsTablet(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
	return ScreenType::IsTablet(context, result);
}

} //namespace Cm
} //namespace Utility
} //namespace Internal
} //namespace Droid
} //namespace Elastos
