
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/CRegion.h"
#include "elastos/droid/view/ViewTreeObserver.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CRegion;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace View {

//=====================================================================
//               ViewTreeObserver::InternalInsetsInfo
//=====================================================================
CAR_INTERFACE_IMPL(ViewTreeObserver::InternalInsetsInfo, Object, IInternalInsetsInfo)

ViewTreeObserver::InternalInsetsInfo::InternalInsetsInfo()
{
    ASSERT_SUCCEEDED(CRect::New((IRect**)&mContentInsets));
    ASSERT_SUCCEEDED(CRect::New((IRect**)&mVisibleInsets));
    ASSERT_SUCCEEDED(CRegion::New((IRegion**)&mTouchableRegion));
}

ECode ViewTreeObserver::InternalInsetsInfo::SetTouchableInsets(
    /* [in] */ Int32 val)
{
    mTouchableInsets = val;
    return NOERROR;
}

ECode ViewTreeObserver::InternalInsetsInfo::GetTouchableInsets(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mTouchableInsets;
    return NOERROR;
}

ECode ViewTreeObserver::InternalInsetsInfo::GetContentInsets(
    /* [out] */ IRect** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mContentInsets;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ViewTreeObserver::InternalInsetsInfo::GetVisibleInsets(
    /* [out] */ IRect** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mVisibleInsets;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ViewTreeObserver::InternalInsetsInfo::GetTouchableRegion(
    /* [out] */ IRegion** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mTouchableRegion;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ViewTreeObserver::InternalInsetsInfo::Reset()
{
    mContentInsets->SetEmpty();
    mVisibleInsets->SetEmpty();
    mTouchableRegion->SetEmpty();
    mTouchableInsets = TOUCHABLE_INSETS_FRAME;
    return NOERROR;
}

ECode ViewTreeObserver::InternalInsetsInfo::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Boolean isEmpty = FALSE;
    mContentInsets->IsEmpty(&isEmpty);
    Boolean isEmpty1 = FALSE;
    mVisibleInsets->IsEmpty(&isEmpty1);
    Boolean isEmpty2 = FALSE;
    mTouchableRegion->IsEmpty(&isEmpty2);
    *result = isEmpty && isEmpty1 && isEmpty2 && (mTouchableInsets == TOUCHABLE_INSETS_FRAME);
    return NOERROR;
}

ECode ViewTreeObserver::InternalInsetsInfo::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    VALIDATE_NOT_NULL(other);

    IInterface* thisInterface = TO_IINTERFACE(this);
    if (other == thisInterface) {
        *result = TRUE;
        return NOERROR;
    }

    if (NULL == other) {
        *result = FALSE;
        return NOERROR;
    }

    IObject* otherObjTmp = IObject::Probe(other);
    InternalInsetsInfo* otherObj = (InternalInsetsInfo*)otherObjTmp;
    Boolean isEqual = FALSE;
    mContentInsets->Equals(otherObj->mContentInsets, &isEqual);
    Boolean isEqual1 = FALSE;
    mVisibleInsets->Equals(otherObj->mVisibleInsets, &isEqual1);
    Boolean isEqual2 = FALSE;
    mTouchableRegion->Equals(otherObj->mTouchableRegion, &isEqual2);
    *result = isEqual && isEqual1 && isEqual2 && (mTouchableInsets == otherObj->mTouchableInsets);
    return NOERROR;
}

ECode ViewTreeObserver::InternalInsetsInfo::Set(
    /* [in] */ InternalInsetsInfo* other)
{
    VALIDATE_NOT_NULL(other);
    mContentInsets->Set(other->mContentInsets);
    mVisibleInsets->Set(other->mVisibleInsets);
    Boolean res = FALSE;
    mTouchableRegion->Set(other->mTouchableRegion, &res);
    mTouchableInsets = other->mTouchableInsets;
    return NOERROR;
}

//=====================================================================
//               ViewTreeObserver::InternalInsetsInfo
//=====================================================================
CAR_INTERFACE_IMPL(ViewTreeObserver, Object, IViewTreeObserver)

ViewTreeObserver::ViewTreeObserver()
    : mAlive(TRUE)
{
}

ECode ViewTreeObserver::constructor()
{
    return NOERROR;
}

ECode ViewTreeObserver::Merge(
    /* [in] */ IViewTreeObserver* observer)
{
    VALIDATE_NOT_NULL(observer);

    ViewTreeObserver* observerObj = (ViewTreeObserver*)observer;

    if (observerObj->mOnWindowAttachListeners != NULL) {
        List<AutoPtr<IOnWindowAttachListener> >* other = observerObj->mOnWindowAttachListeners;
        if (mOnWindowAttachListeners != NULL) {
            List<AutoPtr<IOnWindowAttachListener> >::Iterator it = other->Begin();
            for (; it != other->End(); ++it) {
                mOnWindowAttachListeners->PushBack(*it);
            }
        }
        else {
            mOnWindowAttachListeners = new List<AutoPtr<IOnWindowAttachListener> >(*other);
        }
    }

    if (observerObj->mOnWindowFocusListeners != NULL) {
        List<AutoPtr<IOnWindowFocusChangeListener> >* other = observerObj->mOnWindowFocusListeners;
        if (mOnWindowFocusListeners != NULL) {
            List<AutoPtr<IOnWindowFocusChangeListener> >::Iterator it = other->Begin();
            for (; it != other->End(); ++it) {
                mOnWindowFocusListeners->PushBack(*it);
            }
        }
        else {
            mOnWindowFocusListeners = new List<AutoPtr<IOnWindowFocusChangeListener> >(*other);
        }
    }

    if (observerObj->mOnGlobalFocusListeners != NULL) {
        List<AutoPtr<IOnGlobalFocusChangeListener> >* other = observerObj->mOnGlobalFocusListeners;
        if (mOnGlobalFocusListeners != NULL) {
            List<AutoPtr<IOnGlobalFocusChangeListener> >::Iterator it = other->Begin();
            for (; it != other->End(); ++it) {
                mOnGlobalFocusListeners->PushBack(*it);
            }
        }
        else {
            mOnGlobalFocusListeners = new List<AutoPtr<IOnGlobalFocusChangeListener> >(*other);
        }
    }

    if (observerObj->mOnGlobalLayoutListeners != NULL) {
        List<AutoPtr<IOnGlobalLayoutListener> >* other = observerObj->mOnGlobalLayoutListeners;
        if (mOnGlobalLayoutListeners != NULL) {
            List<AutoPtr<IOnGlobalLayoutListener> >::Iterator it = other->Begin();
            for (; it != other->End(); ++it) {
                mOnGlobalLayoutListeners->PushBack(*it);
            }
        }
        else {
            mOnGlobalLayoutListeners = new List<AutoPtr<IOnGlobalLayoutListener> >(*other);
        }
    }

    if (observerObj->mOnPreDrawListeners != NULL) {
        List<AutoPtr<IOnPreDrawListener> >* other = observerObj->mOnPreDrawListeners;
        if (mOnPreDrawListeners != NULL) {
            List<AutoPtr<IOnPreDrawListener> >::Iterator it = other->Begin();
            for (; it != other->End(); ++it) {
                mOnPreDrawListeners->PushBack(*it);
            }
        }
        else {
            mOnPreDrawListeners = new List<AutoPtr<IOnPreDrawListener> >(*other);
        }
    }

    if (observerObj->mOnTouchModeChangeListeners != NULL) {
        List<AutoPtr<IOnTouchModeChangeListener> >* other = observerObj->mOnTouchModeChangeListeners;
        if (mOnTouchModeChangeListeners != NULL) {
            List<AutoPtr<IOnTouchModeChangeListener> >::Iterator it = other->Begin();
            for (; it != other->End(); ++it) {
                mOnTouchModeChangeListeners->PushBack(*it);
            }
        }
        else {
            mOnTouchModeChangeListeners = new List<AutoPtr<IOnTouchModeChangeListener> >(*other);
        }
    }

    if (observerObj->mOnComputeInternalInsetsListeners != NULL) {
        List<AutoPtr<IOnComputeInternalInsetsListener> >* other = observerObj->mOnComputeInternalInsetsListeners;
        if (mOnComputeInternalInsetsListeners != NULL) {
            List<AutoPtr<IOnComputeInternalInsetsListener> >::Iterator it = other->Begin();
            for (; it != other->End(); ++it) {
                mOnComputeInternalInsetsListeners->PushBack(*it);
            }
        }
        else {
            mOnComputeInternalInsetsListeners = new List<AutoPtr<IOnComputeInternalInsetsListener> >(*other);
        }
    }

    if (observerObj->mOnScrollChangedListeners != NULL) {
        List<AutoPtr<IOnScrollChangedListener> >* other = observerObj->mOnScrollChangedListeners;
        if (mOnScrollChangedListeners != NULL) {
            List<AutoPtr<IOnScrollChangedListener> >::Iterator it = other->Begin();
            for (; it != other->End(); ++it) {
                mOnScrollChangedListeners->PushBack(*it);
            }
        }
        else {
            mOnScrollChangedListeners = new List<AutoPtr<IOnScrollChangedListener> >(*other);
        }
    }

    observerObj->Kill();
    return NOERROR;
}

ECode ViewTreeObserver::AddOnWindowAttachListener(
    /* [in] */ IOnWindowAttachListener* listener)
{
    VALIDATE_NOT_NULL(listener);

    CheckIsAlive();
    if (NULL == mOnWindowAttachListeners) {
        mOnWindowAttachListeners = new List<AutoPtr<IOnWindowAttachListener> >();
    }

    mOnWindowAttachListeners->PushBack(listener);
    return NOERROR;
}

ECode ViewTreeObserver::RemoveOnWindowAttachListener(
    /* [in] */ IOnWindowAttachListener* victim)
{
    VALIDATE_NOT_NULL(victim);

    CheckIsAlive();
    if (NULL != mOnWindowAttachListeners) {
        mOnWindowAttachListeners->Remove(victim);
    }

    return NOERROR;
}

ECode ViewTreeObserver::AddOnWindowFocusChangeListener(
    /* [in] */ IOnWindowFocusChangeListener* listener)
{
    VALIDATE_NOT_NULL(listener);

    CheckIsAlive();
    if (NULL == mOnWindowFocusListeners) {
        mOnWindowFocusListeners = new List<AutoPtr<IOnWindowFocusChangeListener> >();
    }

    mOnWindowFocusListeners->PushBack(listener);
    return NOERROR;
}

ECode ViewTreeObserver::RemoveOnWindowFocusChangeListener(
    /* [in] */ IOnWindowFocusChangeListener* victim)
{
    VALIDATE_NOT_NULL(victim);

    CheckIsAlive();
    if (NULL != mOnWindowFocusListeners) {
        mOnWindowFocusListeners->Remove(victim);
    }

    return NOERROR;
}

ECode ViewTreeObserver::AddOnGlobalFocusChangeListener(
    /* [in] */ IOnGlobalFocusChangeListener* listener)
{
    VALIDATE_NOT_NULL(listener);

    CheckIsAlive();
    if (NULL == mOnGlobalFocusListeners) {
        mOnGlobalFocusListeners = new List<AutoPtr<IOnGlobalFocusChangeListener> >();
    }

    mOnGlobalFocusListeners->PushBack(listener);
    return NOERROR;
}

ECode ViewTreeObserver::RemoveOnGlobalFocusChangeListener(
    /* [in] */ IOnGlobalFocusChangeListener* victim)
{
    VALIDATE_NOT_NULL(victim);

    CheckIsAlive();
    if (NULL != mOnGlobalFocusListeners) {
        mOnGlobalFocusListeners->Remove(victim);
    }

    return NOERROR;
}

ECode ViewTreeObserver::AddOnGlobalLayoutListener(
    /* [in] */ IOnGlobalLayoutListener* listener)
{
    VALIDATE_NOT_NULL(listener);

    CheckIsAlive();
    if (NULL == mOnGlobalLayoutListeners) {
        mOnGlobalLayoutListeners = new List<AutoPtr<IOnGlobalLayoutListener> >();
    }

    mOnGlobalLayoutListeners->PushBack(listener);
    return NOERROR;
}

ECode ViewTreeObserver::RemoveGlobalOnLayoutListener(
    /* [in] */ IOnGlobalLayoutListener* victim)
{
    return RemoveOnGlobalLayoutListener(victim);
}

ECode ViewTreeObserver::RemoveOnGlobalLayoutListener(
    /* [in] */ IOnGlobalLayoutListener* victim)
{
    VALIDATE_NOT_NULL(victim);

    CheckIsAlive();
    if (NULL != mOnGlobalLayoutListeners) {
        mOnGlobalLayoutListeners->Remove(victim);
    }

    return NOERROR;
}

ECode ViewTreeObserver::AddOnPreDrawListener(
    /* [in] */ IOnPreDrawListener* listener)
{
    VALIDATE_NOT_NULL(listener);

    CheckIsAlive();
    if (NULL == mOnPreDrawListeners) {
        mOnPreDrawListeners = new List<AutoPtr<IOnPreDrawListener> >();
    }

    mOnPreDrawListeners->PushBack(listener);
    return NOERROR;
}

ECode ViewTreeObserver::RemoveOnPreDrawListener(
    /* [in] */ IOnPreDrawListener* victim)
{
    VALIDATE_NOT_NULL(victim);

    CheckIsAlive();
    if (NULL != mOnPreDrawListeners) {
        mOnPreDrawListeners->Remove(victim);
    }

    return NOERROR;
}

ECode ViewTreeObserver::AddOnDrawListener(
    /* [in] */ IOnDrawListener* listener)
{
    VALIDATE_NOT_NULL(listener);

    CheckIsAlive();
    if (NULL == mOnDrawListeners) {
        mOnDrawListeners = new List<AutoPtr<IOnDrawListener> >();
    }

    mOnDrawListeners->PushBack(listener);
    return NOERROR;
}

ECode ViewTreeObserver::RemoveOnDrawListener(
    /* [in] */ IOnDrawListener* victim)
{
    VALIDATE_NOT_NULL(victim);

    CheckIsAlive();
    if (NULL != mOnDrawListeners) {
        mOnDrawListeners->Remove(victim);
    }

    return NOERROR;
}

ECode ViewTreeObserver::AddOnScrollChangedListener(
    /* [in] */ IOnScrollChangedListener* listener)
{
    VALIDATE_NOT_NULL(listener);

    CheckIsAlive();
    if (NULL == mOnScrollChangedListeners) {
        mOnScrollChangedListeners = new List<AutoPtr<IOnScrollChangedListener> >();
    }

    mOnScrollChangedListeners->PushBack(listener);
    return NOERROR;
}

ECode ViewTreeObserver::RemoveOnScrollChangedListener(
    /* [in] */ IOnScrollChangedListener* victim)
{
    VALIDATE_NOT_NULL(victim);

    CheckIsAlive();
    if (NULL != mOnScrollChangedListeners) {
        mOnScrollChangedListeners->Remove(victim);
    }

    return NOERROR;
}

ECode ViewTreeObserver::AddOnTouchModeChangeListener(
    /* [in] */ IOnTouchModeChangeListener* listener)
{
    VALIDATE_NOT_NULL(listener);

    CheckIsAlive();
    if (NULL == mOnTouchModeChangeListeners) {
        mOnTouchModeChangeListeners = new List<AutoPtr<IOnTouchModeChangeListener> >();
    }

    mOnTouchModeChangeListeners->PushBack(listener);
    return NOERROR;
}

ECode ViewTreeObserver::RemoveOnTouchModeChangeListener(
    /* [in] */ IOnTouchModeChangeListener* victim)
{
    VALIDATE_NOT_NULL(victim);

    CheckIsAlive();
    if (NULL != mOnTouchModeChangeListeners) {
        mOnTouchModeChangeListeners->Remove(victim);
    }

    return NOERROR;
}

ECode ViewTreeObserver::AddOnComputeInternalInsetsListener(
    /* [in] */ IOnComputeInternalInsetsListener* listener)
{
    VALIDATE_NOT_NULL(listener);

    CheckIsAlive();
    if (NULL == mOnComputeInternalInsetsListeners) {
        mOnComputeInternalInsetsListeners = new List<AutoPtr<IOnComputeInternalInsetsListener> >();
    }

    mOnComputeInternalInsetsListeners->PushBack(listener);
    return NOERROR;
}

ECode ViewTreeObserver::RemoveOnComputeInternalInsetsListener(
    /* [in] */ IOnComputeInternalInsetsListener* victim)
{
    VALIDATE_NOT_NULL(victim);

    CheckIsAlive();
    if (NULL != mOnComputeInternalInsetsListeners) {
        mOnComputeInternalInsetsListeners->Remove(victim);
    }

    return NOERROR;
}

ECode ViewTreeObserver::CheckIsAlive()
{

    if (!mAlive) {
        // throw new IllegalStateException("This ViewTreeObserver is not alive, call "
        //         + "getViewTreeObserver() again");
        Logger::D("ViewTreeObserver", "This ViewTreeObserver is not alive, call getViewTreeObserver() again");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode ViewTreeObserver::IsAlive(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mAlive;
    return NOERROR;
}

ECode ViewTreeObserver::DispatchOnWindowAttachedChange(
    /* [in] */ Boolean attached)
{
    // NOTE: because of the use of CopyOnWriteArrayList, we *must* use an iterator to
    // perform the dispatching. The iterator is a safe guard against listeners that
    // could mutate the list by calling the various add/remove methods. This prevents
    // the array from being modified while we iterate it.
    if (mOnWindowAttachListeners != NULL && !mOnWindowAttachListeners->IsEmpty()) {
        List<AutoPtr<IOnWindowAttachListener> > access(*mOnWindowAttachListeners);
        List<AutoPtr<IOnWindowAttachListener> >::Iterator iter = access.Begin();
        for (; iter != access.End(); ++iter) {
            if (attached){
                (*iter)->OnWindowAttached();
            }
            else {
                (*iter)->OnWindowDetached();
            }
        }
    }

    return NOERROR;
}

ECode ViewTreeObserver::DispatchOnWindowFocusChange(
    /* [in] */ Boolean hasFocus)
{
    if (mOnWindowFocusListeners != NULL && !mOnWindowFocusListeners->IsEmpty()) {
        List<AutoPtr<IOnWindowFocusChangeListener> > access(*mOnWindowFocusListeners);
        List<AutoPtr<IOnWindowFocusChangeListener> >::Iterator iter = access.Begin();
        for (; iter != access.End(); ++iter) {
            (*iter)->OnWindowFocusChanged(hasFocus);
        }
    }

    return NOERROR;
}

ECode ViewTreeObserver::DispatchOnGlobalFocusChange(
    /* [in] */ IView* oldFocus,
    /* [in] */ IView* newFocus)
{
    VALIDATE_NOT_NULL(oldFocus);
    VALIDATE_NOT_NULL(newFocus);

    if (mOnGlobalFocusListeners != NULL && !mOnGlobalFocusListeners->IsEmpty()) {
        List<AutoPtr<IOnGlobalFocusChangeListener> > access(*mOnGlobalFocusListeners);
        List<AutoPtr<IOnGlobalFocusChangeListener> >::Iterator iter = access.Begin();
        for (; iter != access.End(); ++iter) {
            (*iter)->OnGlobalFocusChanged(oldFocus, newFocus);
        }
    }

    return NOERROR;
}

ECode ViewTreeObserver::DispatchOnGlobalLayout()
{
    if (mOnGlobalLayoutListeners != NULL && !mOnGlobalLayoutListeners->IsEmpty()) {
        List<AutoPtr<IOnGlobalLayoutListener> > access(*mOnGlobalLayoutListeners);
        List<AutoPtr<IOnGlobalLayoutListener> >::Iterator iter = access.Begin();
        for (; iter != access.End(); ++iter) {
            (*iter)->OnGlobalLayout();
        }
    }
    return NOERROR;
}

ECode ViewTreeObserver::HasOnPreDrawListeners(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = (mOnPreDrawListeners != NULL && mOnPreDrawListeners->GetSize() > 0);
    return NOERROR;
}

ECode ViewTreeObserver::DispatchOnPreDraw(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (mOnPreDrawListeners != NULL && !mOnPreDrawListeners->IsEmpty()) {
        List<AutoPtr<IOnPreDrawListener> > access(*mOnPreDrawListeners);
        Boolean isPreDraw;
        List<AutoPtr<IOnPreDrawListener> >::Iterator iter = access.Begin();
        for (; iter != access.End(); ++iter) {
            isPreDraw = FALSE;
            assert((*iter) != NULL);
            (*iter)->OnPreDraw(&isPreDraw);
            *result |= !isPreDraw;
        }
    }
    return NOERROR;
}

ECode ViewTreeObserver::DispatchOnDraw()
{
    if (mOnDrawListeners != NULL) {
        List<AutoPtr<IOnDrawListener> >::Iterator iter = mOnDrawListeners->Begin();
        for (; iter != mOnDrawListeners->End(); ++iter) {
            assert((*iter) != NULL);
            (*iter)->OnDraw();
        }
    }
    return NOERROR;
}

ECode ViewTreeObserver::DispatchOnTouchModeChanged(
    /* [in] */ Boolean inTouchMode)
{
    if (mOnTouchModeChangeListeners != NULL && !mOnTouchModeChangeListeners->IsEmpty()) {
        List<AutoPtr<IOnTouchModeChangeListener> > access(*mOnTouchModeChangeListeners);
        List<AutoPtr<IOnTouchModeChangeListener> >::Iterator iter = access.Begin();
        for (; iter != access.End(); ++iter) {
            assert((*iter) != NULL);
            (*iter)->OnTouchModeChanged(inTouchMode);
        }
    }

    return NOERROR;
}

ECode ViewTreeObserver::DispatchOnScrollChanged()
{
    if (mOnScrollChangedListeners != NULL && !mOnScrollChangedListeners->IsEmpty()) {
        List<AutoPtr<IOnScrollChangedListener> > access(*mOnScrollChangedListeners);
        List<AutoPtr<IOnScrollChangedListener> >::Iterator iter = access.Begin();
        for (; iter != access.End(); ++iter) {
            assert((*iter) != NULL);
            (*iter)->OnScrollChanged();
        }
    }

    return NOERROR;

}

ECode ViewTreeObserver::HasComputeInternalInsetsListeners(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (mOnComputeInternalInsetsListeners != NULL && !mOnComputeInternalInsetsListeners->IsEmpty()) {
        List<AutoPtr<IOnComputeInternalInsetsListener> > access(*mOnComputeInternalInsetsListeners);
        *result = !access.IsEmpty();
    }
    return NOERROR;
}

ECode ViewTreeObserver::DispatchOnComputeInternalInsets(
    /* [in] */ IInternalInsetsInfo* inoutInfo)
{
    VALIDATE_NOT_NULL(inoutInfo);

    if (mOnComputeInternalInsetsListeners != NULL && !mOnComputeInternalInsetsListeners->IsEmpty()) {
        List<AutoPtr<IOnComputeInternalInsetsListener> > access(*mOnComputeInternalInsetsListeners);
        List<AutoPtr<IOnComputeInternalInsetsListener> >::Iterator iter = access.Begin();
        for (; iter != access.End(); ++iter) {
            assert((*iter) != NULL);
            (*iter)->OnComputeInternalInsets(inoutInfo);
        }
    }

    return NOERROR;
}

ECode ViewTreeObserver::Kill()
{
    mAlive = FALSE;
    return NOERROR;
}

} // namespace View
} // namespace Droid
} // namespace Elastos

