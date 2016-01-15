
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
    // ==================before translated======================
    // return contentInsets.isEmpty()
    //  && visibleInsets.isEmpty()
    //  && touchableRegion.isEmpty()
    //  && mTouchableInsets == TOUCHABLE_INSETS_FRAME;

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
    // ==================before translated======================
    // if (this == o) return true;
    // if (o == null || getClass() != o.getClass()) return false;
    //
    // InternalInsetsInfo other = (InternalInsetsInfo)o;
    // return mTouchableInsets == other.mTouchableInsets &&
    //     contentInsets.equals(other.contentInsets) &&
    //     visibleInsets.equals(other.visibleInsets) &&
    //     touchableRegion.equals(other.touchableRegion);

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
    // ==================before translated======================
    // if (observer.mOnWindowAttachListeners != null) {
    //     if (mOnWindowAttachListeners != null) {
    //         mOnWindowAttachListeners.addAll(observer.mOnWindowAttachListeners);
    //     } else {
    //         mOnWindowAttachListeners = observer.mOnWindowAttachListeners;
    //     }
    // }
    //
    // if (observer.mOnWindowFocusListeners != null) {
    //     if (mOnWindowFocusListeners != null) {
    //         mOnWindowFocusListeners.addAll(observer.mOnWindowFocusListeners);
    //     } else {
    //         mOnWindowFocusListeners = observer.mOnWindowFocusListeners;
    //     }
    // }
    //
    // if (observer.mOnGlobalFocusListeners != null) {
    //     if (mOnGlobalFocusListeners != null) {
    //         mOnGlobalFocusListeners.addAll(observer.mOnGlobalFocusListeners);
    //     } else {
    //         mOnGlobalFocusListeners = observer.mOnGlobalFocusListeners;
    //     }
    // }
    //
    // if (observer.mOnGlobalLayoutListeners != null) {
    //     if (mOnGlobalLayoutListeners != null) {
    //         mOnGlobalLayoutListeners.addAll(observer.mOnGlobalLayoutListeners);
    //     } else {
    //         mOnGlobalLayoutListeners = observer.mOnGlobalLayoutListeners;
    //     }
    // }
    //
    // if (observer.mOnPreDrawListeners != null) {
    //     if (mOnPreDrawListeners != null) {
    //         mOnPreDrawListeners.addAll(observer.mOnPreDrawListeners);
    //     } else {
    //         mOnPreDrawListeners = observer.mOnPreDrawListeners;
    //     }
    // }
    //
    // if (observer.mOnTouchModeChangeListeners != null) {
    //     if (mOnTouchModeChangeListeners != null) {
    //         mOnTouchModeChangeListeners.addAll(observer.mOnTouchModeChangeListeners);
    //     } else {
    //         mOnTouchModeChangeListeners = observer.mOnTouchModeChangeListeners;
    //     }
    // }
    //
    // if (observer.mOnComputeInternalInsetsListeners != null) {
    //     if (mOnComputeInternalInsetsListeners != null) {
    //         mOnComputeInternalInsetsListeners.addAll(observer.mOnComputeInternalInsetsListeners);
    //     } else {
    //         mOnComputeInternalInsetsListeners = observer.mOnComputeInternalInsetsListeners;
    //     }
    // }
    //
    // if (observer.mOnScrollChangedListeners != null) {
    //     if (mOnScrollChangedListeners != null) {
    //         mOnScrollChangedListeners.addAll(observer.mOnScrollChangedListeners);
    //     } else {
    //         mOnScrollChangedListeners = observer.mOnScrollChangedListeners;
    //     }
    // }
    //
    // observer.kill();

    ViewTreeObserver* observerObj = (ViewTreeObserver*)observer;
    assert(NULL == observerObj);
    if (observerObj->mOnWindowAttachListeners != NULL) {
        if (mOnWindowAttachListeners != NULL) {
            Int32 size = (Int32)observerObj->mOnWindowAttachListeners->GetSize();
            for (Int32 idx=0; idx<size; ++idx) {
                AutoPtr<IOnWindowAttachListener> item = (*observerObj->mOnWindowAttachListeners)[idx];
                mOnWindowAttachListeners->PushBack(item);
            }
        }
        else {
            mOnWindowAttachListeners = observerObj->mOnWindowAttachListeners;
        }
    }

    if (observerObj->mOnWindowFocusListeners != NULL) {
        if (mOnWindowFocusListeners != NULL) {
            Int32 size = (Int32)observerObj->mOnWindowFocusListeners->GetSize();
            for (Int32 idx=0; idx<size; ++idx) {
                AutoPtr<IOnWindowFocusChangeListener> item = (*observerObj->mOnWindowFocusListeners)[idx];
                mOnWindowFocusListeners->PushBack(item);
            }
        }
        else {
            mOnWindowFocusListeners = observerObj->mOnWindowFocusListeners;
        }
    }

    if (observerObj->mOnGlobalFocusListeners != NULL) {
        if (mOnGlobalFocusListeners != NULL) {
            Int32 size = (Int32)observerObj->mOnGlobalFocusListeners->GetSize();
            for (Int32 idx=0; idx<size; ++idx) {
                AutoPtr<IOnGlobalFocusChangeListener> item = (*observerObj->mOnGlobalFocusListeners)[idx];
                mOnGlobalFocusListeners->PushBack(item);
            }
        }
        else {
            mOnGlobalFocusListeners = observerObj->mOnGlobalFocusListeners;
        }
    }

    if (observerObj->mOnGlobalLayoutListeners != NULL) {
        if (mOnGlobalLayoutListeners != NULL) {
            Int32 size = (Int32)observerObj->mOnGlobalLayoutListeners->GetSize();
            for (Int32 idx=0; idx<size; ++idx) {
                AutoPtr<IOnGlobalLayoutListener> item = (*observerObj->mOnGlobalLayoutListeners)[idx];
                mOnGlobalLayoutListeners->PushBack(item);
            }
        }
        else {
            mOnGlobalLayoutListeners = observerObj->mOnGlobalLayoutListeners;
        }
    }

    if (observerObj->mOnPreDrawListeners != NULL) {
        if (mOnPreDrawListeners != NULL) {
            Int32 size = (Int32)observerObj->mOnPreDrawListeners->GetSize();
            for (Int32 idx=0; idx<size; ++idx) {
                AutoPtr<IOnPreDrawListener> item = (*observerObj->mOnPreDrawListeners)[idx];
                mOnPreDrawListeners->PushBack(item);
            }
        }
        else {
            mOnPreDrawListeners = observerObj->mOnPreDrawListeners;
        }
    }

    if (observerObj->mOnTouchModeChangeListeners != NULL) {
        if (mOnTouchModeChangeListeners != NULL) {
            Int32 size = (Int32)observerObj->mOnTouchModeChangeListeners->GetSize();
            for (Int32 idx=0; idx<size; ++idx) {
                AutoPtr<IOnTouchModeChangeListener> item = (*observerObj->mOnTouchModeChangeListeners)[idx];
                mOnTouchModeChangeListeners->PushBack(item);
            }
        }
        else {
            mOnTouchModeChangeListeners = observerObj->mOnTouchModeChangeListeners;
        }
    }

    if (observerObj->mOnComputeInternalInsetsListeners != NULL) {
        if (mOnComputeInternalInsetsListeners != NULL) {
            Int32 size = (Int32)observerObj->mOnComputeInternalInsetsListeners->GetSize();
            for (Int32 idx=0; idx<size; ++idx) {
                AutoPtr<IOnComputeInternalInsetsListener> item = (*observerObj->mOnComputeInternalInsetsListeners)[idx];
                mOnComputeInternalInsetsListeners->PushBack(item);
            }
        }
        else {
            mOnComputeInternalInsetsListeners = observerObj->mOnComputeInternalInsetsListeners;
        }
    }

    if (observerObj->mOnScrollChangedListeners != NULL) {
        if (mOnScrollChangedListeners != NULL) {
            Int32 size = (Int32)observerObj->mOnScrollChangedListeners->GetSize();
            for (Int32 idx=0; idx<size; ++idx) {
                AutoPtr<IOnScrollChangedListener> item = (*observerObj->mOnScrollChangedListeners)[idx];
                mOnScrollChangedListeners->PushBack(item);
            }
        }
        else {
            mOnScrollChangedListeners = observerObj->mOnScrollChangedListeners;
        }
    }

    observerObj->Kill();
    return NOERROR;
}

