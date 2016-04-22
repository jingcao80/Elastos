#ifndef __ELASTOS_DROID_SERVER_CLIPBOARD_CLIPBOARDSERVICE_H__
#define __ELASTOS_DROID_SERVER_CLIPBOARD_CLIPBOARDSERVICE_H__

#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Utility.h>
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IClipData;
using Elastos::Droid::Content::IClipDataItem;
using Elastos::Droid::Content::IClipDescription;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIClipboard;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::IOnPrimaryClipChangedListener;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IIUserManager;
using Elastos::Droid::Os::IRemoteCallbackList;
using Elastos::Droid::Services::SecurityBridge::Api::IClipboardManagerMonitor;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Utility::IHashSet;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Clipboard {

class ClipboardService
    : public Object
    , public IIClipboard
    , public IBinder
{
private:
    class ListenerInfo
        : public Object
    {
    public:
        ListenerInfo(
            /* [in] */ Int32 uid,
            /* [in] */ const String& packageName);

        ~ListenerInfo();

    public:
        Int32 mUid;
        String mPackageName;
    };

    class PerUserClipboard
        : public Object
    {
    public:
        PerUserClipboard(
            /* [in] */ Int32 userId);

        ~PerUserClipboard();

    public:
        Int32 mUserId;

        AutoPtr<IRemoteCallbackList> mPrimaryClipListeners;

        AutoPtr<IClipData> mPrimaryClip;

        // final HashSet<String> activePermissionOwners = new HashSet<String>();
        AutoPtr<IHashSet> mActivePermissionOwners;
    };

    class MyBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        MyBroadcastReceiver(
            /* [in] */ ClipboardService* host);

        ~MyBroadcastReceiver();

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = "ClipboardService::MyBroadcastReceiver: ";
            info->AppendFormat("%p", this);
            return NOERROR;
        }

    private:
        ClipboardService* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    ClipboardService();

    ~ClipboardService();

    /**
     * Instantiates the clipboard.
     */
    CARAPI constructor(
        /* [in] */ IContext *context);

    // @Override
    // public boolean onTransact(int code, Parcel data, Parcel reply, int flags)
    //         throws RemoteException {
    //     try {
    //         return super.onTransact(code, data, reply, flags);
    //     } catch (RuntimeException e) {
    //         if (!(e instanceof SecurityException)) {
    //             Slog.wtf("clipboard", "Exception: ", e);
    //         }
    //         throw e;
    //     }

    // }

    CARAPI SetPrimaryClip(
        /* [in] */ IClipData* clip,
        /* [in] */ const String& callingPackage);

    CARAPI GetPrimaryClip(
        /* [in] */ const String& pkg,
        /* [out] */ IClipData** clip);

    CARAPI GetPrimaryClipDescription(
        /* [in] */ const String& callingPackage,
        /* [out] */ IClipDescription** description);

    CARAPI HasPrimaryClip(
        /* [in] */ const String& callingPackage,
        /* [out] */ Boolean* hasPrimaryClip);

    CARAPI AddPrimaryClipChangedListener(
        /* [in] */ IOnPrimaryClipChangedListener* listener,
        /* [in] */ const String& callingPackage);

    CARAPI RemovePrimaryClipChangedListener(
        /* [in] */ IOnPrimaryClipChangedListener* listener);

    CARAPI HasClipboardText(
        /* [in] */ const String& callingPackage,
        /* [out] */ Boolean* hasClipboardText);

    // @Override
    CARAPI ToString(
        /* [out] */ String* info)
    {
        VALIDATE_NOT_NULL(info);
        *info = "ClipboardService:: ";
        info->AppendFormat("%p", this);
        return NOERROR;
    }

protected:
    CARAPI GetRelatedProfiles(
        /* [in] */ Int32 userId,
        /* [out] */ IList** userInfo);

    CARAPI SetPrimaryClipInternal(
        /* [in] */ PerUserClipboard* clipboard,
        /* [in] */ IClipData* clip);

private:
    CARAPI_(AutoPtr<PerUserClipboard>) GetClipboard();

    CARAPI_(AutoPtr<PerUserClipboard>) GetClipboard(
        /* [in] */ Int32 userId);

    CARAPI_(void) RemoveClipboard(
        /* [in] */ Int32 userId);

    CARAPI CheckUriOwnerLocked(
        /* [in] */ IUri* uri,
        /* [in] */ Int32 uid);

    CARAPI CheckItemOwnerLocked(
        /* [in] */ IClipDataItem* item,
        /* [in] */ Int32 uid);

    CARAPI CheckDataOwnerLocked(
        /* [in] */ IClipData* data,
        /* [in] */ Int32 uid);

    CARAPI_(void) GrantUriLocked(
        /* [in] */ IUri* uri,
        /* [in] */ const String& pkg,
        /* [in] */ Int32 userId);

    CARAPI_(void) GrantItemLocked(
        /* [in] */ IClipDataItem* item,
        /* [in] */ const String& pkg,
        /* [in] */ Int32 userId);

    CARAPI AddActiveOwnerLocked(
        /* [in] */ Int32 uid,
        /* [in] */ const String& pkg);

    CARAPI_(void) RevokeUriLocked(
        /* [in] */ IUri* uri);

    CARAPI_(void) RevokeItemLocked(
        /* [in] */ IClipDataItem* item);

    CARAPI_(void) RevokeUris(
        /* [in] */ PerUserClipboard* clipboard);

private:
    static const String TAG;
    AutoPtr<IContext> mContext;
    AutoPtr<IIActivityManager> mAm;
    AutoPtr<IIUserManager> mUm;
    AutoPtr<IPackageManager> mPm;
    AutoPtr<IAppOpsManager> mAppOps;
    AutoPtr<IBinder> mPermissionOwner;

    static const String SECURITY_BRIDGE_NAME;
    AutoPtr<IClipboardManagerMonitor> mSecurityBridge;
    AutoPtr<ISparseArray> mClipboards;
};

}//namespace Clipboard
}//namespace Server
}//namesapce Droid
}//namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CLIPBOARD_CLIPBOARDSERVICE_H__
