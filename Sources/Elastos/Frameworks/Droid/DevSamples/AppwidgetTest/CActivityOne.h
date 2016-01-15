
#ifndef __CACTIVITYONE_H__
#define __CACTIVITYONE_H__

#include "elastos/droid/app/Activity.h"
#include "_CActivityOne.h"

using Elastos::ElRefBase;
using Elastos::Droid::App::Activity;
using Elastos::Droid::AppWidget::IAppWidgetHostView;
using Elastos::Droid::AppWidget::IAppWidgetProviderInfo;
using Elastos::Droid::AppWidget::IAppWidgetManager;
using Elastos::Droid::AppWidget::IAppWidgetHost;
using Elastos::Droid::View::IViewOnTouchListener;
using Elastos::Droid::View::IViewOnKeyListener;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::IAdapterViewOnItemLongClickListener;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IPopupWindow;
using Elastos::Droid::Widget::ICheckBox;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::ILinearLayout;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace AppwidgetTest {


class CActivityOne : public Activity
{
public:
    class MyListener
    : public IViewOnTouchListener
    , public IAdapterViewOnItemClickListener
    , public IAdapterViewOnItemLongClickListener
    , public IViewOnClickListener
    , public ElRefBase
    {
    public:
        MyListener(
            /* [in] */ CActivityOne* host);

        CARAPI_(PInterface) Probe(
            /* [in]  */ REIID riid);

        CARAPI_(UInt32) AddRef();

        CARAPI_(UInt32) Release();

        CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

        CARAPI OnTouch(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);

        CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);

        CARAPI OnClick(
            /* [in] */ IView* v);

        CARAPI OnItemLongClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id,
            /* [out] */ Boolean* result);
    private:
        AutoPtr<CActivityOne> mHost;
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
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent *data);

    CARAPI_(AutoPtr<IAppWidgetHostView>) GetHostView(
        /* [in] */ Int32 appwidgetId,
        /* [in] */ IAppWidgetProviderInfo* appwidgetInfo);

private:
    static const Int32 HOST_ID = 1048;
    AutoPtr<IListView> mListView;
    AutoPtr<IAppWidgetManager> m_appwidgetmgr;
    AutoPtr<IAppWidgetHost> m_appWidgetHost;
    AutoPtr<ILinearLayout> m_appContainer;
    AutoPtr<IObjectContainer> m_Providers;
    AutoPtr<IView> mSelectedView;
};

} // namespace AppwidgetTest
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CACTIVITYONE_H__
