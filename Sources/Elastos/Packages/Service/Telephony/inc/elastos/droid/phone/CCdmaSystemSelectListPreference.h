#ifndef  __ELASTOS_DROID_PHONE_CCDMASYSTEMSELECTLISTPREFERENCEH__
#define  __ELASTOS_DROID_PHONE_CCDMASYSTEMSELECTLISTPREFERENCEH__

#include "_Elastos_Droid_Phone_CCdmaSystemSelectListPreference.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Phone {

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

    private:
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
    static const String LOG_TAG;
    static const Boolean DBG;

    AutoPtr<IPhone> mPhone;
    AutoPtr<IHandler> mHandler = new MyHandler();
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_CCDMASYSTEMSELECTLISTPREFERENCEH__