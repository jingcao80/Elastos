
#ifndef __ELASTOS_DROID_SERVER_HDMI_HDMICECKEYCODE_H__
#define __ELASTOS_DROID_SERVER_HDMI_HDMICECKEYCODE_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Droid::View::IKeyEvent;
using Elastos::Core::IBoolean;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Helper class to translate android keycode to hdmi cec keycode and vice versa.
 */
class HdmiCecKeycode
    : public Object
{
private:
    /**
     * A mapping between Android and CEC keycode.
     *
     * <p>Normal implementation of this looks like
     * <pre>
     *    new KeycodeEntry(KeyEvent.KEYCODE_DPAD_CENTER, CEC_KEYCODE_SELECT);
     * </pre>
     * <p>However, some keys in CEC requires additional parameter.
     * In order to use parameterized cec key, add unique android keycode (existing or custom)
     * corresponding to a pair of cec keycode and and its param.
     * <pre>
     *    new KeycodeEntry(CUSTOME_ANDORID_KEY_1, CEC_KEYCODE_SELECT_BROADCAST_TYPE,
     *        UI_BROADCAST_TOGGLE_ALL);
     *    new KeycodeEntry(CUSTOME_ANDORID_KEY_2, CEC_KEYCODE_SELECT_BROADCAST_TYPE,
     *        UI_BROADCAST_ANALOGUE);
     * </pre>
     */
    class KeycodeEntry
        : public Object
    {
        friend class HdmiCecKeycode;
    private:
        KeycodeEntry(
            /* [in] */ Int32 androidKeycode,
            /* [in] */ Int32 cecKeycode,
            /* [in] */ Boolean isRepeatable);

        KeycodeEntry(
            /* [in] */ Int32 androidKeycode,
            /* [in] */ Int32 cecKeycode);

        CARAPI_(Int32) ToCecKeycodeIfMatched(
            /* [in] */ Int32 androidKeycode);

        CARAPI_(Int32) ToAndroidKeycodeIfMatched(
            /* [in] */ Int32 cecKeycode);

        CARAPI_(AutoPtr<IBoolean>) IsRepeatableIfMatched(
            /* [in] */ Int32 androidKeycode);

    private:
        Int32 mAndroidKeycode;

        Int32 mCecKeycode;

        Boolean mIsRepeatable;
    };

public:
    /**
     * Translate Android keycode to Hdmi Cec keycode.
     *
     * @param keycode Android keycode. For details, refer {@link KeyEvent}
     * @return single byte CEC keycode if matched.
     */
    static CARAPI AndroidKeyToCecKey(
        /* [in] */ Int32 keycode,
        /* [out] */ Int32* result);

    /**
     * Translate Hdmi CEC keycode to Android keycode.
     *
     * @param keycode CEC keycode
     * @return cec keycode corresponding to the given android keycode.
     *         If finds no matched keycode, return {@link #UNSUPPORTED_KEYCODE}
     */
    static CARAPI CecKeyToAndroidKey(
        /* [in] */ Int32 keycode,
        /* [out] */ Int32* result);

    /**
     * Whether the given {@code androidKeycode} is repeatable key or not.
     *
     * @param androidKeycode keycode of android
     * @return false if the given {@code androidKeycode} is not supported key code
     */
    static CARAPI IsRepeatableKey(
        /* [in] */ Int32 androidKeycode,
        /* [out] */ Boolean* result);

    /**
     * Returns {@code true} if given Android keycode is supported, otherwise {@code false}.
     */
    static CARAPI IsSupportedKeycode(
        /* [in] */ Int32 androidKeycode,
        /* [out] */ Boolean* result);

private:
    HdmiCecKeycode();

    static AutoPtr<ArrayOf<KeycodeEntry*> > InitKEYCODE_ENTRIES();

public:
    static const Int32 UNSUPPORTED_KEYCODE;
    static const Int32 NO_PARAM;

    // =========================================================================
    // Hdmi CEC keycodes
    static const Int32 CEC_KEYCODE_SELECT;
    static const Int32 CEC_KEYCODE_UP;
    static const Int32 CEC_KEYCODE_DOWN;
    static const Int32 CEC_KEYCODE_LEFT;
    static const Int32 CEC_KEYCODE_RIGHT;
    static const Int32 CEC_KEYCODE_RIGHT_UP;
    static const Int32 CEC_KEYCODE_RIGHT_DOWN;
    static const Int32 CEC_KEYCODE_LEFT_UP;
    static const Int32 CEC_KEYCODE_LEFT_DOWN;
    static const Int32 CEC_KEYCODE_ROOT_MENU;
    static const Int32 CEC_KEYCODE_SETUP_MENU;
    static const Int32 CEC_KEYCODE_CONTENTS_MENU;
    static const Int32 CEC_KEYCODE_FAVORITE_MENU;
    static const Int32 CEC_KEYCODE_EXIT;
    // RESERVED = 0x0E - 0x0F
    static const Int32 CEC_KEYCODE_MEDIA_TOP_MENU;
    static const Int32 CEC_KEYCODE_MEDIA_CONTEXT_SENSITIVE_MENU;
    // RESERVED = 0x12 – 0x1C
    static const Int32 CEC_KEYCODE_NUMBER_ENTRY_MODE;
    static const Int32 CEC_KEYCODE_NUMBER_11;
    static const Int32 CEC_KEYCODE_NUMBER_12;
    static const Int32 CEC_KEYCODE_NUMBER_0_OR_NUMBER_10;
    static const Int32 CEC_KEYCODE_NUMBERS_1;
    static const Int32 CEC_KEYCODE_NUMBERS_2;
    static const Int32 CEC_KEYCODE_NUMBERS_3;
    static const Int32 CEC_KEYCODE_NUMBERS_4;
    static const Int32 CEC_KEYCODE_NUMBERS_5;
    static const Int32 CEC_KEYCODE_NUMBERS_6;
    static const Int32 CEC_KEYCODE_NUMBERS_7;
    static const Int32 CEC_KEYCODE_NUMBERS_8;
    static const Int32 CEC_KEYCODE_NUMBERS_9;
    static const Int32 CEC_KEYCODE_DOT;
    static const Int32 CEC_KEYCODE_ENTER;
    static const Int32 CEC_KEYCODE_CLEAR;
    // RESERVED = 0x2D - 0x2E
    static const Int32 CEC_KEYCODE_NEXT_FAVORITE;
    static const Int32 CEC_KEYCODE_CHANNEL_UP;
    static const Int32 CEC_KEYCODE_CHANNEL_DOWN;
    static const Int32 CEC_KEYCODE_PREVIOUS_CHANNEL;
    static const Int32 CEC_KEYCODE_SOUND_SELECT;
    static const Int32 CEC_KEYCODE_INPUT_SELECT;
    static const Int32 CEC_KEYCODE_DISPLAY_INFORMATION;
    static const Int32 CEC_KEYCODE_HELP;
    static const Int32 CEC_KEYCODE_PAGE_UP;
    static const Int32 CEC_KEYCODE_PAGE_DOWN;
    // RESERVED = 0x39 - 0x3F
    static const Int32 CEC_KEYCODE_POWER;
    static const Int32 CEC_KEYCODE_VOLUME_UP;
    static const Int32 CEC_KEYCODE_VOLUME_DOWN;
    static const Int32 CEC_KEYCODE_MUTE;
    static const Int32 CEC_KEYCODE_PLAY;
    static const Int32 CEC_KEYCODE_STOP;
    static const Int32 CEC_KEYCODE_PAUSE;
    static const Int32 CEC_KEYCODE_RECORD;
    static const Int32 CEC_KEYCODE_REWIND;
    static const Int32 CEC_KEYCODE_FAST_FORWARD;
    static const Int32 CEC_KEYCODE_EJECT;
    static const Int32 CEC_KEYCODE_FORWARD;
    static const Int32 CEC_KEYCODE_BACKWARD;
    static const Int32 CEC_KEYCODE_STOP_RECORD;
    static const Int32 CEC_KEYCODE_PAUSE_RECORD;
    static const Int32 CEC_KEYCODE_RESERVED;
    static const Int32 CEC_KEYCODE_ANGLE;
    static const Int32 CEC_KEYCODE_SUB_PICTURE;
    static const Int32 CEC_KEYCODE_VIDEO_ON_DEMAND;
    static const Int32 CEC_KEYCODE_ELECTRONIC_PROGRAM_GUIDE;
    static const Int32 CEC_KEYCODE_TIMER_PROGRAMMING;
    static const Int32 CEC_KEYCODE_INITIAL_CONFIGURATION;
    static const Int32 CEC_KEYCODE_SELECT_BROADCAST_TYPE;
    static const Int32 CEC_KEYCODE_SELECT_SOUND_PRESENTATION;
    // RESERVED = 0x58-0x5F
    static const Int32 CEC_KEYCODE_PLAY_FUNCTION;
    static const Int32 CEC_KEYCODE_PAUSE_PLAY_FUNCTION;
    static const Int32 CEC_KEYCODE_RECORD_FUNCTION;
    static const Int32 CEC_KEYCODE_PAUSE_RECORD_FUNCTION;
    static const Int32 CEC_KEYCODE_STOP_FUNCTION;
    static const Int32 CEC_KEYCODE_MUTE_FUNCTION;
    static const Int32 CEC_KEYCODE_RESTORE_VOLUME_FUNCTION;
    static const Int32 CEC_KEYCODE_TUNE_FUNCTION;
    static const Int32 CEC_KEYCODE_SELECT_MEDIA_FUNCTION;
    static const Int32 CEC_KEYCODE_SELECT_AV_INPUT_FUNCTION;
    static const Int32 CEC_KEYCODE_SELECT_AUDIO_INPUT_FUNCTION;
    static const Int32 CEC_KEYCODE_POWER_TOGGLE_FUNCTION;
    static const Int32 CEC_KEYCODE_POWER_OFF_FUNCTION;
    static const Int32 CEC_KEYCODE_POWER_ON_FUNCTION;
    // RESERVED = 0x6E-0x70
    static const Int32 CEC_KEYCODE_F1_BLUE;
    static const Int32 CEC_KEYCODE_F2_RED;
    static const Int32 CEC_KEYCODE_F3_GREEN;
    static const Int32 CEC_KEYCODE_F4_YELLOW;
    static const Int32 CEC_KEYCODE_F5;
    static const Int32 CEC_KEYCODE_DATA;
    // RESERVED = 0x77-0xFF

    // =========================================================================
    // UI Broadcast Type
    static const Int32 UI_BROADCAST_TOGGLE_ALL;
    static const Int32 UI_BROADCAST_TOGGLE_ANALOGUE_DIGITAL;
    static const Int32 UI_BROADCAST_ANALOGUE;
    static const Int32 UI_BROADCAST_ANALOGUE_TERRESTRIAL;
    static const Int32 UI_BROADCAST_ANALOGUE_CABLE;
    static const Int32 UI_BROADCAST_ANALOGUE_SATELLITE;
    static const Int32 UI_BROADCAST_DIGITAL;
    static const Int32 UI_BROADCAST_DIGITAL_TERRESTRIAL;
    static const Int32 UI_BROADCAST_DIGITAL_CABLE;
    static const Int32 UI_BROADCAST_DIGITAL_SATELLITE;
    static const Int32 UI_BROADCAST_DIGITAL_COMMNICATIONS_SATELLITE;
    static const Int32 UI_BROADCAST_DIGITAL_COMMNICATIONS_SATELLITE_2;
    static const Int32 UI_BROADCAST_IP;

    // =========================================================================
    // UI Sound Presentation Control
    static const Int32 UI_SOUND_PRESENTATION_SOUND_MIX_DUAL_MONO;
    static const Int32 UI_SOUND_PRESENTATION_SOUND_MIX_KARAOKE;
    static const Int32 UI_SOUND_PRESENTATION_SELECT_AUDIO_DOWN_MIX;
    static const Int32 UI_SOUND_PRESENTATION_SELECT_AUDIO_AUTO_REVERBERATION;
    static const Int32 UI_SOUND_PRESENTATION_SELECT_AUDIO_AUTO_EQUALIZER;
    static const Int32 UI_SOUND_PRESENTATION_BASS_STEP_PLUS;
    static const Int32 UI_SOUND_PRESENTATION_BASS_NEUTRAL;
    static const Int32 UI_SOUND_PRESENTATION_BASS_STEP_MINUS;
    static const Int32 UI_SOUND_PRESENTATION_TREBLE_STEP_PLUS;
    static const Int32 UI_SOUND_PRESENTATION_TREBLE_NEUTRAL;
    static const Int32 UI_SOUND_PRESENTATION_TREBLE_STEP_MINUS;

private:
    // Keycode entry container for all mappings.
    // Note that order of entry is the same as above cec keycode definition.
    static const AutoPtr<ArrayOf<KeycodeEntry*> > KEYCODE_ENTRIES;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Server::Hdmi::HdmiCecKeycode::KeycodeEntry, IInterface)

#endif // __ELASTOS_DROID_SERVER_HDMI_HDMICECKEYCODE_H__
