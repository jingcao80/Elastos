
#ifndef __ELASTOS_DROID_INTERNAL_APP_NAVITEMSELECTEDLISTENER_H__
#define __ELASTOS_DROID_INTERNAL_APP_NAVITEMSELECTEDLISTENER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IActionBarOnNavigationListener;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IAdapterViewOnItemSelectedListener;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

/**
 * Wrapper to adapt the ActionBar.OnNavigationListener in an AdapterView.OnItemSelectedListener
 * for use in Spinner widgets. Used by action bar implementations.
 */
class NavItemSelectedListener
    : public Object
    , public IAdapterViewOnItemSelectedListener
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IActionBarOnNavigationListener* listener);

    CARAPI OnItemSelected(
        /* [in] */ IAdapterView* parent,
        /* [in] */ IView* view,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

    CARAPI OnNothingSelected(
        /* [in] */ IAdapterView* parent);

private:
    AutoPtr<IActionBarOnNavigationListener> mListener;
};

} // namespace App
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_APP_NAVITEMSELECTEDLISTENER_H__
