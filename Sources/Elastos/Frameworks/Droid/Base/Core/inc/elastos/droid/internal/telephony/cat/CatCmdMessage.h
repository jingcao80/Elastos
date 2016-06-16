#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CATCMDMESSAGE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CATCMDMESSAGE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"
#include "elastos/droid/internal/telephony/cat/CommandParams.h"
#include "elastos/droid/internal/telephony/cat/CommandDetails.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

/**
  * Class used to pass CAT messages from telephony to application. Application
  * should call getXXX() to get commands's specific values.
  *
  */
class CatCmdMessage
    : public Object
    , public ICatCmdMessage
    , public IParcelable
{
public:
    /*
      * Container for Launch Browser command settings.
      */
    class BrowserSettings
        : public Object
        , public IBrowserSettings
    {
    public:
        CAR_INTERFACE_DECL();

        CARAPI SetUrl(
            /* [in] */ const String& url);

        CARAPI GetUrl(
            /* [out] */ String* url);

        CARAPI SetMode(
            /* [in] */ LaunchBrowserMode mode);

        CARAPI GetMode(
            /* [out] */ LaunchBrowserMode* mode);

    private:
        String url;
        LaunchBrowserMode mode;
    };

    /*
      * Container for Call Setup command settings.
      */
    class CallSettings
        : public Object
        , public ICallSettings
    {
    public:
        CAR_INTERFACE_DECL();

        CARAPI SetConfirmMsg(
            /* [in] */ ITextMessage* confirmMsg);

        CARAPI GetConfirmMsg(
            /* [out] */ ITextMessage** result);

        CARAPI SetCallMsg(
            /* [in] */ ITextMessage* callMsg);

        CARAPI GetCallMsg(
            /* [out] */ ITextMessage** result);

    private:
        AutoPtr<ITextMessage> confirmMsg;
        AutoPtr<ITextMessage> callMsg;
    };

    class SetupEventListSettings
        : public Object
        , public ISetupEventListSettings
    {
    public:
        CAR_INTERFACE_DECL();

        CARAPI SetEventList(
            /* [in] */ ArrayOf<Int32>* eventList);

        CARAPI GetEventList(
            /* [out] */ ArrayOf<Int32>** eventList);

    private:
        AutoPtr<ArrayOf<Int32> > eventList;
    };

    //class SetupEventListConstants
    //{
    //public:
    //    // Event values in SETUP_EVENT_LIST Proactive Command as per ETSI 102.223
    //    static const Int32 USER_ACTIVITY_EVENT = 0x04;
    //    static const Int32 IDLE_SCREEN_AVAILABLE_EVENT = 0x05;
    //    static const Int32 LANGUAGE_SELECTION_EVENT = 0x07;
    //    static const Int32 BROWSER_TERMINATION_EVENT = 0x08;
    //    static const Int32 BROWSING_STATUS_EVENT = 0x0F;
    //    static const Int32 HCI_CONNECTIVITY_EVENT = 0x13;
    //};

    //class BrowserTerminationCauses
    //{
    //public:
    //    static const Int32 USER_TERMINATION = 0x00;
    //    static const Int32 ERROR_TERMINATION = 0x01;
    //};

//private:
    //class InnerParcelableCreator
    //    : public Object
    //    , public Parcelable::Creator<CatCmdMessage>
    //{
    //public:
    //    InnerParcelableCreator(
    //        /* [in] */ CatCmdMessage* owner);

    //    // @Override
    //    CARAPI CreateFromParcel(
    //        /* [in] */ IParcel* in,
    //        /* [out] */ CatCmdMessage** result);

    //    // @Override
    //    CARAPI NewArray(
    //        /* [in] */ Int32 size,
    //        /* [out] */ CatCmdMessage[]** result);

    //private:
    //    CatCmdMessage* mOwner;
    //};

public:
    CAR_INTERFACE_DECL();

    CatCmdMessage();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IInterface/*CommandParams*/* cmdParams);

    CARAPI constructor(
        /* [in] */ IParcel* in);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);
        ///* [in] */ Int32 flags);

    // @Override
    //CARAPI DescribeContents(
    //    /* [out] */ Int32* result);

    /* external API to be used by application */
    virtual CARAPI GetCmdType(
        /* [out] */ CommandType* result);

    virtual CARAPI GetMenu(
        /* [out] */ IMenu** result);

    virtual CARAPI GeInput(
        /* [out] */ IInput** result);

    virtual CARAPI GeTextMessage(
        /* [out] */ ITextMessage** result);

    virtual CARAPI GetBrowserSettings(
        /* [out] */ IBrowserSettings** result);

    virtual CARAPI GetToneSettings(
        /* [out] */ IToneSettings** result);

    virtual CARAPI GetCallSettings(
        /* [out] */ ICallSettings** result);

    /**
      * API to be used by application to check if loading optional icon
      * has failed
      */
    virtual CARAPI HasIconLoadFailed(
        /* [out] */ Boolean* result);

    virtual CARAPI IsRefreshResetOrInit(
        /* [out] */ Boolean* result);

    virtual CARAPI GetSetEventList(
        /* [out] */ ISetupEventListSettings** result);

public:
    // members
    AutoPtr<CommandDetails> mCmdDet;
    // Command Qualifier values for refresh command
    static const Int32 REFRESH_NAA_INIT_AND_FULL_FILE_CHANGE = 0x00;
    static const Int32 REFRESH_NAA_INIT_AND_FILE_CHANGE = 0x02;
    static const Int32 REFRESH_NAA_INIT = 0x03;
    static const Int32 REFRESH_UICC_RESET = 0x04;
    //static const AutoPtr<IParcelable> AutoPtr< ::Creator<CatCmdMessage> > CREATOR;

private:
    AutoPtr<ITextMessage> mTextMsg;
    AutoPtr<IMenu> mMenu;
    AutoPtr<IInput> mInput;
    AutoPtr<BrowserSettings> mBrowserSettings;
    AutoPtr<IToneSettings> mToneSettings;
    AutoPtr<CallSettings> mCallSettings;
    AutoPtr<SetupEventListSettings> mSetupEventListSettings;
    Boolean mLoadIconFailed;
};

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CATCMDMESSAGE_H__
