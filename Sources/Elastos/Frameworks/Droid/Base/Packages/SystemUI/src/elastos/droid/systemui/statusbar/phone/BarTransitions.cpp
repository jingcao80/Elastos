
#include "elastos/droid/systemui/statusbar/phone/BarTransitions.h"
#include "elastos/droid/systemui/statusbar/phone/CBarBackgroundDrawable.h"
#include "../R.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include <elastos/droid/R.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

const Int32 BarTransitions::MODE_OPAQUE;
const Int32 BarTransitions::MODE_SEMI_TRANSPARENT;
const Int32 BarTransitions::MODE_TRANSLUCENT;
const Int32 BarTransitions::MODE_LIGHTS_OUT;
const Int32 BarTransitions::MODE_TRANSPARENT;
const Int32 BarTransitions::MODE_WARNING;
const Int32 BarTransitions::LIGHTS_IN_DURATION;
const Int32 BarTransitions::LIGHTS_OUT_DURATION;
const Int32 BarTransitions::BACKGROUND_DURATION;
const Boolean BarTransitions::DEBUG = FALSE;
const Boolean BarTransitions::DEBUG_COLORS = FALSE;
const Boolean BarTransitions::HIGH_END = InitStatic();

CAR_INTERFACE_IMPL(BarTransitions, Object, IBarTransitions)
BarTransitions::BarTransitions(
    /* [in] */ IView* view,
    /* [in] */ Int32 gradientResourceId)
    : mMode(0)
{
    mTag = String("BarTransitions. TODO: View XXX")/* + view.getClass().getSimpleName()*/;
    mView = view;
    AutoPtr<IContext> ctx;
    mView->GetContext((IContext**)&ctx);
    CBarBackgroundDrawable::New(ctx, gradientResourceId, (IDrawable**)&mBarBackground);
    if (HIGH_END) {
        mView->SetBackground(mBarBackground);
    }
}

Boolean BarTransitions::InitStatic()
{
    AutoPtr<IActivityManagerHelper> helper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&helper);
    Boolean value = FALSE;
    helper->IsHighEndGfx(&value);
    return value;
}

ECode BarTransitions::GetMode(
    /* [out] */ Int32* mode)
{
    VALIDATE_NOT_NULL(mode);
    *mode = mMode;
    return NOERROR;
}

ECode BarTransitions::TransitionTo(
    /* [in] */ Int32 mode,
    /* [in] */ Boolean animate)
{
    // low-end devices do not support translucent modes, fallback to opaque
    if (!HIGH_END && (mode == MODE_SEMI_TRANSPARENT || mode == MODE_TRANSLUCENT
            || mode == MODE_TRANSPARENT)) {
        mode = MODE_OPAQUE;
    }
    if (mMode == mode) return NOERROR;
    Int32 oldMode = mMode;
    mMode = mode;
    if (DEBUG) Logger::D(mTag, "%s -> %s animate=%d",
            ModeToString(oldMode).string(), ModeToString(mode).string(),  animate);
    OnTransition(oldMode, mMode, animate);
    return NOERROR;
}

void BarTransitions::OnTransition(
    /* [in] */ Int32 oldMode,
    /* [in] */ Int32 newMode,
    /* [in] */ Boolean animate)
{
    if (HIGH_END) {
        ApplyModeBackground(oldMode, newMode, animate);
    }
}

void BarTransitions::ApplyModeBackground(
    /* [in] */ Int32 oldMode,
    /* [in] */ Int32 newMode,
    /* [in] */ Boolean animate)
{
    if (DEBUG) Logger::D(mTag, "applyModeBackground oldMode=%s newMode=%s animate=%d",
            ModeToString(oldMode).string(), ModeToString(newMode).string(), animate);
    ((CBarBackgroundDrawable*)mBarBackground.Get())->ApplyModeBackground(oldMode, newMode, animate);
}

String BarTransitions::ModeToString(
    /* [in] */ Int32 mode)
{
    if (mode == MODE_OPAQUE) return String("MODE_OPAQUE");
    if (mode == MODE_SEMI_TRANSPARENT) return String("MODE_SEMI_TRANSPARENT");
    if (mode == MODE_TRANSLUCENT) return String("MODE_TRANSLUCENT");
    if (mode == MODE_LIGHTS_OUT) return String("MODE_LIGHTS_OUT");
    if (mode == MODE_TRANSPARENT) return String("MODE_TRANSPARENT");
    if (mode == MODE_WARNING) return String("MODE_WARNING");
    // throw new IllegalArgumentException("Unknown mode " + mode);
    return String("Unknown mode ") + StringUtils::ToString(mode);
}

ECode BarTransitions::FinishAnimations()
{
    ((CBarBackgroundDrawable*)mBarBackground.Get())->FinishAnimation();
    return NOERROR;
}

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
