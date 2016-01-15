
#ifndef __ELASTOS_DROID_APP_ACTIONBARLAYOUTPARAMS_H__
#define __ELASTOS_DROID_APP_ACTIONBARLAYOUTPARAMS_H__

#include "Elastos.Droid.App.h"
#include "elastos/droid/view/ViewGroup.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::ViewGroup;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewGroupMarginLayoutParams;

namespace Elastos {
namespace Droid {
namespace App {

/**
 * Per-child layout information associated with action bar custom views.
 *
 * @attr ref android.R.styleable#ActionBar_LayoutParams_layout_gravity
 */
class ActionBarLayoutParams
    : public ViewGroup::MarginLayoutParams
    , public IActionBarLayoutParams
{
public:
    CAR_INTERFACE_DECL()

    ActionBarLayoutParams();

    virtual ~ActionBarLayoutParams();

    CARAPI constructor(
        /* [in] */ IContext* ctx,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI constructor(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 gravity);

    CARAPI constructor(
        /* [in] */ Int32 gravity);

    CARAPI constructor(
        /* [in] */ IActionBarLayoutParams* source);

    CARAPI constructor(
        /* [in] */ IViewGroupLayoutParams* source);

    CARAPI GetGravity(
        /* [out] */ Int32* gravity);

    CARAPI SetGravity(
        /* [in] */ Int32 gravity);

public:
    Int32 mGravity;

    /*
     * Note for framework developers:
     *
     * You might notice that ActionBar.LayoutParams is missing a constructor overload
     * for MarginLayoutParams. While it may seem like a good idea to add one, at this
     * point it's dangerous for source compatibility. Upon building against a new
     * version of the SDK an app can end up statically linking to the new MarginLayoutParams
     * overload, causing a crash when running on older platform versions with no other changes.
     */
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_ACTIONBARLAYOUTPARAMS_H__
