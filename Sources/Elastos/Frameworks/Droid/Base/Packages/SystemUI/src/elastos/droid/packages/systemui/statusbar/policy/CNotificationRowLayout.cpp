#include "elastos/droid/systemui/statusbar/policy/CNotificationRowLayout.h"

using Elastos::Droid::View::EIID_View;
using Elastos::Droid::View::EIID_ViewGroup;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

IVIEW_METHODS_IMPL(CNotificationRowLayout, NotificationRowLayout)
IVIEWGROUP_METHODS_IMPL(CNotificationRowLayout, NotificationRowLayout)
IVIEWPARENT_METHODS_IMPL(CNotificationRowLayout, NotificationRowLayout)
IVIEWMANAGER_METHODS_IMPL(CNotificationRowLayout, NotificationRowLayout)
IDRAWABLECALLBACK_METHODS_IMPL(CNotificationRowLayout, NotificationRowLayout)
IKEYEVENTCALLBACK_METHODS_IMPL(CNotificationRowLayout, NotificationRowLayout)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CNotificationRowLayout, NotificationRowLayout)
ILINEARLAYOUT_METHODS_IMPL(CNotificationRowLayout, NotificationRowLayout)

PInterface CNotificationRowLayout::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    else if (riid == EIID_ViewGroup) {
        return reinterpret_cast<PInterface>((ViewGroup*)this);
    }
    return _CNotificationRowLayout::Probe(riid);
}

ECode CNotificationRowLayout::constructor(
    /* [in] */ IContext* ctx,
    /* [in] */ IAttributeSet* attrs)
{
    return NotificationRowLayout::Init(ctx, attrs);
}

ECode CNotificationRowLayout::constructor(
    /* [in] */ IContext* ctx,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return NotificationRowLayout::Init(ctx, attrs, defStyle);
}

ECode CNotificationRowLayout::SetLongPressListener(
    /* [in] */ IViewOnLongClickListener* listener)
{
    return NotificationRowLayout::SetLongPressListener(listener);
}

ECode CNotificationRowLayout::SetOnSizeChangedListener(
    /* [in] */ IOnSizeChangedListener* l)
{
    return NotificationRowLayout::SetOnSizeChangedListener(l);
}

ECode CNotificationRowLayout::SetAnimateBounds(
    /* [in] */ Boolean anim)
{
    return NotificationRowLayout::SetAnimateBounds(anim);
}

ECode CNotificationRowLayout::CanChildBeDismissed(
    /* [in] */ IView* v,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NotificationRowLayout::CanChildBeDismissed(v);
    return NOERROR;
}

ECode CNotificationRowLayout::CanChildBeExpanded(
    /* [in] */ IView* v,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NotificationRowLayout::CanChildBeExpanded(v);
    return NOERROR;
}

ECode CNotificationRowLayout::SetUserExpandedChild(
    /* [in] */ IView* v,
    /* [in] */ Boolean userExpanded,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NotificationRowLayout::SetUserExpandedChild(v, userExpanded);
    return NOERROR;
}

ECode CNotificationRowLayout::SetUserLockedChild(
    /* [in] */ IView* v,
    /* [in] */ Boolean userLocked,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NotificationRowLayout::SetUserLockedChild(v, userLocked);
    return NOERROR;
}

ECode CNotificationRowLayout::OnChildDismissed(
    /* [in] */ IView* v)
{
    return NotificationRowLayout::OnChildDismissed(v);
}

ECode CNotificationRowLayout::OnBeginDrag(
    /* [in] */ IView* v)
{
    return NotificationRowLayout::OnBeginDrag(v);
}

ECode CNotificationRowLayout::OnDragCancelled(
    /* [in] */ IView* v)
{
    return NotificationRowLayout::OnDragCancelled(v);
}

ECode CNotificationRowLayout::GetChildAtPosition(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    *view = NotificationRowLayout::GetChildAtPosition(ev);
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CNotificationRowLayout::GetChildAtRawPosition(
    /* [in] */ Float touchX,
    /* [in] */ Float touchY,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    *view = NotificationRowLayout::GetChildAtRawPosition(touchX, touchY);
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CNotificationRowLayout::GetChildAtPosition(
    /* [in] */ Float touchX,
    /* [in] */ Float touchY,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    *view = NotificationRowLayout::GetChildAtPosition(touchX, touchY);
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CNotificationRowLayout::GetChildContentView(
    /* [in] */ IView* v,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    *view = NotificationRowLayout::GetChildContentView(v);
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CNotificationRowLayout::SetViewRemoval(
    /* [in] */ Boolean removeViews)
{
    return NotificationRowLayout::SetViewRemoval(removeViews);
}

ECode CNotificationRowLayout::SetLayoutTransitionsEnabled(
    /* [in] */ Boolean b)
{
    return NotificationRowLayout::SetLayoutTransitionsEnabled(b);
}

ECode CNotificationRowLayout::DismissRowAnimated(
    /* [in] */ IView* child)
{
    return NotificationRowLayout::DismissRowAnimated(child);
}

ECode CNotificationRowLayout::DismissRowAnimated(
    /* [in] */ IView* child,
    /* [in] */ Int32 vel)
{
    return NotificationRowLayout::DismissRowAnimated(child, vel);
}

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
