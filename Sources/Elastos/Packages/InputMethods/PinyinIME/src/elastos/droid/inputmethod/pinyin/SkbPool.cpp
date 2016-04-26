
#include "elastos/droid/inputmethod/pinyin/SkbPool.h"
#include "elastos/droid/inputmethod/pinyin/XmlKeyboardLoader.h"

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

AutoPtr<SkbPool> SkbPool::sInstance;

SkbPool::SkbPool()
{}

AutoPtr<SkbPool> SkbPool::GetInstance()
{
    if (NULL == sInstance) sInstance = new SkbPool();
    return sInstance;
}

void SkbPool::ResetCachedSkb()
{
    mSoftKeyboards.Clear();
}

AutoPtr<SkbTemplate> SkbPool::GetSkbTemplate(
    /* [in] */ Int32 skbTemplateId,
    /* [in] */ IContext* context)
{
    Vector< AutoPtr<SkbTemplate> >::Iterator ator = mSkbTemplates.Begin();
    for (; ator != mSkbTemplates.End(); ++ator) {
        AutoPtr<SkbTemplate> t = *ator;
        if (t->GetSkbTemplateId() == skbTemplateId) {
            return t;
        }
    }

    if (NULL != context) {
        AutoPtr<XmlKeyboardLoader> xkbl = new XmlKeyboardLoader(context);
        AutoPtr<SkbTemplate> t = xkbl->LoadSkbTemplate(skbTemplateId);
        if (NULL != t) {
            mSkbTemplates.PushBack(t);
            return t;
        }
    }
    return NULL;
}

AutoPtr<SoftKeyboard> SkbPool::GetSoftKeyboard(
    /* [in] */ Int32 skbCacheId,
    /* [in] */ Int32 skbXmlId,
    /* [in] */ Int32 skbWidth,
    /* [in] */ Int32 skbHeight,
    /* [in] */ IContext* context)
{
    Vector<AutoPtr<SoftKeyboard> >::Iterator ator = mSoftKeyboards.Begin();
    for (; ator != mSoftKeyboards.End(); ++ator) {
        AutoPtr<SoftKeyboard> skb = *ator;
        if (skb->GetCacheId() == skbCacheId && skb->GetSkbXmlId() == skbXmlId) {
            skb->SetSkbCoreSize(skbWidth, skbHeight);
            skb->SetNewlyLoadedFlag(FALSE);
            return skb;
        }
    }

    if (NULL != context) {
        AutoPtr<XmlKeyboardLoader> xkbl = new XmlKeyboardLoader(context);
        AutoPtr<SoftKeyboard> skb = xkbl->LoadKeyboard(skbXmlId, skbWidth, skbHeight);
        if (skb != NULL) {
            if (skb->GetCacheFlag()) {
                skb->SetCacheId(skbCacheId);
                mSoftKeyboards.PushBack(skb);
            }
        }
        return skb;
    }
    return NULL;
}

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos
