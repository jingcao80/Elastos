
#ifndef __CACTIVITYONE_H__
#define __CACTIVITYONE_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/app/Activity.h"
#include "_CActivityOne.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::IPopupMenu;
using Elastos::Droid::Widget::CPopupMenu;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IPopupMenuOnMenuItemClickListener;
using Elastos::Droid::Widget::EIID_IPopupMenuOnMenuItemClickListener;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace PopupMenuDemo {


class CActivityOne : public Activity
{
public:
    class MyListener
        : public IViewOnClickListener
        , public ElRefBase
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
        : public IPopupMenuOnMenuItemClickListener
        , public ElRefBase
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI OnMenuItemClick(
            /* [in] */ IMenuItem* item,
            /* [out] */ Boolean* result);
    };

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
} // namespace Droid
} // namespace Elastos

#endif // __CACTIVITYONE_H__