ECode ViewTreeObserver::AddOnWindowAttachListener(
    /* [in] */ IOnWindowAttachListener* listener)
{
    VALIDATE_NOT_NULL(listener);
    // ==================before translated======================
    // checkIsAlive();
    //
    // if (mOnWindowAttachListeners == null) {
    //     mOnWindowAttachListeners
    //             = new CopyOnWriteArrayList<OnWindowAttachListener>();
    // }
    //
    // mOnWindowAttachListeners.add(listener);

    CheckIsAlive();
    if (NULL == mOnWindowAttachListeners) {
        mOnWindowAttachListeners = new List<IOnWindowAttachListener*>();
    }

    mOnWindowAttachListeners->PushBack(listener);
    return NOERROR;
}

ECode ViewTreeObserver::RemoveOnWindowAttachListener(
    /* [in] */ IOnWindowAttachListener* victim)
{
    VALIDATE_NOT_NULL(victim);
    // ==================before translated======================
    // checkIsAlive();
    // if (mOnWindowAttachListeners == null) {
    //     return;
    // }
    // mOnWindowAttachListeners.remove(victim);

    CheckIsAlive();
    if (NULL == mOnWindowAttachListeners) {
        return NOERROR;
    }

    mOnWindowAttachListeners->Remove(victim);
    return NOERROR;
}

