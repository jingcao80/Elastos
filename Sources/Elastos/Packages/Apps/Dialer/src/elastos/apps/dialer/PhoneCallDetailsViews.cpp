
#include "PhoneCallDetailsViews.h"

namespace Elastos{
namespace Apps{
namespace Dialer{

CAR_INTERFACE_IMPL(PhoneCallDetailsViews, Object, IPhoneCallDetailsViews)

PhoneCallDetailsViews::PhoneCallDetailsViews(
    /* [in] */ ITextView* nameView,
    /* [in] */ IView* callTypeView,
    /* [in] */ ICallTypeIconsView* callTypeIcons,
    /* [in] */ IImageView* callAccountIcon,
    /* [in] */ ITextView* callLocationAndDate,
    /* [in] */ ITextView* voicemailTranscriptionView)
    : mNameView(nameView)
    , mCallTypeView(callTypeView)
    , mCallTypeIcons(callTypeIcons)
    , mCallAccountIcon(callAccountIcon)
    , mCallLocationAndDate(callLocationAndDate)
    , mVoicemailTranscriptionView(voicemailTranscriptionView)
{}

AutoPtr<IPhoneCallDetailsViews> PhoneCallDetailsViews::FromView(
    /* [in] */ IView* view)
{
    AutoPtr<IView> nameView;
    view->FindViewById(R::id::name, (IView**)&nameView);
    AutoPtr<IView> callTypeView;
    view->FindViewById(R::id::call_type, (IView**)&callTypeView);
    AutoPtr<IView> callTypeIcons;
    view->FindViewById(R::id::call_type_icons, (IView**)&callTypeIcons);
    AutoPtr<IView> callAccountIcon;
    view->FindViewById(R::id::call_account_icon, (IView**)&callAccountIcon);
    AutoPtr<IView> callLocationAndDate;
    view->FindViewById(R::id::call_location_and_date, (IView**)&callLocationAndDate);
    AutoPtr<IView> voicemailTranscriptionView;
    view->FindViewById(R::id::voicemail_transcription, (IView**)&voicemailTranscriptionView);

    AutoPtr<PhoneCallDetailsViews> views = new PhoneCallDetailsViews(
                ITextView::Probe(nameView), callTypeView,
                ICallTypeIconsView::Probe(callTypeIcons),
                IImageView::Probe(callAccountIcon),
                ITextView::Probe(callLocationAndDate),
                ITextView::Probe(voicemailTranscriptionView));
    return (IPhoneCallDetailsViews*)views;
}

AutoPtr<IPhoneCallDetailsViews> PhoneCallDetailsViews::CreateForTest(
    /* [in] */ IContext* context)
{
    AutoPtr<ITextView> nameView;
    CTextView::New(context, (ITextView**)&nameView);
    AutoPtr<IView> callTypeView;
    CView::New(context, (IView**)&callTypeView);
    AutoPtr<ICallTypeIconsView> callTypeIcons;
    CCallTypeIconsView::New(context, (ICallTypeIconsView**)&callTypeIcons);
    AutoPtr<IImageView> callAccountIcon;
    CImageView::New(context, (IImageView**)&callAccountIcon);
    AutoPtr<ITextView> callLocationAndDate;
    CTextView::New(context, (ITextView**)&callLocationAndDate);
    AutoPtr<ITextView> voicemailTranscriptionView;
    CTextView::New(context, (ITextView**)&voicemailTranscriptionView);

    AutoPtr<PhoneCallDetailsViews> views = PhoneCallDetailsViews(
                nameView, callTypeView, callTypeIcons, callAccountIcon,
                callLocationAndDate, voicemailTranscriptionView);
    return (IPhoneCallDetailsViews*)views;
}


} // Dialer
} // Apps
} // Elastos
