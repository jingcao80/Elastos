
#ifndef __ELASTOS_DROID_INPUTMETHOD_PINYIN_SKBPOOL_H__
#define __ELASTOS_DROID_INPUTMETHOD_PINYIN_SKBPOOL_H__

#include "_Elastos.Droid.InputMethod.Pinyin.h"
#include "elastos/droid/inputmethod/pinyin/SkbTemplate.h"
#include "elastos/droid/inputmethod/pinyin/SoftKeyboard.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/Vector.h>

using Elastos::Droid::Content::IContext;
using Elastos::Core::Object;
using Elastos::Utility::Etl::Vector;

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

/**
 * Class used to cache previously loaded soft keyboard layouts.
 */
class SkbPool: public Object
{
public:
    static CARAPI_(AutoPtr<SkbPool>) GetInstance();

    CARAPI_(void) ResetCachedSkb();

    CARAPI_(AutoPtr<SkbTemplate>) GetSkbTemplate(
        /* [in] */ Int32 skbTemplateId,
        /* [in] */ IContext* context);

    // Try to find the keyboard in the pool with the cache id. If there is no
    // keyboard found, try to load it with the given xml id.
    CARAPI_(AutoPtr<SoftKeyboard>) GetSoftKeyboard(
        /* [in] */ Int32 skbCacheId,
        /* [in] */ Int32 skbXmlId,
        /* [in] */ Int32 skbWidth,
        /* [in] */ Int32 skbHeight,
        /* [in] */ IContext* context);

private:
    SkbPool();

private:
    static AutoPtr<SkbPool> sInstance;

    Vector< AutoPtr<SkbTemplate> > mSkbTemplates;
    Vector< AutoPtr<SoftKeyboard> > mSoftKeyboards;
};

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_INPUTMETHOD_PINYIN_SKBPOOL_H__