ECode ViewTreeObserver::AddOnWindowFocusChangeListener(
    /* [in] */ IOnWindowFocusChangeListener* listener)
{
    VALIDATE_NOT_NULL(listener);
    // ==================before translated======================
    // checkIsAlive();
    //
    // if (mOnWindowFocusListeners == null) {
    //     mOnWindowFocusListeners
    //             = new CopyOnWriteArrayList<OnWindowFocusChangeListener>();
    // }
    //
    // mOnWindowFocusListeners.add(listener);

    CheckIsAlive();
    if (NULL == mOnWindowFocusListeners) {
        mOnWindowFocusListeners = new List<IOnWindowFocusChangeListener*>();
    }

    mOnWindowFocusListeners->PushBack(listener);
    return NOERROR;
}

ECode ViewTreeObserver::RemoveOnWindowFocusChangeListener(
    /* [in] */ IOnWindowFocusChangeListener* victim)
{
    VALIDATE_NOT_NULL(victim);
    // ==================before translated======================
    // checkIsAlive();
    // if (mOnWindowFocusListeners == null) {
    //     return;
    // }
    // mOnWindowFocusListeners.remove(victim);

    CheckIsAlive();
    if (NULL == mOnWindowFocusListeners) {
        return NOERROR;
    }

    mOnWindowFocusListeners->Remove(victim);
    return NOERROR;
}

ECode ViewTreeObserver::AddOnGlobalFocusChangeListener(
    /* [in] */ IOnGlobalFocusChangeListener* listener)
{
    VALIDATE_NOT_NULL(listener);
    // ==================before translated======================
    // checkIsAlive();
    //
    // if (mOnGlobalFocusListeners == null) {
    //     mOnGlobalFocusListeners = new CopyOnWriteArrayList<OnGlobalFocusChangeListener>();
    // }
    //
    // mOnGlobalFocusListeners.add(listener);

    CheckIsAlive();
    if (NULL == mOnGlobalFocusListeners) {
        mOnGlobalFocusListeners = new List<IOnGlobalFocusChangeListener*>();
    }

    mOnGlobalFocusListeners->PushBack(listener);
    return NOERROR;
}

