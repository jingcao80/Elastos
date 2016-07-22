
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_MULTIUSERAVATARCACHE_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_MULTIUSERAVATARCACHE_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Graphics.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Core::Object;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

class MultiUserAvatarCache
    : public Object
{
public:
    TO_STRING_IMPL("MultiUserAvatarCache")

    static CARAPI_(AutoPtr<MultiUserAvatarCache>) GetInstance();

    CARAPI Clear(
        /* [in] */ Int32 userId);

    CARAPI Get(
        /* [in] */ Int32 userId,
        /* [out] */ IDrawable** drawable);

    CARAPI Put(
        /* [in] */ Int32 userId,
        /* [in] */ IDrawable* image);

private:
    MultiUserAvatarCache();

private:
    static AutoPtr<MultiUserAvatarCache> sInstance;

    AutoPtr<IHashMap> mCache;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_KEYGUARD_MULTIUSERAVATARCACHE_H__