
#ifndef __ELASTOS_DROID_PROVIDERS_DOWNLOADS_CDOWNLOADNOTIFIER_H__
#define __ELASTOS_DROID_PROVIDERS_DOWNLOADS_CDOWNLOADNOTIFIER_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Providers_Downloads_CDownloadNotifier.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::INotificationManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Utility::IInt64SparseArray;
using Elastos::Core::ICharSequence;
using Elastos::Utility::ICollection;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Downloads {

/**
 * Update {@link NotificationManager} to reflect current {@link DownloadInfo}
 * states. Collapses similar downloads into a single notification, and builds
 * {@link PendingIntent} that launch towards {@link DownloadReceiver}.
 */
CarClass(CDownloadNotifier)
    , public Object
    , public IDownloadNotifier
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CDownloadNotifier();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI CancelAll();

    /**
     * Notify the current speed of an active download, used for calculating
     * estimated remaining time.
     */
    CARAPI NotifyDownloadSpeed(
        /* [in] */ Int64 id,
        /* [in] */ Int64 bytesPerSecond);

    /**
     * Update {@link NotificationManager} to reflect the given set of
     * {@link DownloadInfo}, adding, collapsing, and removing as needed.
     */
    CARAPI UpdateWith(
        /* [in] */ ICollection* downloads);

    CARAPI DumpSpeeds();

private:
    CARAPI_(void) UpdateWithLocked(
        /* [in] */ ICollection* downloads);

    static CARAPI_(AutoPtr<ICharSequence>) GetDownloadTitle(
        /* [in] */ IResources* res,
        /* [in] */ IDownloadInfo* info);

    CARAPI_(AutoPtr<ArrayOf<Int64> >) GetDownloadIds(
        /* [in] */ ICollection* infos);

    /**
     * Build tag used for collapsing several {@link DownloadInfo} into a single
     * {@link Notification}.
     */
    static CARAPI_(String) BuildNotificationTag(
        /* [in] */ IDownloadInfo* info);

    /**
     * Return the cluster type of the given tag, as created by
     * {@link #buildNotificationTag(DownloadInfo)}.
     */
    static CARAPI_(Int32) GetNotificationTagType(
        /* [in] */ String tag);

    static CARAPI_(Boolean) IsActiveAndVisible(
        /* [in] */ IDownloadInfo* download);

    static CARAPI_(Boolean) IsCompleteAndVisible(
        /* [in] */ IDownloadInfo* download);

    CARAPI_(AutoPtr<IClassInfo>) TransformClassInfo(
        /* [in] */ ClassID objId);

private:
    static Int32 TYPE_ACTIVE;
    static Int32 TYPE_WAITING;
    static Int32 TYPE_COMPLETE;

    AutoPtr<IContext> mContext;
    AutoPtr<INotificationManager> mNotifManager;

    /**
     * Currently active notifications, mapped from clustering tag to timestamp
     * when first shown.
     *
     * @see #buildNotificationTag(DownloadInfo)
     */
    AutoPtr<IHashMap> mActiveNotifs;

    /**
     * Current speed of active downloads, mapped from {@link DownloadInfo#mId}
     * to speed in bytes per second.
     */
    AutoPtr<IInt64SparseArray> mDownloadSpeed;
    /**
     * Last time speed was reproted, mapped from {@link DownloadInfo#mId} to
     * {@link SystemClock#elapsedRealtime()}.
     */
    AutoPtr<IInt64SparseArray> mDownloadTouch;
};

} // namespace Downloads
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_PROVIDERS_DOWNLOADS_CDOWNLOADNOTIFIER_H__