ECode ViewTreeObserver::RemoveOnGlobalFocusChangeListener(
    /* [in] */ IOnGlobalFocusChangeListener* victim)
{
    VALIDATE_NOT_NULL(victim);
    // ==================before translated======================
    // checkIsAlive();
    // if (mOnGlobalFocusListeners == null) {
    //     return;
    // }
    // mOnGlobalFocusListeners.remove(victim);

    CheckIsAlive();
    if (NULL == mOnGlobalFocusListeners) {
        return NOERROR;
    }

    mOnGlobalFocusListeners->Remove(victim);
    return NOERROR;
}

ECode ViewTreeObserver::AddOnGlobalLayoutListener(
    /* [in] */ IOnGlobalLayoutListener* listener)
{
    VALIDATE_NOT_NULL(listener);
    // ==================before translated======================
    // checkIsAlive();
    //
    // if (mOnGlobalLayoutListeners == null) {
    //     mOnGlobalLayoutListeners = new CopyOnWriteArray<OnGlobalLayoutListener>();
    // }
    //
    // mOnGlobalLayoutListeners.add(listener);

    CheckIsAlive();
    if (NULL == mOnGlobalLayoutListeners) {
        mOnGlobalLayoutListeners = new List<IOnGlobalLayoutListener*>();
    }

    mOnGlobalLayoutListeners->PushBack(listener);
    return NOERROR;
}

ECode ViewTreeObserver::RemoveGlobalOnLayoutListener(
    /* [in] */ IOnGlobalLayoutListener* victim)
{
    VALIDATE_NOT_NULL(victim);
    // ==================before translated======================
    // removeOnGlobalLayoutListener(victim);

    return RemoveOnGlobalLayoutListener(victim);
}

ECode ViewTreeObserver::RemoveOnGlobalLayoutListener(
    /* [in] */ IOnGlobalLayoutListener* victim)
{
    VALIDATE_NOT_NULL(victim);
    // ==================before translated======================
    // checkIsAlive();
    // if (mOnGlobalLayoutListeners == null) {
    //     return;
    // }
    // mOnGlobalLayoutListeners.remove(victim);

    CheckIsAlive();
    if (NULL == mOnGlobalLayoutListeners) {
        return NOERROR;
    }

    mOnGlobalLayoutListeners->Remove(victim);
    return NOERROR;
}

ECode ViewTreeObserver::AddOnPreDrawListener(
    /* [in] */ IOnPreDrawListener* listener)
{
    VALIDATE_NOT_NULL(listener);
    // ==================before translated======================
    // checkIsAlive();
    //
    // if (mOnPreDrawListeners == null) {
    //     mOnPreDrawListeners = new CopyOnWriteArray<OnPreDrawListener>();
    // }
    //
    // mOnPreDrawListeners.add(listener);

    CheckIsAlive();
    if (NULL == mOnPreDrawListeners) {
        mOnPreDrawListeners = new List<IOnPreDrawListener*>();
    }

    mOnPreDrawListeners->PushBack(listener);
    return NOERROR;
}

ECode ViewTreeObserver::RemoveOnPreDrawListener(
    /* [in] */ IOnPreDrawListener* victim)
{
    VALIDATE_NOT_NULL(victim);
    // ==================before translated======================
    // checkIsAlive();
    // if (mOnPreDrawListeners == null) {
    //     return;
    // }
    // mOnPreDrawListeners.remove(victim);

    CheckIsAlive();
    if (NULL == mOnPreDrawListeners) {
        return NOERROR;
    }

    mOnPreDrawListeners->Remove(victim);
    return NOERROR;
}

ECode ViewTreeObserver::AddOnDrawListener(
    /* [in] */ IOnDrawListener* listener)
{
    VALIDATE_NOT_NULL(listener);
    // ==================before translated======================
    // checkIsAlive();
    //
    // if (mOnDrawListeners == null) {
    //     mOnDrawListeners = new ArrayList<OnDrawListener>();
    // }
    //
    // mOnDrawListeners.add(listener);

    CheckIsAlive();
    if (NULL == mOnDrawListeners) {
        mOnDrawListeners = new List<IOnDrawListener*>();
    }

    mOnDrawListeners->PushBack(listener);
    return NOERROR;
}

