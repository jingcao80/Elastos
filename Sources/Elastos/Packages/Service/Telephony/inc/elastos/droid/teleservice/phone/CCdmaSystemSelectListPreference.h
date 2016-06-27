#ifndef  __ELASTOS_DROID_PHONE_CCDMASYSTEMSELECTLISTPREFERENCEH__
#define  __ELASTOS_DROID_PHONE_CCDMASYSTEMSELECTLISTPREFERENCEH__

#include "_Elastos_Droid_TeleService_Phone_CCdmaSystemSelectListPreference.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/preference/ListPreference.h"
#include "Elastos.Droid.Internal.h"

using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Preference::ListPreference;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

class CCdmaOptions;

CarClass(CCdmaSystemSelectListPreference)
    , public ListPreference
    , public ICdmaSystemSelectListPreference
{
private:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CCdmaSystemSelectListPreference::MyHandler")

        MyHandler(
            /* [in] */ CCdmaSystemSelectListPreference* mHost);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CARAPI_(void) HandleQueryCdmaRoamingPreference(
            /* [in] */ IMessage* msg);

        CARAPI_(void) HandleSetCdmaRoamingPreference(
            /* [in] */ IMessage* msg);

        CARAPI_(void) ResetCdmaRoamingModeToDefault();

    public:
        static const Int32 MESSAGE_GET_ROAMING_PREFERENCE;
        static const Int32 MESSAGE_SET_ROAMING_PREFERENCE;

        CCdmaSystemSelectListPreference* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL();

    CCdmaSystemSelectListPreference();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

protected:
    //@Override
    CARAPI ShowDialog(
        /* [in] */ IBundle* state);

    //@Override
    CARAPI OnDialogClosed(
        /* [in] */ Boolean positiveResult);

private:
    static const String TAG;
    static const Boolean DBG;

    AutoPtr<IPhone> mPhone;
    AutoPtr<IHandler> mHandler;
    friend class CCdmaOptions;
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CCDMASYSTEMSELECTLISTPREFERENCEH__
