
#include "elastos/droid/dialer/PhoneCallDetailsViews.h"
#include "elastos/droid/dialer/calllog/CCallTypeIconsView.h"
#include "R.h"

using Elastos::Droid::Dialer::CallLog::CCallTypeIconsView;
using Elastos::Droid::View::CView;
using Elastos::Droid::Widget::CTextView;
using Elastos::Droid::Widget::CImageView;

namespace Elastos {
namespace Droid {
namespace Dialer {

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

AutoPtr<PhoneCallDetailsViews> PhoneCallDetailsViews::FromView(
    /* [in] */ IView* view)
{
    AutoPtr<IView> nameView;
    view->FindViewById(Elastos::Droid::Dialer::R::id::name, (IView**)&nameView);
    AutoPtr<IView> callTypeView;
    view->FindViewById(Elastos::Droid::Dialer::R::id::call_type, (IView**)&callTypeView);
    AutoPtr<IView> callTypeIcons;
    view->FindViewById(Elastos::Droid::Dialer::R::id::call_type_icons, (IView**)&callTypeIcons);
    AutoPtr<IView> callAccountIcon;
    view->FindViewById(Elastos::Droid::Dialer::R::id::call_account_icon, (IView**)&callAccountIcon);
    AutoPtr<IView> callLocationAndDate;
    view->FindViewById(Elastos::Droid::Dialer::R::id::call_location_and_date, (IView**)&callLocationAndDate);
    AutoPtr<IView> voicemailTranscriptionView;
    view->FindViewById(Elastos::Droid::Dialer::R::id::voicemail_transcription, (IView**)&voicemailTranscriptionView);

    AutoPtr<PhoneCallDetailsViews> views = new PhoneCallDetailsViews(
                ITextView::Probe(nameView),
                callTypeView,
                ICallTypeIconsView::Probe(callTypeIcons),
                IImageView::Probe(callAccountIcon),
                ITextView::Probe(callLocationAndDate),
                ITextView::Probe(voicemailTranscriptionView));
    return views;
}

AutoPtr<PhoneCallDetailsViews> PhoneCallDetailsViews::CreateForTest(
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

    AutoPtr<PhoneCallDetailsViews> views = new PhoneCallDetailsViews(
                nameView, callTypeView, callTypeIcons, callAccountIcon,
                callLocationAndDate, voicemailTranscriptionView);
    return views;
}


} // Dialer
} // Droid
} // Elastos
