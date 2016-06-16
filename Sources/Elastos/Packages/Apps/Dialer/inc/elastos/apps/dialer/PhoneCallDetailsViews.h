#ifndef __ELASTOS_APPS_DIALER_PHONECALLDETAILSVIEWS_H__
#define __ELASTOS_APPS_DIALER_PHONECALLDETAILSVIEWS_H__

#include "Elastos.Droid.App.h"

using Elastos::Apps::Dialer::CallLog::ICallTypeIconsView;

namespace Elastos {
namespace Apps {
namespace Dialer {

/**
 * Encapsulates the views that are used to display the details of a phone call in the call log.
 */
class PhoneCallDetailsViews
    : public Object
    , public IPhoneCallDetailsViews
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Create a new instance by extracting the elements from the given view.
     * <p>
     * The view should contain three text views with identifiers {@code R.id.name},
     * {@code R.id.date}, and {@code R.id.number}, and a linear layout with identifier
     * {@code R.id.call_types}.
     */
    static CARAPI_(AutoPtr<IPhoneCallDetailsViews>) FromView(
        /* [in] */ IView* view);

    static CARAPI_(AutoPtr<IPhoneCallDetailsViews>) CreateForTest(
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
    const AutoPtr<ITextView> mNameView;
    const AutoPtr<IView> mCallTypeView;
    const AutoPtr<ICallTypeIconsView> mCallTypeIcons;
    const AutoPtr<IImageView> mCallAccountIcon;
    const AutoPtr<ITextView> mCallLocationAndDate;
    const AutoPtr<ITextView> mVoicemailTranscriptionView;
};

} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_PHONECALLDETAILSVIEWS_H__