ECode ViewTreeObserver::RemoveOnDrawListener(
    /* [in] */ IOnDrawListener* victim)
{
    VALIDATE_NOT_NULL(victim);
    // ==================before translated======================
    // checkIsAlive();
    // if (mOnDrawListeners == null) {
    //     return;
    // }
    // mOnDrawListeners.remove(victim);

    CheckIsAlive();
    if (NULL == mOnDrawListeners) {
        return NOERROR;
    }

    mOnDrawListeners->Remove(victim);
    return NOERROR;
}

ECode ViewTreeObserver::AddOnScrollChangedListener(
    /* [in] */ IOnScrollChangedListener* listener)
{
    VALIDATE_NOT_NULL(listener);
    // ==================before translated======================
    // checkIsAlive();
    //
    // if (mOnScrollChangedListeners == null) {
    //     mOnScrollChangedListeners = new CopyOnWriteArray<OnScrollChangedListener>();
    // }
    //
    // mOnScrollChangedListeners.add(listener);

    CheckIsAlive();
    if (NULL == mOnScrollChangedListeners) {
        mOnScrollChangedListeners = new List<IOnScrollChangedListener*>();
    }

    mOnScrollChangedListeners->PushBack(listener);
    return NOERROR;
}

ECode ViewTreeObserver::RemoveOnScrollChangedListener(
    /* [in] */ IOnScrollChangedListener* victim)
{
    VALIDATE_NOT_NULL(victim);
    // ==================before translated======================
    // checkIsAlive();
    // if (mOnScrollChangedListeners == null) {
    //     return;
    // }
    // mOnScrollChangedListeners.remove(victim);

    CheckIsAlive();
    if (NULL == mOnScrollChangedListeners) {
        return NOERROR;
    }

    mOnScrollChangedListeners->Remove(victim);
    return NOERROR;
}

ECode ViewTreeObserver::AddOnTouchModeChangeListener(
    /* [in] */ IOnTouchModeChangeListener* listener)
{
    VALIDATE_NOT_NULL(listener);
    // ==================before translated======================
    // checkIsAlive();
    //
    // if (mOnTouchModeChangeListeners == null) {
    //     mOnTouchModeChangeListeners = new CopyOnWriteArrayList<OnTouchModeChangeListener>();
    // }
    //
    // mOnTouchModeChangeListeners.add(listener);

    CheckIsAlive();
    if (NULL == mOnTouchModeChangeListeners) {
        mOnTouchModeChangeListeners = new List<IOnTouchModeChangeListener*>();
    }

    mOnTouchModeChangeListeners->PushBack(listener);
    return NOERROR;
}

ECode ViewTreeObserver::RemoveOnTouchModeChangeListener(
    /* [in] */ IOnTouchModeChangeListener* victim)
{
    VALIDATE_NOT_NULL(victim);
    // ==================before translated======================
    // checkIsAlive();
    // if (mOnTouchModeChangeListeners == null) {
    //     return;
    // }
    // mOnTouchModeChangeListeners.remove(victim);

    CheckIsAlive();
    if (NULL == mOnTouchModeChangeListeners) {
        return NOERROR;
    }

    mOnTouchModeChangeListeners->Remove(victim);
    return NOERROR;
}

ECode ViewTreeObserver::AddOnComputeInternalInsetsListener(
    /* [in] */ IOnComputeInternalInsetsListener* listener)
{
    VALIDATE_NOT_NULL(listener);
    // ==================before translated======================
    // checkIsAlive();
    //
    // if (mOnComputeInternalInsetsListeners == null) {
    //     mOnComputeInternalInsetsListeners =
    //             new CopyOnWriteArray<OnComputeInternalInsetsListener>();
    // }
    //
    // mOnComputeInternalInsetsListeners.add(listener);

    CheckIsAlive();
    if (NULL == mOnComputeInternalInsetsListeners) {
        mOnComputeInternalInsetsListeners = new List<IOnComputeInternalInsetsListener*>();
    }

    mOnComputeInternalInsetsListeners->PushBack(listener);
    return NOERROR;
}

