
#include "elastos/droid/incallui/widget/multiwaveview/Tweener.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::CHashMap;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::ISet;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace InCallUI {
namespace Widget {
namespace MultiwaveView {

//=====================================================================
//               Tweener::TweenerAnimatorListenerAdapter
//=====================================================================
ECode Tweener::TweenerAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    Tweener::Remove(animation);
    return NOERROR;
}

ECode Tweener::TweenerAnimatorListenerAdapter::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    Tweener::Remove(animation);
    return NOERROR;
}


//=====================================================================
//               Tweener
//=====================================================================
const String Tweener::TAG(String("Tweener"));
const Boolean Tweener::DEBUG = FALSE;
AutoPtr<IHashMap> Tweener::sTweens =  Init_sTweens();
AutoPtr<IAnimatorListener> Tweener::sCleanupListener = new Tweener::TweenerAnimatorListenerAdapter();

AutoPtr<IHashMap> Tweener::Init_sTweens()
{
    AutoPtr<IHashMap> map;
    CHashMap::New((IHashMap**)&map);
    return map;
}

Tweener::Tweener(
    /* [in] */ IObjectAnimator* anim)
    : mAnimator(anim)
{}

void Tweener::Remove(
    /* [in] */ IAnimator* animator)
{
    AutoPtr<ISet> st;
    sTweens->GetEntrySet((ISet**)&st);
    AutoPtr<IIterator> iter;
    st->GetIterator((IIterator**)&iter);
    Boolean hasNext = FALSE;
    while ((iter->HasNext(&hasNext), hasNext)) {
        AutoPtr<IInterface> node;
        iter->GetNext((IInterface**)&node);
        AutoPtr<IMapEntry> entry = IMapEntry::Probe(node);
        AutoPtr<IInterface> val;
        entry->GetValue((IInterface**)&val);
        Tweener* cval = (Tweener*)IObject::Probe(val);
        if (IAnimator::Probe(cval->mAnimator) == animator) {
            if (DEBUG) {
                AutoPtr<IInterface> k;
                entry->GetKey((IInterface**)&k);
                AutoPtr<IInterface> p;
                sTweens->Get(k, (IInterface**)&p);
                Int32 s = 0;
                sTweens->GetSize(&s);
                Logger::V(TAG, "Removing tweener %s sTweens.size() = %d", TO_CSTR(p), s);
            }
            iter->Remove();
            break; // an animator can only be attached to one object
        }
    }
}

ECode Tweener::To(
    /* [in] */ IInterface* object,
    /* [in] */ Int64 duration,
    /* [in] */ ArrayOf<IInterface*>* vars,
    /* [out] */ Tweener** result)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode Tweener::From(
    /* [in] */ IInterface* object,
    /* [in] */ Int64 duration,
    /* [in] */ ArrayOf<IInterface*>* vars,
    /* [out] */ Tweener** result)
{
    assert(0 && "TODO");
    return NOERROR;
}

void Tweener::Reset()
{
    if (DEBUG) {
        Logger::V(TAG, "Reset()");
        Int32 s = 0;
        sTweens->GetSize(&s);
        if (s > 0) {
            Logger::V(TAG, "Cleaning up %d animations", s);
        }
    }
    sTweens->Clear();
}

void Tweener::Replace(
    /* [in] */ IArrayList* props,
    /* [in] */ ArrayOf<IInterface*>* args)
{
    assert(0 && "TODO");
}

} // namespace MultiwaveView
} // namespace Widget
} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

