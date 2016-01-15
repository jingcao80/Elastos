
#include "CDctConstants.h"

using IProtocol;

namespace Elastos {
namespace Droid {
namespace Telephony {

    const Int32 CDctConstants::BASE = IProtocol::BASE_DATA_CONNECTION_TRACKER;
    const Int32 CDctConstants::EVENT_DATA_SETUP_COMPLETE = BASE + 0;
    const Int32 CDctConstants::EVENT_RADIO_AVAILABLE = BASE + 1;
    const Int32 CDctConstants::EVENT_RECORDS_LOADED = BASE + 2;
    const Int32 CDctConstants::EVENT_TRY_SETUP_DATA = BASE + 3;
    const Int32 CDctConstants::EVENT_DATA_STATE_CHANGED = BASE + 4;
    const Int32 CDctConstants::EVENT_POLL_PDP = BASE + 5;
    const Int32 CDctConstants::EVENT_RADIO_OFF_OR_NOT_AVAILABLE = BASE + 6;
    const Int32 CDctConstants::EVENT_VOICE_CALL_STARTED = BASE + 7;
    const Int32 CDctConstants::EVENT_VOICE_CALL_ENDED = BASE + 8;
    const Int32 CDctConstants::EVENT_DATA_CONNECTION_DETACHED = BASE + 9;
    const Int32 CDctConstants::EVENT_LINK_STATE_CHANGED = BASE + 10;
    const Int32 CDctConstants::EVENT_ROAMING_ON = BASE + 11;
    const Int32 CDctConstants::EVENT_ROAMING_OFF = BASE + 12;
    const Int32 CDctConstants::EVENT_ENABLE_NEW_APN = BASE + 13;
    const Int32 CDctConstants::EVENT_RESTORE_DEFAULT_APN = BASE + 14;
    const Int32 CDctConstants::EVENT_DISCONNECT_DONE = BASE + 15;
    const Int32 CDctConstants::EVENT_DATA_CONNECTION_ATTACHED = BASE + 16;
    const Int32 CDctConstants::EVENT_DATA_STALL_ALARM = BASE + 17;
    const Int32 CDctConstants::EVENT_DO_RECOVERY = BASE + 18;
    const Int32 CDctConstants::EVENT_APN_CHANGED = BASE + 19;
    const Int32 CDctConstants::EVENT_CDMA_DATA_DETACHED = BASE + 20;
    const Int32 CDctConstants::EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED = BASE + 21;
    const Int32 CDctConstants::EVENT_PS_RESTRICT_ENABLED = BASE + 22;
    const Int32 CDctConstants::EVENT_PS_RESTRICT_DISABLED = BASE + 23;
    const Int32 CDctConstants::EVENT_CLEAN_UP_CONNECTION = BASE + 24;
    const Int32 CDctConstants::EVENT_CDMA_OTA_PROVISION = BASE + 25;
    const Int32 CDctConstants::EVENT_RESTART_RADIO = BASE + 26;
    const Int32 CDctConstants::EVENT_SET_INTERNAL_DATA_ENABLE = BASE + 27;
    const Int32 CDctConstants::EVENT_RESET_DONE = BASE + 28;
    const Int32 CDctConstants::EVENT_CLEAN_UP_ALL_CONNECTIONS = BASE + 29;
    const Int32 CDctConstants::CMD_SET_USER_DATA_ENABLE = BASE + 30;
    const Int32 CDctConstants::CMD_SET_DEPENDENCY_MET = BASE + 31;
    const Int32 CDctConstants::CMD_SET_POLICY_DATA_ENABLE = BASE + 32;
    const Int32 CDctConstants::EVENT_ICC_CHANGED = BASE + 33;

    const Int32 CDctConstants::APN_INVALID_ID = -1;
    const Int32 CDctConstants::APN_DEFAULT_ID = 0;
    const Int32 CDctConstants::APN_MMS_ID = 1;
    const Int32 CDctConstants::APN_SUPL_ID = 2;
    const Int32 CDctConstants::APN_DUN_ID = 3;
    const Int32 CDctConstants::APN_HIPRI_ID = 4;
    const Int32 CDctConstants::APN_IMS_ID = 5;
    const Int32 CDctConstants::APN_FOTA_ID = 6;
    const Int32 CDctConstants::APN_CBS_ID = 7;
    const Int32 CDctConstants::APN_NUM_TYPES = 8;

    const Int32 CDctConstants::DISABLED = 0;
    const Int32 CDctConstants::ENABLED = 1;

    const String CDctConstants::APN_TYPE_KEY("apnType");
    String CDctConstants::ACTION_DATA_CONNECTION_TRACKER_MESSENGER("com.android.internal.telephony");
    String CDctConstants::EXTRA_MESSENGER("EXTRA_MESSENGER");

} // Telephony
} // Droid
} // Elastos