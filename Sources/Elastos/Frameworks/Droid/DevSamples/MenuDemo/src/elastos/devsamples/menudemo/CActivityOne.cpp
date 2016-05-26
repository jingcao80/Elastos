
#include "CActivityOne.h"
#include "R.h"
#include <elastos/droid/R.h>
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.View.h>
#include <Elastos.Droid.Widget.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::View::ISubMenu;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace MenuDemo {

static const String TAG("MenuDemo::CActivityOne");

//=======================================================================
// CActivityOne
//=======================================================================

CAR_OBJECT_IMPL(CActivityOne)

ECode CActivityOne::constructor()
{
    Logger::I(TAG, " >> constructor()");
    return Activity::constructor();
}

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Logger::I(TAG, " >> OnCreate()");
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::main);
    return NOERROR;
}

ECode CActivityOne::OnStart()
{
    Logger::I(TAG, " >> OnStart()");
    return Activity::OnStart();
}

ECode CActivityOne::OnResume()
{
    Logger::I(TAG, " >> OnResume()");
    return Activity::OnResume();
}

ECode CActivityOne::OnPause()
{
    Logger::I(TAG, " >> OnPause()");
    return Activity::OnPause();
}

ECode CActivityOne::OnStop()
{
    Logger::I(TAG, " >> OnStop()");
    return Activity::OnStop();
}

ECode CActivityOne::OnDestroy()
{
    Logger::I(TAG, " >> OnDestroy()");
    return Activity::OnDestroy();
}

ECode CActivityOne::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    Logger::I(TAG, " >> OnActivityResult()");
    return Activity::OnActivityResult(requestCode, resultCode, data);
}

ECode CActivityOne::OnCreateOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* allowToShow)
{
    Logger::I(TAG, " >> OnCreateOptionsMenu()");
    VALIDATE_NOT_NULL(allowToShow)
    AutoPtr<ISubMenu> sub, temp;
    menu->AddSubMenu(
        IMenu::NONE, IMenu::FIRST, 0, CoreUtils::Convert("发送"), (ISubMenu**)&sub);
    sub->SetIcon(Elastos::Droid::R::drawable::ic_menu_send);

    temp = NULL;
    IMenu::Probe(sub)->AddSubMenu(
        IMenu::NONE, IMenu::FIRST + 4, 4, CoreUtils::Convert("发送到微信"), (ISubMenu**)&temp);
    temp = NULL;
    IMenu::Probe(sub)->AddSubMenu(
        IMenu::NONE, IMenu::FIRST + 5, 5, CoreUtils::Convert("发送到微博"), (ISubMenu**)&temp);

    sub = NULL;
    menu->AddSubMenu(
        IMenu::NONE, IMenu::FIRST + 1, 1, CoreUtils::Convert("编辑"), (ISubMenu**)&sub);
    sub->SetIcon(Elastos::Droid::R::drawable::ic_menu_edit);

    sub = NULL;
    menu->AddSubMenu(
        IMenu::NONE, IMenu::FIRST + 2, 2, CoreUtils::Convert("添加"), (ISubMenu**)&sub);
    sub->SetIcon(Elastos::Droid::R::drawable::ic_menu_add);

    sub = NULL;
    menu->AddSubMenu(
        IMenu::NONE, IMenu::FIRST + 3, 3, CoreUtils::Convert("删除"), (ISubMenu**)&sub);
    sub->SetIcon(Elastos::Droid::R::drawable::ic_menu_delete);

    *allowToShow = TRUE;
    return NOERROR;
}

ECode CActivityOne::OnOptionsItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    Logger::I(TAG, " >> OnOptionsItemSelected()");
    VALIDATE_NOT_NULL(result)
    AutoPtr<IView> view = FindViewById(R::id::tv);
    ITextView* tv = ITextView::Probe(view);

    Int32 itemId;
    item->GetItemId(&itemId);
    switch(itemId){
    case IMenu::FIRST:
        tv->SetText(CoreUtils::Convert("你点击了[发送]"));
        break;
    case IMenu::FIRST + 1:
        tv->SetText(CoreUtils::Convert("你点击了[编辑]"));
        break;
    case IMenu::FIRST + 2:
        tv->SetText(CoreUtils::Convert("你点击了[添加]"));
        break;
    case IMenu::FIRST + 3:
        tv->SetText(CoreUtils::Convert("你点击了[删除]"));
        break;
    case IMenu::FIRST + 4:
        tv->SetText(CoreUtils::Convert("你点击了[发送到微信]"));
        break;
    case IMenu::FIRST + 5:
        tv->SetText(CoreUtils::Convert("你点击了[发送到微博]"));
        break;
    }

    return Activity::OnOptionsItemSelected(item, result);

}

} // namespace MenuDemo
} // namespace DevSamples
} // namespace Elastos
