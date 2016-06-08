#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_COMMANDPARAMSFACTORY_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_COMMANDPARAMSFACTORY_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/internal/telephony/cat/CommandParams.h"
#include "elastos/droid/internal/telephony/cat/ComprehensionTlv.h"
#include "elastos/droid/internal/telephony/cat/CommandDetails.h"
//#include "elastos/droid/internal/telephony/cat/RilMessageDecoder.h"
#include "elastos/droid/internal/telephony/cat/BerTlv.h"
#include "elastos/droid/internal/telephony/cat/IconLoader.h"

// import android.content.res.Resources;
// import android.graphics.Bitmap;
// import android.os.Handler;
// import com.android.internal.telephony.GsmAlphabet;

using Elastos::Droid::Internal::Telephony::Uicc::IIccFileHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Utility::IIterator;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

class RilMessageDecoder;

//// import static com.android.internal.telephony.cat.CatCmdMessage.
// SetupEventListConstants.USER_ACTIVITY_EVENT;
//// import static com.android.internal.telephony.cat.CatCmdMessage.
// SetupEventListConstants.IDLE_SCREEN_AVAILABLE_EVENT;
//// import static com.android.internal.telephony.cat.CatCmdMessage.
// SetupEventListConstants.LANGUAGE_SELECTION_EVENT;
//// import static com.android.internal.telephony.cat.CatCmdMessage.
// SetupEventListConstants.BROWSER_TERMINATION_EVENT;
//// import static com.android.internal.telephony.cat.CatCmdMessage.
// SetupEventListConstants.BROWSING_STATUS_EVENT;
//// import static com.android.internal.telephony.cat.CatCmdMessage.
// SetupEventListConstants.HCI_CONNECTIVITY_EVENT;
/**
  * Factory class, used for decoding raw byte arrays, received from baseband,
  * into a CommandParams object.
  *
  */
