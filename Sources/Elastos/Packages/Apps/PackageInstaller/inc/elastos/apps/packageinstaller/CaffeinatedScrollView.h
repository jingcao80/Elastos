
#ifndef __ELASTOS_APPS_PACKAGEINSTALLER_CAFFEINATEDSCROLLVIEW_H__
#define __ELASTOS_APPS_PACKAGEINSTALLER_CAFFEINATEDSCROLLVIEW_H__

#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.Droid.Widget.h>
#include <elastos/core/Object.h>
#include <Elastos.CoreLibrary.Core.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Utility::IAttributeSet;
//using Elastos::Droid::Widget::ScrollView;
using Elastos::Core::IRunnable;
using Elastos::Core::Object;

namespace Elastos {
namespace Apps {
namespace PackageInstaller {

/**
  * It's a ScrollView that knows how to stay awake.
  */
class CaffeinatedScrollView
    : public Object // ScrollView
{
public:
    CaffeinatedScrollView();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    /**
      * Make this visible so we can call it
      */
    // @Override
    CARAPI AwakenScrollBars(
        /* [out] */ Boolean* result);

    virtual CARAPI SetFullScrollAction(
        /* [in] */ IRunnable* action);

protected:
    // @Override
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

    // @Override
    CARAPI_(void) OnScrollChanged(
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 oldl,
        /* [in] */ Int32 oldt);

private:
    CARAPI_(void) CheckFullScrollAction();

private:
    AutoPtr<IRunnable> mFullScrollAction;
    Int32 mBottomSlop;
};

} // namespace PackageInstaller
} // namespace Apps
} // namespace Elastos

#endif // __ELASTOS_APPS_PACKAGEINSTALLER_CAFFEINATEDSCROLLVIEW_H__

