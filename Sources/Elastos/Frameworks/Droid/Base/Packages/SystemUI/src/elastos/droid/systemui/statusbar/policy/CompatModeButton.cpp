#include "elastos/droid/systemui/statusbar/policy/CompatModeButton.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

const Boolean CompatModeButton::DEBUG = FALSE;

CompatModeButton::CompatModeButton()
{
}

CompatModeButton::CompatModeButton(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : ImageView(context, attrs)
{
    InitImpl(context, attrs, 0);
}

CompatModeButton::CompatModeButton(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
    : ImageView(context, attrs, defStyle)
{
    InitImpl(context, attrs, defStyle);
}

ECode CompatModeButton::Init(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return Init(context, attrs, 0);
}

ECode CompatModeButton::Init(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    ImageView::Init(context, attrs, defStyle);
    return InitImpl(context, attrs, defStyle);
}

ECode CompatModeButton::InitImpl(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    SetClickable(TRUE);

    AutoPtr<IInterface> tmpObj;
    context->GetSystemService(IContext::ACTIVITY_SERVICE, (IInterface**)&tmpObj);
    mAM = IActivityManager::Probe(tmpObj.Get());
    assert(mAM != NULL);
    return Refresh();
}

ECode CompatModeButton::Refresh()
{
    Int32 mode;
    mAM->GetFrontActivityScreenCompatMode(&mode);
    if (mode == IActivityManager::COMPAT_MODE_UNKNOWN) {
        // If in an unknown state, don't change.
        return NOERROR;
    }
    Boolean vis = (mode != IActivityManager::COMPAT_MODE_NEVER
        && mode != IActivityManager::COMPAT_MODE_ALWAYS);
    // if (DEBUG)
    {
        Slogger::D("CompatModeButton", "compat mode is %d; compat mode icon will %s", mode, (vis ? "show" : "hide"));
        Slogger::D("CompatModeButton", "todo: disable compat mode icon on statusbar by luo.zhaohui");
    }

    vis = FALSE;// TODO: disable compat mode icon on statusbar by luo.zhaohui
    SetVisibility(vis ? IView::VISIBLE : IView::GONE);
    return NOERROR;
}

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