ECode ViewTreeObserver::RemoveOnComputeInternalInsetsListener(
    /* [in] */ IOnComputeInternalInsetsListener* victim)
{
    VALIDATE_NOT_NULL(victim);
    // ==================before translated======================
    // checkIsAlive();
    // if (mOnComputeInternalInsetsListeners == null) {
    //     return;
    // }
    // mOnComputeInternalInsetsListeners.remove(victim);

    CheckIsAlive();
    if (NULL == mOnComputeInternalInsetsListeners) {
        return NOERROR;
    }

    mOnComputeInternalInsetsListeners->Remove(victim);
    return NOERROR;
}

ECode ViewTreeObserver::CheckIsAlive()
{
    // ==================before translated======================
    // if (!mAlive) {
    //     throw new IllegalStateException("This ViewTreeObserver is not alive, call "
    //             + "getViewTreeObserver() again");
    // }

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
    // ==================before translated======================
    // return mAlive;

    *result = mAlive;
    return NOERROR;
}

ECode ViewTreeObserver::DispatchOnWindowAttachedChange(
    /* [in] */ Boolean attached)
{
    // ==================before translated======================
    // // NOTE: because of the use of CopyOnWriteArrayList, we *must* use an iterator to
    // // perform the dispatching. The iterator is a safe guard against listeners that
    // // could mutate the list by calling the various add/remove methods. This prevents
    // // the array from being modified while we iterate it.
    // final CopyOnWriteArrayList<OnWindowAttachListener> listeners
    //         = mOnWindowAttachListeners;
    // if (listeners != null && listeners.size() > 0) {
    //     for (OnWindowAttachListener listener : listeners) {
    //         if (attached) listener.onWindowAttached();
    //         else listener.onWindowDetached();
    //     }
    // }

    // NOTE: because of the use of CopyOnWriteArrayList, we *must* use an iterator to
    // perform the dispatching. The iterator is a safe guard against listeners that
    // could mutate the list by calling the various add/remove methods. This prevents
    // the array from being modified while we iterate it.
    AutoPtr< List<IOnWindowAttachListener*> > listeners = mOnWindowAttachListeners;
    if (listeners != NULL && listeners->GetSize() > 0) {
        IOnWindowAttachListener* listener = NULL;
        for (Int32 idx=0; idx<(Int32)listeners->GetSize(); ++idx) {
            listener = (*listeners)[idx];
            if (attached)
                listener->OnWindowAttached();
            else
                listener->OnWindowDetached();
        }
    }
    return NOERROR;
}

