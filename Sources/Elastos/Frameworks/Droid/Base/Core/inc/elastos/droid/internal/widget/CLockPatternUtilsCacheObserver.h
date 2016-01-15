
#ifndef  __ELASTOS_DROID_WIDGET_INTERNAL_CLOCKPATTERNUTILSCACHEOBSERVER_H__
#define  __ELASTOS_DROID_WIDGET_INTERNAL_CLOCKPATTERNUTILSCACHEOBSERVER_H__

#include "_Elastos_Droid_Internal_Widget_CLockPatternUtilsCacheObserver.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

CarClass(CLockPatternUtilsCacheObserver)
    , public Object
    , public IILockSettingsObserver
{
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ILockPatternUtilsCache* service);

    CARAPI OnLockSettingChanged(
        /* [in] */ const String& key,
        /* [in] */ Int32 userId);

private:
    AutoPtr<ILockPatternUtilsCache> mService;
};

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_INTERNAL_CLOCKPATTERNUTILSCACHEOBSERVER_H__
