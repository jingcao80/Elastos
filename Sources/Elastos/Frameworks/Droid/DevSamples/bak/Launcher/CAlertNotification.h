
#ifndef __CALERTNOTIFICATION_H__
#define __CALERTNOTIFICATION_H__

#include "_CAlertNotification.h"
#include <elautoptr.h>

using Elastos::Core::ICharSequence;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::INotificationAction;
using Elastos::Droid::App::INotificationManager;
using Elastos::Droid::Widget::IRemoteViews;
using Elastos::Droid::Graphics::IBitmap;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Launcher {

CarClass(CAlertNotification)
{
public:
    ~CAlertNotification();

    CARAPI IsProgressStyle(
        /* [out] */ Boolean* pIsProgressStyle);

    CARAPI Show();

    CARAPI SetCustomIcon(
        /* [in] */ IUri* aIconUri);

    CARAPI UpdateProgress(
        /* [in] */ const String& aAlertText,
        /* [in] */ Int32 aProgress,
        /* [in] */ Int32 aProgressMax);

public:
    CARAPI constructor(
        /* [in] */ IContext* pContext,
        /* [in] */ Int32 aNotificationId,
        /* [in] */ Int32 aIcon,
        /* [in] */ const String& aTitle,
        /* [in] */ const String& aText,
        /* [in] */ Int64 when);

    CARAPI GetWhen(
        /* [out] */ Int64 * pWhen);

    CARAPI SetWhen(
        /* [in] */ Int64 when);

    CARAPI GetIcon(
        /* [out] */ Int32 * pIcon);

    CARAPI SetIcon(
        /* [in] */ Int32 icon);

    CARAPI GetIconLevel(
        /* [out] */ Int32* iconLevel);

    CARAPI SetIconLevel(
        /* [in] */ Int32 iconLevel);

    CARAPI GetNumber(
        /* [out] */ Int32* number);

    CARAPI SetNumber(
        /* [in] */ Int32 number);

    CARAPI GetContentIntent(
        /* [out] */ IPendingIntent ** ppIntent);

    CARAPI SetContentIntent(
        /* [in] */ IPendingIntent * pIntent);

    CARAPI GetDeleteIntent(
        /* [out] */ IPendingIntent ** ppIntent);

    CARAPI SetDeleteIntent(
        /* [in] */ IPendingIntent * pIntent);

    CARAPI GetFullScreenIntent(
        /* [out] */ IPendingIntent** intent);

    CARAPI SetFullScreenIntent(
        /* [in] */ IPendingIntent* intent);

    CARAPI GetTickerText(
        /* [out] */ ICharSequence ** ppText);

    CARAPI SetTickerText(
        /* [in] */ ICharSequence * pText);

    CARAPI GetTickerView(
        /* [out] */ IRemoteViews** view);

    CARAPI SetTickerView(
        /* [in] */ IRemoteViews* view);

    CARAPI GetBigContentView(
        /* [out] */ IRemoteViews** view);

    CARAPI SetBigContentView(
        /* [in] */ IRemoteViews* view);

    CARAPI GetLargeIcon(
        /* [out] */ IBitmap** bitmap);

    CARAPI SetLargeIcon(
        /* [in] */ IBitmap* bitmap);


    CARAPI GetContentView(
        /* [out] */ IRemoteViews ** ppView);

    CARAPI SetContentView(
        /* [in] */ IRemoteViews * pView);

    CARAPI GetSound(
        /* [out] */ IUri ** ppSound);

    CARAPI SetSound(
        /* [in] */ IUri * pSound);

    CARAPI GetAudioStreamType(
        /* [out] */ Int32 * pType);

    CARAPI SetAudioStreamType(
        /* [in] */ Int32 type);

    CARAPI GetVibrate(
        /* [out, callee] */ ArrayOf<Int64> ** ppVibrate);

    CARAPI SetVibrate(
        /* [in] */ ArrayOf<Int64> * pVibrate);

    CARAPI GetLedARGB(
        /* [out] */ Int32 * pArgb);

    CARAPI SetLedARGB(
        /* [in] */ Int32 argb);

    CARAPI GetLedOnMS(
        /* [out] */ Int32 * pOnMS);

    CARAPI SetLedOnMS(
        /* [in] */ Int32 onMS);

    CARAPI GetLedOffMS(
        /* [out] */ Int32 * pOffMS);

    CARAPI SetLedOffMS(
        /* [in] */ Int32 offMS);

    CARAPI GetDefaults(
        /* [out] */ Int32 * pDefaults);

    CARAPI SetDefaults(
        /* [in] */ Int32 defaults);

    CARAPI GetFlags(
        /* [out] */ Int32 * pFlags);

    CARAPI SetFlags(
        /* [in] */ Int32 flags);

    CARAPI GetPriority(
        /* [out] */ Int32* priority);

    CARAPI SetPriority(
        /* [in] */ Int32 priority);

    CARAPI GetKind(
        /* [out, callee] */ ArrayOf<String>** kind);

    CARAPI SetKind(
        /* [in] */ ArrayOf<String>* kind);

    CARAPI GetExtras(
        /* [out] */ IBundle** extras);

    CARAPI SetExtras(
        /* [in] */ IBundle* extras);

    CARAPI GetActions(
        /* [out, callee] */ ArrayOf<INotificationAction*>** actions);

    CARAPI SetActions(
        /* [in] */ ArrayOf<INotificationAction*>* actions);

    CARAPI SetLatestEventInfo(
        /* [in] */ IContext* context,
        /* [in] */ ICharSequence* contentTitle,
        /* [in] */ ICharSequence* contentText,
        /* [in] */ IPendingIntent* contentIntent);

    CARAPI SetUser(
        /* [in] */ IUserHandle* user);

private:
    static const Double UPDATE_THRESHOLD;

    AutoPtr<INotification> mNotification;
    AutoPtr<IContext> mContext;
    Int32 mId;
    Int32 mIcon;
    String mTitle;
    String mText;
    Boolean mProgressStyle;
    AutoPtr<INotificationManager> mNotificationManager;
    Double mPrevPercent;
    String mPrevAlertText;
};

} // namespace Launcher
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CALERTNOTIFICATION_H__

