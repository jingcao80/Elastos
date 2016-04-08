
#include "CAlertNotification.h"
#include "GeckoApp.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Math.h>
#include <Elastos.CoreLibrary.h>

using Elastos::Net::IURL;
using Elastos::Net::CURL;
using Elastos::Core::CStringWrapper;
using Elastos::Droid::App::CNotification;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Graphics::CBitmapFactory;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Launcher {

const Double CAlertNotification::UPDATE_THRESHOLD = .01;

CAlertNotification::~CAlertNotification()
{
}

ECode CAlertNotification::constructor(
    /* [in] */ IContext* pContext,
    /* [in] */ Int32 aNotificationId,
    /* [in] */ Int32 aIcon,
    /* [in] */ const String& aTitle,
    /* [in] */ const String& aText,
    /* [in] */ Int64 when)
{
    AutoPtr<ICharSequence> aTitleSeq;
    ECode ec = CStringWrapper::New(aTitle, (ICharSequence**)&(aTitleSeq));
    if (FAILED(ec)) return ec;

    AutoPtr<ICharSequence> aTextSeq;
    ec = CStringWrapper::New(aText, (ICharSequence**)&(aTextSeq));
    if (FAILED(ec)) return ec;

    ec = CNotification::New(aIcon,
        (aText.GetLength() > 0) ? aTextSeq : aTitleSeq, when, (INotification**)&mNotification);
    if (FAILED(ec)) return ec;

    mContext = pContext;

    mIcon = aIcon;
    mTitle = aTitle;
    mText = aText;
    mProgressStyle = FALSE;
    mId = aNotificationId;

    mPrevPercent = -1;
    mPrevAlertText = "";

    return mContext->GetSystemService(
            IContext::NOTIFICATION_SERVICE, (IInterface**)&mNotificationManager);
}

ECode CAlertNotification::IsProgressStyle(
    /* [out] */ Boolean* pIsProgressStyle)
{
    if (!pIsProgressStyle) return E_INVALID_ARGUMENT;
    *pIsProgressStyle = mProgressStyle;
    return NOERROR;
}

ECode CAlertNotification::Show()
{
    return mNotificationManager->Notify(mId, this);
}

ECode CAlertNotification::SetCustomIcon(
    /* [in] */ IUri* aIconUri)
{
    String scheme;
    aIconUri->GetScheme(&scheme);
    if (aIconUri == NULL || scheme.IsNull())
        return E_INVALID_ARGUMENT;
    // Custom view
    Int32 layout = 0x7f030001;//R.layout.notification_icon_text;
    String packageName;
    GeckoApp::sAppContext->GetPackageName(&packageName);
    AutoPtr<IRemoteViews> pView;
    ECode ec;
//    ec = CRemoteViews::New(packageName, layout, (IRemoteViews**)&pView);
//    if (FAILED(ec)) return ec;

    //try {
    String iconUriString;
    aIconUri->ToString(&iconUriString);
    AutoPtr<IURL> pUrl;
    ec = CURL::New(iconUriString, (IURL**)&pUrl);
    if (FAILED(ec)) return ec;
    AutoPtr<IInputStream> inStream;
    ec = pUrl->OpenStream((IInputStream**)&inStream);
    AutoPtr<IBitmapFactory> bitmapFactory;
    ec = CBitmapFactory::AcquireSingleton((IBitmapFactory**)&bitmapFactory);
    if (FAILED(ec)) return ec;
    AutoPtr<IBitmap> pBm;
    ec = bitmapFactory->DecodeStream(inStream, (IBitmap**)&pBm);
    if (FAILED(ec)) return ec;

    ec = pView->SetImageViewBitmap(0x7f070004, pBm);//R.id.notificationImage
    if (FAILED(ec)) return ec;

    AutoPtr<ICharSequence> mTitleSeq;
    FAIL_RETURN(CStringWrapper::New(mTitle, (ICharSequence**)&(mTitleSeq)));
    ec = pView->SetTextViewText(0x7f070005, (ICharSequence*)mTitleSeq); //R.id.notificationTitle
    if (FAILED(ec)) return ec;

    if (mText.GetLength() > 0) {
        ec = pView->SetTextViewText(0x7f070006, mTitleSeq);//R.id.notificationText
        if (FAILED(ec)) return ec;
    }
    ec = SetContentView(pView);
    if (FAILED(ec)) return ec;

    ec = mNotificationManager->Notify(mId, this);
    //} catch(Exception ex) {
    //    Log.e("GeckoAlert", "failed to create bitmap", ex);
    //}

    return ec;
}

ECode CAlertNotification::UpdateProgress(
    /* [in] */ const String& aAlertText,
    /* [in] */ Int32 aProgress,
    /* [in] */ Int32 aProgressMax)
{
    ECode ec;
    if (!mProgressStyle) {
        // Custom view
        Int32 layout = aAlertText.GetLength() > 0 ?
                0x7f030003 : 0x7f030002; //R.layout.notification_progress_text : R.layout.notification_progress;

        String packageName;
        GeckoApp::sAppContext->GetPackageName(&packageName);
        AutoPtr<IRemoteViews> pView;
//        ec = CRemoteViews::New(packageName, layout, (IRemoteViews**)&pView);
//        if (FAILED(ec)) return ec;

        AutoPtr<ICharSequence> mTitleSeq;
        ec = CStringWrapper::New(mTitle, (ICharSequence**)&(mTitleSeq));
        if (FAILED(ec)) return ec;

        pView->SetImageViewResource(0x7f070004, mIcon); //R.id.notificationImage
        pView->SetTextViewText(0x7f070005, mTitleSeq);//R.id.notificationTitle

        SetContentView(pView);

        Int32 flags;
        mNotification->GetFlags(&flags);
        flags |= INotification::FLAG_ONGOING_EVENT | INotification::FLAG_ONLY_ALERT_ONCE;
        mNotification->SetFlags(flags);

        mProgressStyle = TRUE;
    }

    String text;
    Double percent = 0;
    if (aProgressMax > 0) {
        percent = ((Double)aProgress / (Double)aProgressMax);
    }

    if (aAlertText.GetLength() > 0) {
        text = aAlertText;
    }
    else {
        assert(0);
        /*AutoPtr<INumberFormat> pFormat;
        ec = NumberFormat::GetPercentInstance((INumberFormat**)&pFormat);
        if (FAILED(ec)) return ec;
        ec = pFormat->Format(percent, &text);
        if (FAILED(ec)) return ec;*/
    }

    if (mPrevAlertText.Equals(text)
        && Math::Abs(mPrevPercent - percent) < UPDATE_THRESHOLD) {
        return NOERROR;
    }

    AutoPtr<IRemoteViews> pIRView;
    ec = GetContentView((IRemoteViews**)&pIRView);
    if (FAILED(ec)) return ec;

    AutoPtr<ICharSequence> textSeq;
    ec = CStringWrapper::New(text, (ICharSequence**)&(textSeq));
    if (FAILED(ec)) return ec;

    ec = pIRView->SetTextViewText(0x7f070006, textSeq);//R.id.notificationText
    if (FAILED(ec)) return ec;

    ec = pIRView->SetProgressBar(0x7f070007, //R.id.notificationProgressbar,
                     (Int32)aProgressMax, (Int32)aProgress, FALSE);
    if (FAILED(ec)) return ec;

    // Update the notification
    ec = mNotificationManager->Notify(mId, this);
    if (FAILED(ec)) return ec;

    mPrevPercent = percent;
    mPrevAlertText = text;
    return NOERROR;
}

ECode CAlertNotification::GetWhen(
    /* [out] */ Int64 * pWhen)
{
    return mNotification->GetWhen(pWhen);
}

ECode CAlertNotification::SetWhen(
    /* [in] */ Int64 when)
{
    return mNotification->SetWhen(when);
}

ECode CAlertNotification::GetIcon(
    /* [out] */ Int32 * pIcon)
{
    return mNotification->GetIcon(pIcon);
}

ECode CAlertNotification::SetIcon(
    /* [in] */ Int32 icon)
{
    return mNotification->SetIcon(icon);
}

ECode CAlertNotification::GetIconLevel(
    /* [out] */ Int32* iconLevel)
{
    return mNotification->GetIconLevel(iconLevel);
}

ECode CAlertNotification::SetIconLevel(
    /* [in] */ Int32 iconLevel)
{
    return mNotification->SetIconLevel(iconLevel);
}

ECode CAlertNotification::GetNumber(
    /* [out] */ Int32* number)
{
    return mNotification->GetNumber(number);
}

ECode CAlertNotification::SetNumber(
    /* [in] */ Int32 number)
{
    return mNotification->SetNumber(number);
}

ECode CAlertNotification::GetContentIntent(
    /* [out] */ IPendingIntent ** ppIntent)
{
    return mNotification->GetContentIntent(ppIntent);
}

ECode CAlertNotification::SetContentIntent(
    /* [in] */ IPendingIntent * pIntent)
{
    return mNotification->SetContentIntent(pIntent);
}

ECode CAlertNotification::GetDeleteIntent(
    /* [out] */ IPendingIntent ** ppIntent)
{
    return mNotification->GetDeleteIntent(ppIntent);
}

ECode CAlertNotification::SetDeleteIntent(
    /* [in] */ IPendingIntent * pIntent)
{
    return mNotification->SetDeleteIntent(pIntent);
}

ECode CAlertNotification::GetFullScreenIntent(
    /* [out] */ IPendingIntent** intent)
{
    return mNotification->GetFullScreenIntent(intent);
}

ECode CAlertNotification::SetFullScreenIntent(
    /* [in] */ IPendingIntent* intent)
{
    return mNotification->SetFullScreenIntent(intent);
}

ECode CAlertNotification::GetTickerText(
    /* [out] */ ICharSequence ** ppText)
{
    return mNotification->GetTickerText(ppText);
}

ECode CAlertNotification::SetTickerText(
    /* [in] */ ICharSequence * pText)
{
    return mNotification->SetTickerText(pText);
}

ECode CAlertNotification::GetTickerView(
    /* [out] */ IRemoteViews** view)
{
    return mNotification->GetTickerView(view);
}

ECode CAlertNotification::SetTickerView(
    /* [in] */ IRemoteViews* view)
{
    return mNotification->SetTickerView(view);
}

ECode CAlertNotification::GetBigContentView(
    /* [out] */ IRemoteViews** view)
{
    return mNotification->GetBigContentView(view);
}

ECode CAlertNotification::SetBigContentView(
    /* [in] */ IRemoteViews* view)
{
    return mNotification->SetBigContentView(view);
}

ECode CAlertNotification::GetLargeIcon(
    /* [out] */ IBitmap** bitmap)
{
    return mNotification->GetLargeIcon(bitmap);
}

ECode CAlertNotification::SetLargeIcon(
    /* [in] */ IBitmap* bitmap)
{
    return mNotification->SetLargeIcon(bitmap);
}

ECode CAlertNotification::GetContentView(
    /* [out] */ IRemoteViews ** ppView)
{
    return mNotification->GetContentView(ppView);
}

ECode CAlertNotification::SetContentView(
    /* [in] */ IRemoteViews * pView)
{
    return mNotification->SetContentView(pView);
}

ECode CAlertNotification::GetSound(
    /* [out] */ IUri ** ppSound)
{
    return mNotification->GetSound(ppSound);
}

ECode CAlertNotification::SetSound(
    /* [in] */ IUri * pSound)
{
    return mNotification->SetSound(pSound);
}

ECode CAlertNotification::GetAudioStreamType(
    /* [out] */ Int32 * pType)
{
    return mNotification->GetAudioStreamType(pType);
}

ECode CAlertNotification::SetAudioStreamType(
    /* [in] */ Int32 type)
{
    return mNotification->SetAudioStreamType(type);
}

ECode CAlertNotification::GetVibrate(
    /* [out, callee] */ ArrayOf<Int64> ** ppVibrate)
{
    return mNotification->GetVibrate(ppVibrate);
}

ECode CAlertNotification::SetVibrate(
    /* [in] */ ArrayOf<Int64> * pVibrate)
{
    return mNotification->SetVibrate(pVibrate);
}

ECode CAlertNotification::GetLedARGB(
    /* [out] */ Int32 * pArgb)
{
    return mNotification->GetLedARGB(pArgb);
}

ECode CAlertNotification::SetLedARGB(
    /* [in] */ Int32 argb)
{
    return mNotification->SetLedARGB(argb);
}

ECode CAlertNotification::GetLedOnMS(
    /* [out] */ Int32 * pOnMS)
{
    return mNotification->GetLedOnMS(pOnMS);
}

ECode CAlertNotification::SetLedOnMS(
    /* [in] */ Int32 onMS)
{
    return mNotification->SetLedOnMS(onMS);
}

ECode CAlertNotification::GetLedOffMS(
    /* [out] */ Int32 * pOffMS)
{
    return mNotification->GetLedOffMS(pOffMS);
}

ECode CAlertNotification::SetLedOffMS(
    /* [in] */ Int32 offMS)
{
    return mNotification->SetLedOffMS(offMS);
}

ECode CAlertNotification::GetDefaults(
    /* [out] */ Int32 * pDefaults)
{
    return mNotification->GetDefaults(pDefaults);
}

ECode CAlertNotification::SetDefaults(
    /* [in] */ Int32 defaults)
{
    return mNotification->SetDefaults(defaults);
}

ECode CAlertNotification::GetFlags(
    /* [out] */ Int32 * pFlags)
{
    return mNotification->GetFlags(pFlags);
}

ECode CAlertNotification::SetFlags(
    /* [in] */ Int32 flags)
{
    return mNotification->SetFlags(flags);
}

ECode CAlertNotification::GetPriority(
    /* [out] */ Int32* priority)
{
    return mNotification->GetPriority(priority);
}

ECode CAlertNotification::SetPriority(
    /* [in] */ Int32 priority)
{
    return mNotification->SetPriority(priority);
}

ECode CAlertNotification::GetKind(
    /* [out, callee] */ ArrayOf<String>** kind)
{
    return mNotification->GetKind(kind);
}

ECode CAlertNotification::SetKind(
    /* [in] */ ArrayOf<String>* kind)
{
    return mNotification->SetKind(kind);
}

ECode CAlertNotification::GetExtras(
    /* [out] */ IBundle** extras)
{
    return mNotification->GetExtras(extras);
}

ECode CAlertNotification::SetExtras(
    /* [in] */ IBundle* extras)
{
    return mNotification->SetExtras(extras);
}

ECode CAlertNotification::GetActions(
    /* [out, callee] */ ArrayOf<INotificationAction*>** actions)
{
    return mNotification->GetActions(actions);
}

ECode CAlertNotification::SetActions(
    /* [in] */ ArrayOf<INotificationAction*>* actions)
{
    return mNotification->SetActions(actions);
}

ECode CAlertNotification::SetLatestEventInfo(
/* [in] */ IContext* context,
/* [in] */ ICharSequence* contentTitle,
/* [in] */ ICharSequence* contentText,
/* [in] */ IPendingIntent* contentIntent)
{
return mNotification->SetLatestEventInfo(context, contentTitle, contentText, contentIntent);
}

ECode CAlertNotification::SetUser(
    /* [in] */ IUserHandle* user)
{
    return mNotification->SetUser(user);
}

} // namespace Launcher
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
