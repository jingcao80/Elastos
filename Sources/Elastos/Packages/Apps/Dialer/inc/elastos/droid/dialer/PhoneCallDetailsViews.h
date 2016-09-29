#ifndef __ELASTOS_DROID_DIALER_PHONECALLDETAILSVIEWS_H__
#define __ELASTOS_DROID_DIALER_PHONECALLDETAILSVIEWS_H__

#include "_Elastos.Droid.Dialer.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Dialer::CallLog::ICallTypeIconsView;

namespace Elastos {
namespace Droid {
namespace Dialer {

/**
 * Encapsulates the views that are used to display the details of a phone call in the call log.
 */
class PhoneCallDetailsViews : public Object
{
public:
    /**
     * Create a new instance by extracting the elements from the given view.
     * <p>
     * The view should contain three text views with identifiers {@code R.id.name},
     * {@code R.id.date}, and {@code R.id.number}, and a linear layout with identifier
     * {@code R.id.call_types}.
     */
    static CARAPI_(AutoPtr<PhoneCallDetailsViews>) FromView(
        /* [in] */ IView* view);

    static CARAPI_(AutoPtr<PhoneCallDetailsViews>) CreateForTest(
        /* [in] */ IContext* context);

private:
    PhoneCallDetailsViews(
        /* [in] */ ITextView* nameView,
        /* [in] */ IView* callTypeView,
        /* [in] */ ICallTypeIconsView* callTypeIcons,
        /* [in] */ IImageView* callAccountIcon,
        /* [in] */ ITextView* callLocationAndDate,
        /* [in] */ ITextView* voicemailTranscriptionView);

public:
    AutoPtr<ITextView> mNameView;
    AutoPtr<IView> mCallTypeView;
    AutoPtr<ICallTypeIconsView> mCallTypeIcons;
    AutoPtr<IImageView> mCallAccountIcon;
    AutoPtr<ITextView> mCallLocationAndDate;
    AutoPtr<ITextView> mVoicemailTranscriptionView;
};

} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_PHONECALLDETAILSVIEWS_H__
