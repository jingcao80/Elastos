#ifndef __ELASTOS_DROID_OS_NATIVEBINDER_H__
#define __ELASTOS_DROID_OS_NATIVEBINDER_H__

#include <binder/IBinder.h>

namespace Elastos {
namespace Droid {
namespace Os {

extern AutoPtr<IBinder> DroidObjectForIBinder(const android::sp<android::IBinder>& val);

extern android::sp<android::IBinder> IBinderForDroidObject(IBinder* obj);

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_NATIVEBINDER_H__
