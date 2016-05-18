
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_USERINFOCONTROLLER_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_USERINFOCONTROLLER_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/os/AsyncTask.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Core::Object;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

class CUserInfoControllerReceiver;
class CProfileReceiverBroadcastReceiver;

class UserInfoController
    : public Object
    , public IUserInfoController
{
private:
    class UserInfoTask: public AsyncTask
    {
    public:
        UserInfoTask(
            /* [in] */ UserInfoController* host,
            /* [in] */ Int32 userId,
            /* [in] */ Boolean isGuest,
            /* [in] */ const String& userName,
            /* [in] */ Int32 avatarSize,
            /* [in] */ IContext* context);

    protected:
        // @Override
        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [out] */ IInterface** result);

        // @Override
        CARAPI OnPostExecute(
            /* [in] */ IInterface* /*Pair<String, Drawable>*/ result);

    private:
        UserInfoController* mHost;
        Int32 mUserId;
        Boolean mIsGuest;
        String mUserName;
        Int32 mAvatarSize;
        AutoPtr<IContext> mContext;
    };

public:
    CAR_INTERFACE_DECL()

    UserInfoController(
        /* [in] */ IContext* context);

    CARAPI AddListener(
        /* [in] */ IUserInfoControllerOnUserInfoChangedListener* callback);

    CARAPI ReloadUserInfo();

private:
    CARAPI QueryForUserInformation();

    CARAPI_(void) NotifyChanged();

private:
    static const String TAG;

    AutoPtr<IContext> mContext;
    AutoPtr<IArrayList> mCallbacks;  /*<OnUserInfoChangedListener*/
    AutoPtr<AsyncTask> mUserInfoTask;  /*<Void, Void, Pair<String, Drawable>*/

    Boolean mUseDefaultAvatar;
    String mUserName;
    AutoPtr<IDrawable> mUserDrawable;
    AutoPtr<IBroadcastReceiver> mReceiver;
    AutoPtr<IBroadcastReceiver> mProfileReceiver;

    friend class CUserInfoControllerReceiver;
    friend class CProfileReceiverBroadcastReceiver;
};

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_USERINFOCONTROLLER_H__
