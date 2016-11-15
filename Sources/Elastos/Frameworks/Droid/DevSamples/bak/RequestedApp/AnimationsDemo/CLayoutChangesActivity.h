
#ifndef __CLAYOUTCHANGESACTIVITY_H__
#define __CLAYOUTCHANGESACTIVITY_H__

#include "_CLayoutChangesActivity.h"
#include "elastos/droid/app/Activity.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewOnClickListener;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace AnimationsDemo {

/**
 * This sample demonstrates how to use system-provided, automatic layout transitions. Layout
 * transitions are animations that occur when views are added to, removed from, or changed within
 * a {@link ViewGroup}.
 *
 * <p>In this sample, the user can add rows to and remove rows from a vertical
 * {@link android.widget.LinearLayout}.</p>
 */
class CLayoutChangesActivity : public Activity
{
private:
    class MyListener
        : public IViewOnClickListener
        , public ElRefBase
    {
    public:
        MyListener(
            /* [in] */ CLayoutChangesActivity* host,
            /* [in] */ IView* view);

        CARAPI_(PInterface) Probe(
            /* [in]  */ REIID riid);

        CARAPI_(UInt32) AddRef();

        CARAPI_(UInt32) Release();

        CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CLayoutChangesActivity* mHost;
        AutoPtr<IView> mView;
    };

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnCreateOptionsMenu(
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* res);

    CARAPI OnOptionsItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* res);

private:
    CARAPI_(void) AddItem();

private:
    /**
     * The container view which has layout change animations turned on. In this sample, this view
     * is a {@link android.widget.LinearLayout}.
     */
    AutoPtr<IViewGroup> mContainerView;

    /**
     * A static list of country names.
     */
    static const String COUNTRIES[];
};

} // namespace AnimationsDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif  //__CLAYOUTCHANGESACTIVITY_H__
