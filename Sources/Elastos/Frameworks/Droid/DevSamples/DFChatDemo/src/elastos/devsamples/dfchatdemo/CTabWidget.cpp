#include "CTabWidget.h"
#include "R.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.Content.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Widget::ITabHost;
using Elastos::Droid::Widget::ITabSpec;
using Elastos::Core::CoreUtils;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace DFChatDemo {

static const String TAG("DFChatDemo.CTabWidget");

CAR_OBJECT_IMPL(CTabWidget)

ECode CTabWidget::constructor()
{
    return TabActivity::constructor();
}

ECode CTabWidget::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Logger::I(TAG, "onCreate()");
    TabActivity::OnCreate(savedInstanceState);
    SetContentView(R::layout::main);

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);

    AutoPtr<ITabHost> tabHost;
    GetTabHost((ITabHost**)&tabHost);

    AutoPtr<ITabSpec> spec;
    AutoPtr<IIntent> intent;
    AutoPtr<IDrawable> drawable;

    AutoPtr<ICharSequence> emptySeq = CoreUtils::Convert("");
    CIntent::New(this, ECLSID_CUseActivity, (IIntent**)&intent);
    res->GetDrawable(R::drawable::ic_tab_use, (IDrawable**)&drawable);
    tabHost->NewTabSpec(String("use"), (ITabSpec**)&spec);
    spec->SetIndicator(emptySeq, drawable);
    spec->SetContent(intent);
    tabHost->AddTab(spec);

    intent = NULL; drawable = NULL; spec = NULL;
    CIntent::New(this, ECLSID_CHostActivity, (IIntent**)&intent);;
    res->GetDrawable(R::drawable::ic_tab_host, (IDrawable**)&drawable);
    tabHost->NewTabSpec(String("host"), (ITabSpec**)&spec);
    spec->SetIndicator(emptySeq, drawable);
    spec->SetContent(intent);
    tabHost->AddTab(spec);

    tabHost->SetCurrentTab(0);
    return NOERROR;
}

} // namespace DFChatDemo
} // namespace DevSamples
} // namespace Elastos