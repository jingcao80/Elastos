#include "elastos/droid/systemui/egg/LLandActivity.h"
#include "elastos/droid/systemui/egg/LLand.h"
#include "R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Egg {

ECode LLandActivity::constructor()
{
    return Activity::constructor();
}

ECode LLandActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    Activity::SetContentView(R::layout::lland);
    AutoPtr<IView> view;
    Activity::FindViewById(R::id::world, (IView**)&view);
    AutoPtr<LLand> world = (LLand*)(view.Get());

    view = NULL;
    Activity::FindViewById(R::id::score, (IView**)&view);
    world->SetScoreField(ITextView::Probe(view));
    view = NULL;
    Activity::FindViewById(R::id::welcome, (IView**)&view);
    world->SetSplash(view);
    Boolean result;
    world->RequestFocus(&result);
    Logger::V(LLand::TAG, "focus: %s", result ? "TRUE" : "FALSE");
    return NOERROR;
}

} // namespace Egg
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos