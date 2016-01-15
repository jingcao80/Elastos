
#include "elastos/droid/internal/widget/CLockPatternUtilsCacheObserver.h"
#include "elastos/droid/internal/widget/CLockPatternUtilsCache.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

CAR_OBJECT_IMPL(CLockPatternUtilsCacheObserver)
CAR_INTERFACE_IMPL(CLockPatternUtilsCacheObserver, Object, IILockSettingsObserver);

ECode CLockPatternUtilsCacheObserver::constructor()
{
    return NOERROR;
}

ECode CLockPatternUtilsCacheObserver::constructor(
    /* [in] */ ILockPatternUtilsCache* service)
{
    mService = service;
    return NOERROR;
}

ECode CLockPatternUtilsCacheObserver::OnLockSettingChanged(
    /* [in] */ const String& key,
    /* [in] */ Int32 userId)
{
    ((CLockPatternUtilsCache*)mService.Get())->InvalidateCache(key, userId);
    return NOERROR;
}

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos
