#ifndef __ELASTOS_DROID_TELEPHONY_DCTCONSTANTS_H__
#define __ELASTOS_DROID_TELEPHONY_DCTCONSTANTS_H__

namespace Elastos {
namespace Droid {
namespace Telephony {

    class CDctConstants {

        public:

            /***** Event Codes *****/
            static const Int32 BASE;// = Protocol.BASE_DATA_CONNECTION_TRACKER;
            static const Int32 EVENT_DATA_SETUP_COMPLETE;// = BASE + 0;
            static const Int32 EVENT_RADIO_AVAILABLE;// = BASE + 1;
            static const Int32 EVENT_RECORDS_LOADED;// = BASE + 2;
            static const Int32 EVENT_TRY_SETUP_DATA;// = BASE + 3;
            static const Int32 EVENT_DATA_STATE_CHANGED;// = BASE + 4;
            static const Int32 EVENT_POLL_PDP;// = BASE + 5;
            static const Int32 EVENT_RADIO_OFF_OR_NOT_AVAILABLE;// = BASE + 6;
            static const Int32 EVENT_VOICE_CALL_STARTED;// = BASE + 7;
            static const Int32 EVENT_VOICE_CALL_ENDED;// = BASE + 8;
            static const Int32 EVENT_DATA_CONNECTION_DETACHED;// = BASE + 9;
            static const Int32 EVENT_LINK_STATE_CHANGED;// = BASE + 10;
            static const Int32 EVENT_ROAMING_ON;// = BASE + 11;
            static const Int32 EVENT_ROAMING_OFF;// = BASE + 12;
            static const Int32 EVENT_ENABLE_NEW_APN;// = BASE + 13;
            static const Int32 EVENT_RESTORE_DEFAULT_APN;// = BASE + 14;
            static const Int32 EVENT_DISCONNECT_DONE;// = BASE + 15;
            static const Int32 EVENT_DATA_CONNECTION_ATTACHED;// = BASE + 16;
            static const Int32 EVENT_DATA_STALL_ALARM;// = BASE + 17;
            static const Int32 EVENT_DO_RECOVERY;// = BASE + 18;
            static const Int32 EVENT_APN_CHANGED;// = BASE + 19;
            static const Int32 EVENT_CDMA_DATA_DETACHED;// = BASE + 20;
            static const Int32 EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED;// = BASE + 21;
            static const Int32 EVENT_PS_RESTRICT_ENABLED;// = BASE + 22;
            static const Int32 EVENT_PS_RESTRICT_DISABLED;// = BASE + 23;
            static const Int32 EVENT_CLEAN_UP_CONNECTION;// = BASE + 24;
            static const Int32 EVENT_CDMA_OTA_PROVISION;// = BASE + 25;
            static const Int32 EVENT_RESTART_RADIO;// = BASE + 26;
            static const Int32 EVENT_SET_INTERNAL_DATA_ENABLE;// = BASE + 27;
            static const Int32 EVENT_RESET_DONE;// = BASE + 28;
            static const Int32 EVENT_CLEAN_UP_ALL_CONNECTIONS;// = BASE + 29;
            static const Int32 CMD_SET_USER_DATA_ENABLE;// = BASE + 30;
            static const Int32 CMD_SET_DEPENDENCY_MET;// = BASE + 31;
            static const Int32 CMD_SET_POLICY_DATA_ENABLE;// = BASE + 32;
            static const Int32 EVENT_ICC_CHANGED;// = BASE + 33;

            /***** Constants *****/

            static const Int32 APN_INVALID_ID;// = -1;
            static const Int32 APN_DEFAULT_ID;// = 0;
            static const Int32 APN_MMS_ID;// = 1;
            static const Int32 APN_SUPL_ID;// = 2;
            static const Int32 APN_DUN_ID;// = 3;
            static const Int32 APN_HIPRI_ID;// = 4;
            static const Int32 APN_IMS_ID;// = 5;
            static const Int32 APN_FOTA_ID;// = 6;
            static const Int32 APN_CBS_ID;// = 7;
            static const Int32 APN_NUM_TYPES;// = 8;

            static const Int32 DISABLED;// = 0;
            static const Int32 ENABLED;// = 1;

            static const String APN_TYPE_KEY;// = "apnType";
            static String ACTION_DATA_CONNECTION_TRACKER_MESSENGER;// = "com.android.internal.telephony";
            static String EXTRA_MESSENGER;// = "EXTRA_MESSENGER";
    };

} // Telephony
} // Droid
} // Elastos
#endif //__ELASTOS_DROID_TELEPHONY_DCTCONSTANTS_H__
