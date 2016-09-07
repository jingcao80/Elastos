
#ifndef __ELASTOS_DROID_CONTACTS_COMMON_LIST_VIEWPAGERTABSTRIP_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_LIST_VIEWPAGERTABSTRIP_H__

#include "_Elastos.Droid.Dialer.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/widget/LinearLayout.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Widget::LinearLayout;

namespace Elastos{
namespace Droid{
namespace Contacts {
namespace Common {
namespace List {

class ViewPagerTabStrip
    : public LinearLayout
{
public:
    ViewPagerTabStrip();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    /**
     * Notifies this view that view pager has been scrolled. We save the tab index
     * and selection offset for interpolating the position and width of selection
     * underline.
     */
    CARAPI_(void) OnPageScrolled(
        /* [in] */ Int32 position,
        /* [in] */ Float positionOffset,
        /* [in] */ Int32 positionOffsetPixels);

protected:
    // @Override
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

private:
    CARAPI_(Boolean) IsRtl();

private:
    Int32 mSelectedUnderlineThickness;
    AutoPtr<IPaint> mSelectedUnderlinePaint;

    Int32 mIndexForSelection;
    Float mSelectionOffset;
};

} // List
} // Common
} // Contacts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_LIST_VIEWPAGERTABSTRIP_H__
