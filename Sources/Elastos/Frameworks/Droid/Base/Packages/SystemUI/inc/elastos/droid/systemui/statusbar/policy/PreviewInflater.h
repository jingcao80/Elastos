
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_PREVIEWINFLATER_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_PREVIEWINFLATER_H__

#include "_SystemUI.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::SystemUI::StatusBar::Phone::IKeyguardPreviewContainer;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IWindowInsets;
using Elastos::Core::Object;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

/**
 * Utility class to inflate previews for phone and camera affordance.
 */
class PreviewInflater
    : public Object
    , public IPreviewInflater
{
private:
    class WidgetInfo: public Object
    {
    public:
        WidgetInfo();

        String mContextPackage;
        Int32 mLayoutId;
    };

public:
    CAR_INTERFACE_DECL();

    PreviewInflater(
        /* [in] */ IContext* context,
        /* [in] */ ILockPatternUtils* lockPatternUtils);

    CARAPI InflatePreview(
        /* [in] */ IIntent* intent,
        /* [out] */ IView** view);

    static CARAPI_(Boolean) WouldLaunchResolverActivity(
        /* [in] */ IContext* ctx,
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 currentUserId);

private:
    CARAPI_(AutoPtr<IView>) InflateWidgetView(
        /* [in] */ WidgetInfo* widgetInfo);

    CARAPI_(AutoPtr<WidgetInfo>) GetWidgetInfo(
        /* [in] */ IIntent* intent);

    static CARAPI_(Boolean) WouldLaunchResolverActivity(
        /* [in] */ IResolveInfo* resolved,
        /* [in] */ IList/*<ResolveInfo>*/* appList);

private:
    static const String TAG;
    static const String META_DATA_KEYGUARD_LAYOUT;

    AutoPtr<IContext> mContext;
    AutoPtr<ILockPatternUtils> mLockPatternUtils;
};

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_PREVIEWINFLATER_H__
