#include "elastos/droid/systemui/statusbar/tablet/NotificationPanelTitle.h"
#include "elastos/droid/systemui/SystemUIR.h"
#include "elastos/droid/view/accessibility/CAccessibilityEventHelper.h"
#include "elastos/droid/view/SoundEffectConstants.h"

using Elastos::Droid::View::SoundEffectConstants;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::Accessibility::IAccessibilityEventHelper;
using Elastos::Droid::View::Accessibility::CAccessibilityEventHelper;
using Elastos::Droid::SystemUI::SystemUIR;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {


NotificationPanelTitle::NotificationPanelTitle()
{}

NotificationPanelTitle::NotificationPanelTitle(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : RelativeLayout(context, attrs)
{
    InitImpl(context, attrs);
}

ECode NotificationPanelTitle::Init(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    RelativeLayout::Init(context, attrs);
    return InitImpl(context, attrs);
}

ECode NotificationPanelTitle::InitImpl(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    SetOnClickListener(THIS_PROBE(IViewOnClickListener));
    return NOERROR;
}

ECode NotificationPanelTitle::SetPanel(
    /* [in] */ INotificationPanel* b)
{
    mPanel = b;
    return NOERROR;
}

ECode NotificationPanelTitle::OnFinishInflate()
{
    RelativeLayout::OnFinishInflate();

    mSettingsButton = FindViewById(SystemUIR::id::settings_button);
    mButtons.PushBack(mSettingsButton);

    AutoPtr<IView> tmpView = FindViewById(SystemUIR::id::notification_button);
    mButtons.PushBack(tmpView);
    return NOERROR;
}

ECode NotificationPanelTitle::SetPressed(
    /* [in] */ Boolean pressed)
{
    RelativeLayout::SetPressed(pressed);
    List<AutoPtr<IView> >::Iterator it;
    for (it = mButtons.Begin(); it != mButtons.End(); ++it) {
        AutoPtr<IView> button = *it;
        if (button != NULL) {
            button->SetPressed(pressed);
        }
    }
    return NOERROR;
}

Boolean NotificationPanelTitle::OnTouchEvent(
    /* [in] */ IMotionEvent* e)
{
    Boolean enable;
    mSettingsButton->IsEnabled(&enable);
    if (!enable)
        return FALSE;

    Int32 action;
    e->GetAction(&action);
    switch (action) {
        case IMotionEvent::ACTION_DOWN:
            SetPressed(TRUE);
            break;
        case IMotionEvent::ACTION_MOVE: {
                Float fx, fy;
                e->GetX(&fx);
                e->GetX(&fy);
                Int32 x = fx;
                Int32 y = fy;
                SetPressed(x > 0 && x < GetWidth() && y > 0 && y < GetHeight());
            }
            break;
        case IMotionEvent::ACTION_UP:
            if (IsPressed()) {
                PlaySoundEffect(SoundEffectConstants::CLICK);
                mPanel->SwapPanels();
                SetPressed(FALSE);
            }
            break;
        case IMotionEvent::ACTION_CANCEL:
            SetPressed(FALSE);
            break;
    }
    return TRUE;
}

ECode NotificationPanelTitle::OnClick(
    /* [in] */ IView* v)
{
    Boolean enable;
    mSettingsButton->IsEnabled(&enable);
    if (enable && v == THIS_PROBE(IView)) {
        mPanel->SwapPanels();
    }
    return NOERROR;
}

Boolean NotificationPanelTitle::OnRequestSendAccessibilityEvent(
    /* [in] */ IView* child,
    /* [in] */ IAccessibilityEvent* event)
{
    if (RelativeLayout::OnRequestSendAccessibilityEvent(child, event)) {
        AutoPtr<IAccessibilityEventHelper> helper;
        CAccessibilityEventHelper::AcquireSingleton((IAccessibilityEventHelper**)&helper);
        AutoPtr<IAccessibilityEvent> record;
        helper->Obtain((IAccessibilityEvent**)&record);

        OnInitializeAccessibilityEvent(record);
        DispatchPopulateAccessibilityEvent(record);
        event->AppendRecord(record);
        return TRUE;
    }
    return FALSE;
}


}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