class CommandParamsFactory
    : public Handler
{
public:
    // synchronized
    static CARAPI_(AutoPtr<CommandParamsFactory>) GetInstance(
        /* [in] */ RilMessageDecoder* caller,
        /* [in] */ IIccFileHandler* fh);

    virtual CARAPI Make(
        /* [in] */ BerTlv* berTlv);

    // @Override
    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    virtual CARAPI Dispose();

private:
    CommandParamsFactory(
        /* [in] */ RilMessageDecoder* caller,
        /* [in] */ IIccFileHandler* fh);

    CARAPI_(AutoPtr<CommandDetails>) ProcessCommandDetails(
        /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs);

    CARAPI_(ResultCode) SetIcons(
        /* [in] */ IInterface* data);

    CARAPI_(void) SendCmdParams(
        /* [in] */ ResultCode resCode);

    /**
      * Search for a COMPREHENSION-TLV object with the given tag from a list
      *
      * @param tag A tag to search for
      * @param ctlvs List of ComprehensionTlv objects used to search in
      *
      * @return A ComprehensionTlv object that has the tag value of {@code tag}.
      *         If no object is found with the tag, null is returned.
      */
    CARAPI_(AutoPtr<ComprehensionTlv>) SearchForTag(
        /* [in] */ ComprehensionTlvTag tag,
        /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs);

    /**
      * Search for the next COMPREHENSION-TLV object with the given tag from a
      * list iterated by {@code iter}. {@code iter} points to the object next to
      * the found object when this method returns. Used for searching the same
      * list for similar tags, usually item id.
      *
      * @param tag A tag to search for
      * @param iter Iterator for ComprehensionTlv objects used for search
      *
      * @return A ComprehensionTlv object that has the tag value of {@code tag}.
      *         If no object is found with the tag, null is returned.
      */
    CARAPI_(AutoPtr<ComprehensionTlv>) SearchForNextTag(
        /* [in] */ ComprehensionTlvTag tag,
        /* [in] */ IIterator/*<ComprehensionTlv*>*/* iter);

    /**
      * Processes DISPLAY_TEXT proactive command from the SIM card.
      *
      * @param cmdDet Command Details container object.
      * @param ctlvs List of ComprehensionTlv objects following Command Details
      *        object and Device Identities object within the proactive command
      * @return true if the command is processing is pending and additional
      *         asynchronous processing is required.
      * @throws ResultException
      */
    CARAPI_(Boolean) ProcessDisplayText(
        /* [in] */ CommandDetails* cmdDet,
        /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs);

    /**
      * Processes SET_UP_IDLE_MODE_TEXT proactive command from the SIM card.
      *
      * @param cmdDet Command Details container object.
      * @param ctlvs List of ComprehensionTlv objects following Command Details
      *        object and Device Identities object within the proactive command
      * @return true if the command is processing is pending and additional
      *         asynchronous processing is required.
      * @throws ResultException
      */
    CARAPI_(Boolean) ProcessSetUpIdleModeText(
        /* [in] */ CommandDetails* cmdDet,
        /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs);

    /**
      * Processes GET_INKEY proactive command from the SIM card.
      *
      * @param cmdDet Command Details container object.
      * @param ctlvs List of ComprehensionTlv objects following Command Details
      *        object and Device Identities object within the proactive command
      * @return true if the command is processing is pending and additional
      *         asynchronous processing is required.
      * @throws ResultException
      */
    CARAPI_(Boolean) ProcessGetInkey(
        /* [in] */ CommandDetails* cmdDet,
        /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs);

    /**
      * Processes GET_INPUT proactive command from the SIM card.
      *
      * @param cmdDet Command Details container object.
      * @param ctlvs List of ComprehensionTlv objects following Command Details
      *        object and Device Identities object within the proactive command
      * @return true if the command is processing is pending and additional
      *         asynchronous processing is required.
      * @throws ResultException
      */
    CARAPI_(Boolean) ProcessGetInput(
        /* [in] */ CommandDetails* cmdDet,
        /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs);

    /**
      * Processes SELECT_ITEM proactive command from the SIM card.
      *
      * @param cmdDet Command Details container object.
      * @param ctlvs List of ComprehensionTlv objects following Command Details
      *        object and Device Identities object within the proactive command
      * @return true if the command is processing is pending and additional
      *         asynchronous processing is required.
      * @throws ResultException
      */
    CARAPI_(Boolean) ProcessSelectItem(
        /* [in] */ CommandDetails* cmdDet,
        /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs);

    /**
      * Processes EVENT_NOTIFY message from baseband.
      *
      * @param cmdDet Command Details container object.
      * @param ctlvs List of ComprehensionTlv objects following Command Details
      *        object and Device Identities object within the proactive command
      * @return true if the command is processing is pending and additional
      *         asynchronous processing is required.
      */
    CARAPI_(Boolean) ProcessEventNotify(
        /* [in] */ CommandDetails* cmdDet,
        /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs);

    /**
      * Processes SET_UP_EVENT_LIST proactive command from the SIM card.
      *
      * @param cmdDet Command Details object retrieved.
      * @param ctlvs List of ComprehensionTlv objects following Command Details
      *        object and Device Identities object within the proactive command
      * @return false. This function always returns false meaning that the command
      *         processing is  not pending and additional asynchronous processing
      *         is not required.
      */
    CARAPI_(Boolean) ProcessSetUpEventList(
        /* [in] */ CommandDetails* cmdDet,
        /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs);

    /**
      * Processes LAUNCH_BROWSER proactive command from the SIM card.
      *
      * @param cmdDet Command Details container object.
      * @param ctlvs List of ComprehensionTlv objects following Command Details
      *        object and Device Identities object within the proactive command
      * @return true if the command is processing is pending and additional
      *         asynchronous processing is required.
      * @throws ResultException
      */
    CARAPI_(Boolean) ProcessLaunchBrowser(
        /* [in] */ CommandDetails* cmdDet,
        /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs);

    /**
      * Processes PLAY_TONE proactive command from the SIM card.
      *
      * @param cmdDet Command Details container object.
      * @param ctlvs List of ComprehensionTlv objects following Command Details
      *        object and Device Identities object within the proactive command
      * @return true if the command is processing is pending and additional
      *         asynchronous processing is required.t
      * @throws ResultException
      */
    CARAPI_(Boolean) ProcessPlayTone(
        /* [in] */ CommandDetails* cmdDet,
        /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs);

    /**
      * Processes SETUP_CALL proactive command from the SIM card.
      *
      * @param cmdDet Command Details object retrieved from the proactive command
      *        object
      * @param ctlvs List of ComprehensionTlv objects following Command Details
      *        object and Device Identities object within the proactive command
      * @return true if the command is processing is pending and additional
      *         asynchronous processing is required.
      */
    CARAPI_(Boolean) ProcessSetupCall(
        /* [in] */ CommandDetails* cmdDet,
        /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs);

    CARAPI_(Boolean) ProcessProvideLocalInfo(
        /* [in] */ CommandDetails* cmdDet,
        /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs);

    CARAPI_(Boolean) ProcessBIPClient(
        /* [in] */ CommandDetails* cmdDet,
        /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs);

    CARAPI_(Boolean) ProcessActivate(
        /* [in] */ CommandDetails* cmdDet,
        /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs);

public:
    // constants
    static const Int32 MSG_ID_LOAD_ICON_DONE = 1;
    // loading icons state parameters.
    static const Int32 LOAD_NO_ICON = 0;
    static const Int32 LOAD_SINGLE_ICON = 1;
    static const Int32 LOAD_MULTI_ICONS = 2;
    // Command Qualifier values for PLI command
    static const Int32 DTTZ_SETTING = 0x03;
    static const Int32 LANGUAGE_SETTING = 0x04;

private:
    static AutoPtr<CommandParamsFactory> sInstance;
    AutoPtr<IconLoader> mIconLoader;
    AutoPtr<CommandParams> mCmdParams;
    Int32 mIconLoadState;
    RilMessageDecoder* mCaller;
    Boolean mloadIcon;
    // As per TS 102.223 Annex C, Structure of CAT communications,
    // the APDU length can be max 255 bytes. This leaves only 239 bytes for user
    // input string. CMD details TLV + Device IDs TLV + Result TLV + Other
    // details of TextString TLV not including user input take 16 bytes.
    //
    // If UCS2 encoding is used, maximum 118 UCS2 chars can be encoded in 238 bytes.
    // Each UCS2 char takes 2 bytes. Byte Order Mask(BOM), 0xFEFF takes 2 bytes.
    //
    // If GSM 7 bit default(use 8 bits to represent a 7 bit char) format is used,
    // maximum 239 chars can be encoded in 239 bytes since each char takes 1 byte.
    //
    // No issues for GSM 7 bit packed format encoding.
    static const Int32 MAX_GSM7_DEFAULT_CHARS = 239;
    static const Int32 MAX_UCS2_CHARS = 118;
};

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_COMMANDPARAMSFACTORY_H__

