
#ifndef __Elastos_DevSamples_PopupMenuDemo_CACTIVITYONE_H__
#define __Elastos_DevSamples_PopupMenuDemo_CACTIVITYONE_H__

#include "_Elastos_DevSamples_PopupMenuDemo_CActivityOne.h"
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.View.h>
#include <Elastos.Droid.Widget.h>
#include <elastos/droid/app/Activity.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IPopupMenu;
using Elastos::Droid::Widget::IPopupMenuOnMenuItemClickListener;

namespace Elastos {
namespace DevSamples {
namespace PopupMenuDemo {

class CActivityOne
    : public Activity
{
public:
    class MyListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyListener(
            /* [in] */ CActivityOne* host);

        CARAPI OnClick(
            /* [in] */ IView* v);
    private:
        CActivityOne* mHost;
    };

    class MemuListener
        : public Object
        , public IPopupMenuOnMenuItemClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI OnMenuItemClick(
            /* [in] */ IMenuItem* item,
            /* [out] */ Boolean* result);
    };

public:
    CAR_OBJECT_DECL()

    CARAPI constructor();

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnStart();

    CARAPI OnResume();

    CARAPI OnPause();

    CARAPI OnStop();

    CARAPI OnDestroy();

private:
    AutoPtr<IButton> mButton;
};

} // namespace PopupMenuDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_PopupMenuDemo_CACTIVITYONE_H__