ECode ViewTreeObserver::DispatchOnWindowFocusChange(
    /* [in] */ Boolean hasFocus)
{
    // ==================before translated======================
    // // NOTE: because of the use of CopyOnWriteArrayList, we *must* use an iterator to
    // // perform the dispatching. The iterator is a safe guard against listeners that
    // // could mutate the list by calling the various add/remove methods. This prevents
    // // the array from being modified while we iterate it.
    // final CopyOnWriteArrayList<OnWindowFocusChangeListener> listeners
    //         = mOnWindowFocusListeners;
    // if (listeners != null && listeners.size() > 0) {
    //     for (OnWindowFocusChangeListener listener : listeners) {
    //         listener.onWindowFocusChanged(hasFocus);
    //     }
    // }

    AutoPtr< List<IOnWindowFocusChangeListener*> > listeners = mOnWindowFocusListeners;
    if (listeners != NULL && listeners->GetSize() > 0) {
        IOnWindowFocusChangeListener* listener = NULL;
        for (Int32 idx=0; idx<(Int32)listeners->GetSize(); ++idx) {
            listener = (*listeners)[idx];
            listener->OnWindowFocusChanged(hasFocus);
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
    // ==================before translated======================
    // // NOTE: because of the use of CopyOnWriteArrayList, we *must* use an iterator to
    // // perform the dispatching. The iterator is a safe guard against listeners that
    // // could mutate the list by calling the various add/remove methods. This prevents
    // // the array from being modified while we iterate it.
    // final CopyOnWriteArrayList<OnGlobalFocusChangeListener> listeners = mOnGlobalFocusListeners;
    // if (listeners != null && listeners.size() > 0) {
    //     for (OnGlobalFocusChangeListener listener : listeners) {
    //         listener.onGlobalFocusChanged(oldFocus, newFocus);
    //     }
    // }

    AutoPtr< List<IOnGlobalFocusChangeListener*> > listeners = mOnGlobalFocusListeners;
    if (listeners != NULL && listeners->GetSize() > 0) {
        IOnGlobalFocusChangeListener* listener = NULL;
        for (Int32 idx=0; idx<(Int32)listeners->GetSize(); ++idx) {
            listener = (*listeners)[idx];
            listener->OnGlobalFocusChanged(oldFocus, newFocus);
        }
    }
    return NOERROR;
}

ECode ViewTreeObserver::DispatchOnGlobalLayout()
{
    // ==================before translated======================
    // // NOTE: because of the use of CopyOnWriteArrayList, we *must* use an iterator to
    // // perform the dispatching. The iterator is a safe guard against listeners that
    // // could mutate the list by calling the various add/remove methods. This prevents
    // // the array from being modified while we iterate it.
    // final CopyOnWriteArray<OnGlobalLayoutListener> listeners = mOnGlobalLayoutListeners;
    // if (listeners != null && listeners.size() > 0) {
    //     CopyOnWriteArray.Access<OnGlobalLayoutListener> access = listeners.start();
    //     try {
    //         int count = access.size();
    //         for (int i = 0; i < count; i++) {
    //             access.get(i).onGlobalLayout();
    //         }
    //     } finally {
    //         listeners.end();
    //     }
    // }

    if (!mOnGlobalLayoutListeners->IsEmpty()) {
        List<IOnGlobalLayoutListener*>::Iterator iter = mOnGlobalLayoutListeners->Begin();
        for (; iter != mOnGlobalLayoutListeners->End(); ++iter) {
            assert((*iter) != NULL);
            (*iter)->OnGlobalLayout();
        }
    }
    return NOERROR;
}

ECode ViewTreeObserver::HasOnPreDrawListeners(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mOnPreDrawListeners != null && mOnPreDrawListeners.size() > 0;

    *result = (mOnPreDrawListeners != NULL && mOnPreDrawListeners->GetSize() > 0);
    return NOERROR;
}

ECode ViewTreeObserver::DispatchOnPreDraw(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // boolean cancelDraw = false;
    // final CopyOnWriteArray<OnPreDrawListener> listeners = mOnPreDrawListeners;
    // if (listeners != null && listeners.size() > 0) {
    //     CopyOnWriteArray.Access<OnPreDrawListener> access = listeners.start();
    //     try {
    //         int count = access.size();
    //         for (int i = 0; i < count; i++) {
    //             cancelDraw |= !(access.get(i).onPreDraw());
    //         }
    //     } finally {
    //         listeners.end();
    //     }
    // }
    // return cancelDraw;

    if (mOnPreDrawListeners != NULL && !mOnPreDrawListeners->IsEmpty()) {
        Boolean isPreDraw;
        List<IOnPreDrawListener*>::Iterator iter = mOnPreDrawListeners->Begin();
        for (; iter != mOnPreDrawListeners->End(); ++iter) {
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
    // ==================before translated======================
    // if (mOnDrawListeners != null) {
    //     final ArrayList<OnDrawListener> listeners = mOnDrawListeners;
    //     int numListeners = listeners.size();
    //     for (int i = 0; i < numListeners; ++i) {
    //         listeners.get(i).onDraw();
    //     }
    // }

    if (mOnDrawListeners != NULL) {
        List<IOnDrawListener*>::Iterator iter = mOnDrawListeners->Begin();
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
    // ==================before translated======================
    // final CopyOnWriteArrayList<OnTouchModeChangeListener> listeners =
    //         mOnTouchModeChangeListeners;
    // if (listeners != null && listeners.size() > 0) {
    //     for (OnTouchModeChangeListener listener : listeners) {
    //         listener.onTouchModeChanged(inTouchMode);
    //     }
    // }

    if (mOnTouchModeChangeListeners != NULL && mOnTouchModeChangeListeners->GetSize() > 0) {
        List<IOnTouchModeChangeListener*>::Iterator iter = mOnTouchModeChangeListeners->Begin();
        for (; iter != mOnTouchModeChangeListeners->End(); ++iter) {
            assert((*iter) != NULL);
            (*iter)->OnTouchModeChanged(inTouchMode);
        }
    }
    return NOERROR;
}

ECode ViewTreeObserver::DispatchOnScrollChanged()
{
    // ==================before translated======================
    // // NOTE: because of the use of CopyOnWriteArrayList, we *must* use an iterator to
    // // perform the dispatching. The iterator is a safe guard against listeners that
    // // could mutate the list by calling the various add/remove methods. This prevents
    // // the array from being modified while we iterate it.
    // final CopyOnWriteArray<OnScrollChangedListener> listeners = mOnScrollChangedListeners;
    // if (listeners != null && listeners.size() > 0) {
    //     CopyOnWriteArray.Access<OnScrollChangedListener> access = listeners.start();
    //     try {
    //         int count = access.size();
    //         for (int i = 0; i < count; i++) {
    //             access.get(i).onScrollChanged();
    //         }
    //     } finally {
    //         listeners.end();
    //     }
    // }

    if (mOnScrollChangedListeners != NULL && mOnScrollChangedListeners->GetSize() > 0) {
        List<IOnScrollChangedListener*>::Iterator iter = mOnScrollChangedListeners->Begin();
        for (; iter != mOnScrollChangedListeners->End(); ++iter) {
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
    // ==================before translated======================
    // final CopyOnWriteArray<OnComputeInternalInsetsListener> listeners =
    //         mOnComputeInternalInsetsListeners;
    // return (listeners != null && listeners.size() > 0);

    *result = (mOnComputeInternalInsetsListeners != NULL && mOnComputeInternalInsetsListeners->GetSize() > 0);
    return NOERROR;
}

ECode ViewTreeObserver::DispatchOnComputeInternalInsets(
    /* [in] */ IInternalInsetsInfo* inoutInfo)
{
    VALIDATE_NOT_NULL(inoutInfo);
    // ==================before translated======================
    // // NOTE: because of the use of CopyOnWriteArrayList, we *must* use an iterator to
    // // perform the dispatching. The iterator is a safe guard against listeners that
    // // could mutate the list by calling the various add/remove methods. This prevents
    // // the array from being modified while we iterate it.
    // final CopyOnWriteArray<OnComputeInternalInsetsListener> listeners =
    //         mOnComputeInternalInsetsListeners;
    // if (listeners != null && listeners.size() > 0) {
    //     CopyOnWriteArray.Access<OnComputeInternalInsetsListener> access = listeners.start();
    //     try {
    //         int count = access.size();
    //         for (int i = 0; i < count; i++) {
    //             access.get(i).onComputeInternalInsets(inoutInfo);
    //         }
    //     } finally {
    //         listeners.end();
    //     }
    // }

    if (mOnComputeInternalInsetsListeners != NULL && mOnComputeInternalInsetsListeners->GetSize() > 0) {
        List<IOnComputeInternalInsetsListener*>::Iterator iter = mOnComputeInternalInsetsListeners->Begin();
        for (; iter != mOnComputeInternalInsetsListeners->End(); ++iter) {
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

