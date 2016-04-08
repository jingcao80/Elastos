
#ifndef __CMAINACTIVITY_H__
#define __CMAINACTIVITY_H__

#include "_CMainActivity.h"
#include "elastos/droid/app/ListActivity.h"

using Elastos::Droid::App::ListActivity;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace AnimationsDemo {

/**
 * The launchpad activity for this sample project. This activity launches other activities that
 * demonstrate implementations of common animations.
 */
class CMainActivity : public ListActivity
{
public:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

protected:
    CARAPI OnListItemClick(
        /* [in] */ IListView* listView,
        /* [in] */ IView* view,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

private:
    /**
     * The collection of all samples in the app. This gets instantiated in {@link
     * #onCreate(android.os.Bundle)} because the {@link Sample} constructor needs access to {@link
     * android.content.res.Resources}.
     */
    static AutoPtr<IObjectContainer> mSamples;
};

} // namespace AnimationsDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif  // __CMAINACTIVITY_H__
