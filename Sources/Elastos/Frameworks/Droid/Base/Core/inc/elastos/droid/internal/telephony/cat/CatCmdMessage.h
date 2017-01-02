//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
        String mUrl;
        LaunchBrowserMode mMode;
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

    public:
        AutoPtr<ITextMessage> mConfirmMsg;
        AutoPtr<ITextMessage> mCallMsg;
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

    public:
        AutoPtr<ArrayOf<Int32> > mEventList;
    };

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

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* in);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);
        ///* [in] */ Int32 flags);

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
