#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_COMMANDPARAMS_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_COMMANDPARAMS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"
#include "elastos/droid/internal/telephony/cat/CommandDetails.h"

using Elastos::Droid::Graphics::IBitmap;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

/**
  * Container class for proactive command parameters.
  *
  */
class CommandParams
    : public Object
{
public:
    CommandParams(
        /* [in] */ CommandDetails* cmdDet);

    virtual CARAPI GetCommandType(
        /* [out] */ CommandType* result);

    virtual CARAPI SetIcon(
        /* [in] */ IBitmap* icon,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

public:
    AutoPtr<CommandDetails> mCmdDet;
    // Variable to track if an optional icon load has failed.
    Boolean mLoadIconFailed;
};

class DisplayTextParams
    : public CommandParams
{
public:
    DisplayTextParams(
        /* [in] */ CommandDetails* cmdDet,
        /* [in] */ ITextMessage* textMsg);

    // @Override
    CARAPI SetIcon(
        /* [in] */ IBitmap* icon,
        /* [out] */ Boolean* result);

public:
    AutoPtr<ITextMessage> mTextMsg;
};

class LaunchBrowserParams
    : public CommandParams
{
public:
    LaunchBrowserParams(
        /* [in] */ CommandDetails* cmdDet,
        /* [in] */ ITextMessage* confirmMsg,
        /* [in] */ const String& url,
        /* [in] */ LaunchBrowserMode mode);

    // @Override
    CARAPI SetIcon(
        /* [in] */ IBitmap* icon,
        /* [out] */ Boolean* result);

public:
    AutoPtr<ITextMessage> mConfirmMsg;
    LaunchBrowserMode mMode;
    String mUrl;
};

class SetEventListParams
    : public CommandParams
{
public:
    SetEventListParams(
        /* [in] */ CommandDetails* cmdDet,
        /* [in] */ ArrayOf<Int32>* eventInfo);

public:
    AutoPtr< ArrayOf<Int32> > mEventInfo;
};

class PlayToneParams
    : public CommandParams
{
public:
    PlayToneParams(
        /* [in] */ CommandDetails* cmdDet,
        /* [in] */ ITextMessage* textMsg,
        /* [in] */ Tone tone,
        /* [in] */ IDuration* duration,
        /* [in] */ Boolean vibrate);

    // @Override
    CARAPI SetIcon(
        /* [in] */ IBitmap* icon,
        /* [out] */ Boolean* result);

public:
    AutoPtr<ITextMessage> mTextMsg;
    AutoPtr<IToneSettings> mSettings;
};

class CallSetupParams
    : public CommandParams
{
public:
    CallSetupParams(
        /* [in] */ CommandDetails* cmdDet,
        /* [in] */ ITextMessage* confirmMsg,
        /* [in] */ ITextMessage* callMsg);

    // @Override
    CARAPI SetIcon(
        /* [in] */ IBitmap* icon,
        /* [out] */ Boolean* result);

public:
    AutoPtr<ITextMessage> mConfirmMsg;
    AutoPtr<ITextMessage> mCallMsg;
};

class SelectItemParams
    : public CommandParams
{
public:
    SelectItemParams(
        /* [in] */ CommandDetails* cmdDet,
        /* [in] */ IMenu* menu,
        /* [in] */ Boolean loadTitleIcon);

    // @Override
    CARAPI SetIcon(
        /* [in] */ IBitmap* icon,
        /* [out] */ Boolean* result);

public:
    AutoPtr<IMenu> mMenu;
    Boolean mLoadTitleIcon;
};

class GetInputParams
    : public CommandParams
{
public:
    GetInputParams(
        /* [in] */ CommandDetails* cmdDet,
        /* [in] */ IInput* input);

    // @Override
    CARAPI SetIcon(
        /* [in] */ IBitmap* icon,
        /* [out] */ Boolean* result);

public:
    AutoPtr<IInput> mInput;
};

/*
  * BIP (Bearer Independent Protocol) is the mechanism for SIM card applications
  * to access data connection through the mobile device.
  *
  * SIM utilizes proactive commands (OPEN CHANNEL, CLOSE CHANNEL, SEND DATA and
  * RECEIVE DATA to control/read/write data for BIP. Refer to ETSI TS 102 223 for
  * the details of proactive commands procedures and their structures.
  */
class BIPClientParams
    : public CommandParams
{
public:
    BIPClientParams(
        /* [in] */ CommandDetails* cmdDet,
        /* [in] */ ITextMessage* textMsg,
        /* [in] */ Boolean has_alpha_id);

    // @Override
    CARAPI SetIcon(
        /* [in] */ IBitmap* icon,
        /* [out] */ Boolean* result);

public:
    AutoPtr<ITextMessage> mTextMsg;
    Boolean mHasAlphaId;
};

class ActivateParams
    : public CommandParams
{
public:
    ActivateParams(
        /* [in] */ CommandDetails* cmdDet,
        /* [in] */ Int32 target);

public:
    Int32 mActivateTarget;
};

// Samsung STK
class SendSMSParams
    : public DisplayTextParams
{
public:
    SendSMSParams(
        /* [in] */ CommandDetails* cmdDet,
        /* [in] */ ITextMessage* textmessage,
        /* [in] */ const String& smscaddress,
        /* [in] */ const String& smsPdu);

public:
    String mPdu;
    String mSmscAddress;
};

class SendUSSDParams
    : public DisplayTextParams
{
public:
    SendUSSDParams(
        /* [in] */ CommandDetails* cmdDet,
        /* [in] */ ITextMessage* textmessage,
        /* [in] */ const String& ussdstring);

public:
    String mUssdString;
};

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_COMMANDPARAMS_H__
