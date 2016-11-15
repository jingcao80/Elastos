
#include "elastos/droid/server/hdmi/HdmiCecKeycode.h"
#include <Elastos.Droid.View.h>

using Elastos::Core::CBoolean;
using Elastos::Core::IBoolean;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

//=============================================================================
// HdmiCecKeycode::KeycodeEntry
//=============================================================================
HdmiCecKeycode::KeycodeEntry::KeycodeEntry(
    /* [in] */ Int32 androidKeycode,
    /* [in] */ Int32 cecKeycode,
    /* [in] */ Boolean isRepeatable)
{
    mElastosKeycode = androidKeycode;
    mCecKeycode = cecKeycode;
    mIsRepeatable = isRepeatable;
}

HdmiCecKeycode::KeycodeEntry::KeycodeEntry(
    /* [in] */ Int32 androidKeycode,
    /* [in] */ Int32 cecKeycode)
{
    KeycodeEntry(androidKeycode, cecKeycode, TRUE);
}

Int32 HdmiCecKeycode::KeycodeEntry::ToCecKeycodeIfMatched(
    /* [in] */ Int32 androidKeycode)
{
    if (mElastosKeycode == androidKeycode) {
        return mCecKeycode;
    } else {
        return UNSUPPORTED_KEYCODE;
    }
}

Int32 HdmiCecKeycode::KeycodeEntry::ToElastosKeycodeIfMatched(
    /* [in] */ Int32 cecKeycode)
{
    if (cecKeycode == mCecKeycode) {
        return mElastosKeycode;
    } else {
        return UNSUPPORTED_KEYCODE;
    }
}

AutoPtr<IBoolean> HdmiCecKeycode::KeycodeEntry::IsRepeatableIfMatched(
    /* [in] */ Int32 androidKeycode)
{
    AutoPtr<IBoolean> rev;
    if (mElastosKeycode == androidKeycode) {
        CBoolean::New(mIsRepeatable, (IBoolean**)&rev);
    }
    return rev;
}

//=============================================================================
// HdmiCecKeycode
//=============================================================================
const Int32 HdmiCecKeycode::UNSUPPORTED_KEYCODE = -1;
const Int32 HdmiCecKeycode::NO_PARAM = -1;
const Int32 HdmiCecKeycode::CEC_KEYCODE_SELECT = 0x00;
const Int32 HdmiCecKeycode::CEC_KEYCODE_UP = 0x01;
const Int32 HdmiCecKeycode::CEC_KEYCODE_DOWN = 0x02;
const Int32 HdmiCecKeycode::CEC_KEYCODE_LEFT = 0x03;
const Int32 HdmiCecKeycode::CEC_KEYCODE_RIGHT = 0x04;
const Int32 HdmiCecKeycode::CEC_KEYCODE_RIGHT_UP = 0x05;
const Int32 HdmiCecKeycode::CEC_KEYCODE_RIGHT_DOWN = 0x06;
const Int32 HdmiCecKeycode::CEC_KEYCODE_LEFT_UP = 0x07;
const Int32 HdmiCecKeycode::CEC_KEYCODE_LEFT_DOWN = 0x08;
const Int32 HdmiCecKeycode::CEC_KEYCODE_ROOT_MENU = 0x09;
const Int32 HdmiCecKeycode::CEC_KEYCODE_SETUP_MENU = 0x0A;
const Int32 HdmiCecKeycode::CEC_KEYCODE_CONTENTS_MENU = 0x0B;
const Int32 HdmiCecKeycode::CEC_KEYCODE_FAVORITE_MENU = 0x0C;
const Int32 HdmiCecKeycode::CEC_KEYCODE_EXIT = 0x0D;
const Int32 HdmiCecKeycode::CEC_KEYCODE_MEDIA_TOP_MENU = 0x10;
const Int32 HdmiCecKeycode::CEC_KEYCODE_MEDIA_CONTEXT_SENSITIVE_MENU = 0x11;
const Int32 HdmiCecKeycode::CEC_KEYCODE_NUMBER_ENTRY_MODE = 0x1D;
const Int32 HdmiCecKeycode::CEC_KEYCODE_NUMBER_11 = 0x1E;
const Int32 HdmiCecKeycode::CEC_KEYCODE_NUMBER_12 = 0x1F;
const Int32 HdmiCecKeycode::CEC_KEYCODE_NUMBER_0_OR_NUMBER_10 = 0x20;
const Int32 HdmiCecKeycode::CEC_KEYCODE_NUMBERS_1 = 0x21;
const Int32 HdmiCecKeycode::CEC_KEYCODE_NUMBERS_2 = 0x22;
const Int32 HdmiCecKeycode::CEC_KEYCODE_NUMBERS_3 = 0x23;
const Int32 HdmiCecKeycode::CEC_KEYCODE_NUMBERS_4 = 0x24;
const Int32 HdmiCecKeycode::CEC_KEYCODE_NUMBERS_5 = 0x25;
const Int32 HdmiCecKeycode::CEC_KEYCODE_NUMBERS_6 = 0x26;
const Int32 HdmiCecKeycode::CEC_KEYCODE_NUMBERS_7 = 0x27;
const Int32 HdmiCecKeycode::CEC_KEYCODE_NUMBERS_8 = 0x28;
const Int32 HdmiCecKeycode::CEC_KEYCODE_NUMBERS_9 = 0x29;
const Int32 HdmiCecKeycode::CEC_KEYCODE_DOT = 0x2A;
const Int32 HdmiCecKeycode::CEC_KEYCODE_ENTER = 0x2B;
const Int32 HdmiCecKeycode::CEC_KEYCODE_CLEAR = 0x2C;
const Int32 HdmiCecKeycode::CEC_KEYCODE_NEXT_FAVORITE = 0x2F;
const Int32 HdmiCecKeycode::CEC_KEYCODE_CHANNEL_UP = 0x30;
const Int32 HdmiCecKeycode::CEC_KEYCODE_CHANNEL_DOWN = 0x31;
const Int32 HdmiCecKeycode::CEC_KEYCODE_PREVIOUS_CHANNEL = 0x32;
const Int32 HdmiCecKeycode::CEC_KEYCODE_SOUND_SELECT = 0x33;
const Int32 HdmiCecKeycode::CEC_KEYCODE_INPUT_SELECT = 0x34;
const Int32 HdmiCecKeycode::CEC_KEYCODE_DISPLAY_INFORMATION = 0x35;
const Int32 HdmiCecKeycode::CEC_KEYCODE_HELP = 0x36;
const Int32 HdmiCecKeycode::CEC_KEYCODE_PAGE_UP = 0x37;
const Int32 HdmiCecKeycode::CEC_KEYCODE_PAGE_DOWN = 0x38;
const Int32 HdmiCecKeycode::CEC_KEYCODE_POWER = 0x40;
const Int32 HdmiCecKeycode::CEC_KEYCODE_VOLUME_UP = 0x41;
const Int32 HdmiCecKeycode::CEC_KEYCODE_VOLUME_DOWN = 0x42;
const Int32 HdmiCecKeycode::CEC_KEYCODE_MUTE = 0x43;
const Int32 HdmiCecKeycode::CEC_KEYCODE_PLAY = 0x44;
const Int32 HdmiCecKeycode::CEC_KEYCODE_STOP = 0x45;
const Int32 HdmiCecKeycode::CEC_KEYCODE_PAUSE = 0x46;
const Int32 HdmiCecKeycode::CEC_KEYCODE_RECORD = 0x47;
const Int32 HdmiCecKeycode::CEC_KEYCODE_REWIND = 0x48;
const Int32 HdmiCecKeycode::CEC_KEYCODE_FAST_FORWARD = 0x49;
const Int32 HdmiCecKeycode::CEC_KEYCODE_EJECT = 0x4A;
const Int32 HdmiCecKeycode::CEC_KEYCODE_FORWARD = 0x4B;
const Int32 HdmiCecKeycode::CEC_KEYCODE_BACKWARD = 0x4C;
const Int32 HdmiCecKeycode::CEC_KEYCODE_STOP_RECORD = 0x4D;
const Int32 HdmiCecKeycode::CEC_KEYCODE_PAUSE_RECORD = 0x4E;
const Int32 HdmiCecKeycode::CEC_KEYCODE_RESERVED = 0x4F;
const Int32 HdmiCecKeycode::CEC_KEYCODE_ANGLE = 0x50;
const Int32 HdmiCecKeycode::CEC_KEYCODE_SUB_PICTURE = 0x51;
const Int32 HdmiCecKeycode::CEC_KEYCODE_VIDEO_ON_DEMAND = 0x52;
const Int32 HdmiCecKeycode::CEC_KEYCODE_ELECTRONIC_PROGRAM_GUIDE = 0x53;
const Int32 HdmiCecKeycode::CEC_KEYCODE_TIMER_PROGRAMMING = 0x54;
const Int32 HdmiCecKeycode::CEC_KEYCODE_INITIAL_CONFIGURATION = 0x55;
const Int32 HdmiCecKeycode::CEC_KEYCODE_SELECT_BROADCAST_TYPE = 0x56;
const Int32 HdmiCecKeycode::CEC_KEYCODE_SELECT_SOUND_PRESENTATION = 0x57;
const Int32 HdmiCecKeycode::CEC_KEYCODE_PLAY_FUNCTION = 0x60;
const Int32 HdmiCecKeycode::CEC_KEYCODE_PAUSE_PLAY_FUNCTION = 0x61;
const Int32 HdmiCecKeycode::CEC_KEYCODE_RECORD_FUNCTION = 0x62;
const Int32 HdmiCecKeycode::CEC_KEYCODE_PAUSE_RECORD_FUNCTION = 0x63;
const Int32 HdmiCecKeycode::CEC_KEYCODE_STOP_FUNCTION = 0x64;
const Int32 HdmiCecKeycode::CEC_KEYCODE_MUTE_FUNCTION = 0x65;
const Int32 HdmiCecKeycode::CEC_KEYCODE_RESTORE_VOLUME_FUNCTION = 0x66;
const Int32 HdmiCecKeycode::CEC_KEYCODE_TUNE_FUNCTION = 0x67;
const Int32 HdmiCecKeycode::CEC_KEYCODE_SELECT_MEDIA_FUNCTION = 0x68;
const Int32 HdmiCecKeycode::CEC_KEYCODE_SELECT_AV_INPUT_FUNCTION = 0x69;
const Int32 HdmiCecKeycode::CEC_KEYCODE_SELECT_AUDIO_INPUT_FUNCTION = 0x6A;
const Int32 HdmiCecKeycode::CEC_KEYCODE_POWER_TOGGLE_FUNCTION = 0x6B;
const Int32 HdmiCecKeycode::CEC_KEYCODE_POWER_OFF_FUNCTION = 0x6C;
const Int32 HdmiCecKeycode::CEC_KEYCODE_POWER_ON_FUNCTION = 0x6D;
const Int32 HdmiCecKeycode::CEC_KEYCODE_F1_BLUE = 0x71;
const Int32 HdmiCecKeycode::CEC_KEYCODE_F2_RED = 0x72;
const Int32 HdmiCecKeycode::CEC_KEYCODE_F3_GREEN = 0x73;
const Int32 HdmiCecKeycode::CEC_KEYCODE_F4_YELLOW = 0x74;
const Int32 HdmiCecKeycode::CEC_KEYCODE_F5 = 0x75;
const Int32 HdmiCecKeycode::CEC_KEYCODE_DATA = 0x76;
const Int32 HdmiCecKeycode::UI_BROADCAST_TOGGLE_ALL = 0x00;
const Int32 HdmiCecKeycode::UI_BROADCAST_TOGGLE_ANALOGUE_DIGITAL = 0x01;
const Int32 HdmiCecKeycode::UI_BROADCAST_ANALOGUE = 0x10;
const Int32 HdmiCecKeycode::UI_BROADCAST_ANALOGUE_TERRESTRIAL = 0x20;
const Int32 HdmiCecKeycode::UI_BROADCAST_ANALOGUE_CABLE = 0x30;
const Int32 HdmiCecKeycode::UI_BROADCAST_ANALOGUE_SATELLITE = 0x40;
const Int32 HdmiCecKeycode::UI_BROADCAST_DIGITAL = 0x50;
const Int32 HdmiCecKeycode::UI_BROADCAST_DIGITAL_TERRESTRIAL = 0x60;
const Int32 HdmiCecKeycode::UI_BROADCAST_DIGITAL_CABLE = 0x70;
const Int32 HdmiCecKeycode::UI_BROADCAST_DIGITAL_SATELLITE = 0x80;
const Int32 HdmiCecKeycode::UI_BROADCAST_DIGITAL_COMMNICATIONS_SATELLITE = 0x90;
const Int32 HdmiCecKeycode::UI_BROADCAST_DIGITAL_COMMNICATIONS_SATELLITE_2 = 0x91;
const Int32 HdmiCecKeycode::UI_BROADCAST_IP = 0xA0;
const Int32 HdmiCecKeycode::UI_SOUND_PRESENTATION_SOUND_MIX_DUAL_MONO = 0x20;
const Int32 HdmiCecKeycode::UI_SOUND_PRESENTATION_SOUND_MIX_KARAOKE = 0x30;
const Int32 HdmiCecKeycode::UI_SOUND_PRESENTATION_SELECT_AUDIO_DOWN_MIX = 0x80;
const Int32 HdmiCecKeycode::UI_SOUND_PRESENTATION_SELECT_AUDIO_AUTO_REVERBERATION = 0x90;
const Int32 HdmiCecKeycode::UI_SOUND_PRESENTATION_SELECT_AUDIO_AUTO_EQUALIZER = 0xA0;
const Int32 HdmiCecKeycode::UI_SOUND_PRESENTATION_BASS_STEP_PLUS = 0xB1;
const Int32 HdmiCecKeycode::UI_SOUND_PRESENTATION_BASS_NEUTRAL = 0xB2;
const Int32 HdmiCecKeycode::UI_SOUND_PRESENTATION_BASS_STEP_MINUS = 0xB3;
const Int32 HdmiCecKeycode::UI_SOUND_PRESENTATION_TREBLE_STEP_PLUS = 0xC1;
const Int32 HdmiCecKeycode::UI_SOUND_PRESENTATION_TREBLE_NEUTRAL = 0xC2;
const Int32 HdmiCecKeycode::UI_SOUND_PRESENTATION_TREBLE_STEP_MINUS = 0xC3;
const AutoPtr<ArrayOf<HdmiCecKeycode::KeycodeEntry*> > HdmiCecKeycode::KEYCODE_ENTRIES = InitKEYCODE_ENTRIES();

HdmiCecKeycode::HdmiCecKeycode()
{}

ECode HdmiCecKeycode::ElastosKeyToCecKey(
    /* [in] */ Int32 keycode,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    for (Int32 i = 0; i < KEYCODE_ENTRIES->GetLength(); ++i) {
        Int32 cecKeycode = (*KEYCODE_ENTRIES)[i]->ToCecKeycodeIfMatched(keycode);
        if (cecKeycode != UNSUPPORTED_KEYCODE) {
            *result = cecKeycode;
            return NOERROR;
        }
    }
    *result = UNSUPPORTED_KEYCODE;
    return NOERROR;
}

ECode HdmiCecKeycode::CecKeyToElastosKey(
    /* [in] */ Int32 keycode,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    for (Int32 i = 0; i < KEYCODE_ENTRIES->GetLength(); ++i) {
        Int32 androidKey = (*KEYCODE_ENTRIES)[i]->ToElastosKeycodeIfMatched(keycode);
        if (androidKey != UNSUPPORTED_KEYCODE) {
            *result = androidKey;
            return NOERROR;
        }
    }
    *result = UNSUPPORTED_KEYCODE;
    return NOERROR;
}

ECode HdmiCecKeycode::IsRepeatableKey(
    /* [in] */ Int32 androidKeycode,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    for (Int32 i = 0; i < KEYCODE_ENTRIES->GetLength(); ++i) {
        AutoPtr<IBoolean> isRepeatable = (*KEYCODE_ENTRIES)[i]->IsRepeatableIfMatched(androidKeycode);
        if (isRepeatable != NULL) {
            return isRepeatable->GetValue(result);
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode HdmiCecKeycode::IsSupportedKeycode(
    /* [in] */ Int32 androidKeycode,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 cecKey;
    HdmiCecKeycode::ElastosKeyToCecKey(androidKeycode, &cecKey);
    *result = cecKey != HdmiCecKeycode::UNSUPPORTED_KEYCODE;
    return NOERROR;
}

AutoPtr<ArrayOf<HdmiCecKeycode::KeycodeEntry*> > HdmiCecKeycode::InitKEYCODE_ENTRIES()
{
    KeycodeEntry* tmp[] = {
        new KeycodeEntry(IKeyEvent::KEYCODE_DPAD_CENTER, CEC_KEYCODE_SELECT),
        new KeycodeEntry(IKeyEvent::KEYCODE_DPAD_UP, CEC_KEYCODE_UP),
        new KeycodeEntry(IKeyEvent::KEYCODE_DPAD_DOWN, CEC_KEYCODE_DOWN),
        new KeycodeEntry(IKeyEvent::KEYCODE_DPAD_LEFT, CEC_KEYCODE_LEFT),
        new KeycodeEntry(IKeyEvent::KEYCODE_DPAD_RIGHT, CEC_KEYCODE_RIGHT),
        // No Android keycode defined for CEC_KEYCODE_RIGHT_UP
        new KeycodeEntry(UNSUPPORTED_KEYCODE, CEC_KEYCODE_RIGHT_UP),
        // No Android keycode defined for CEC_KEYCODE_RIGHT_DOWN
        new KeycodeEntry(UNSUPPORTED_KEYCODE, CEC_KEYCODE_RIGHT_DOWN),
        // No Android keycode defined for CEC_KEYCODE_LEFT_UP
        new KeycodeEntry(UNSUPPORTED_KEYCODE, CEC_KEYCODE_LEFT_UP),
        // No Android keycode defined for CEC_KEYCODE_LEFT_DOWN
        new KeycodeEntry(UNSUPPORTED_KEYCODE, CEC_KEYCODE_LEFT_DOWN),
        new KeycodeEntry(IKeyEvent::KEYCODE_HOME, CEC_KEYCODE_ROOT_MENU, FALSE),
        new KeycodeEntry(IKeyEvent::KEYCODE_SETTINGS, CEC_KEYCODE_SETUP_MENU, FALSE),
        new KeycodeEntry(IKeyEvent::KEYCODE_MENU, CEC_KEYCODE_CONTENTS_MENU, FALSE),
        // No Android keycode defined for CEC_KEYCODE_FAVORITE_MENU
        new KeycodeEntry(UNSUPPORTED_KEYCODE, CEC_KEYCODE_FAVORITE_MENU),
        new KeycodeEntry(IKeyEvent::KEYCODE_BACK, CEC_KEYCODE_EXIT),
        // RESERVED
        new KeycodeEntry(IKeyEvent::KEYCODE_MEDIA_TOP_MENU, CEC_KEYCODE_MEDIA_TOP_MENU),
        // No Android keycode defined for CEC_KEYCODE_MEDIA_CONTEXT_SENSITIVE_MENU
        new KeycodeEntry(UNSUPPORTED_KEYCODE, CEC_KEYCODE_MEDIA_CONTEXT_SENSITIVE_MENU),
        // RESERVED
        // No Android keycode defined for CEC_KEYCODE_NUMBER_ENTRY_MODE
        new KeycodeEntry(UNSUPPORTED_KEYCODE, CEC_KEYCODE_NUMBER_ENTRY_MODE),
        new KeycodeEntry(IKeyEvent::KEYCODE_11, CEC_KEYCODE_NUMBER_11),
        new KeycodeEntry(IKeyEvent::KEYCODE_12, CEC_KEYCODE_NUMBER_12),
        new KeycodeEntry(IKeyEvent::KEYCODE_0, CEC_KEYCODE_NUMBER_0_OR_NUMBER_10),
        new KeycodeEntry(IKeyEvent::KEYCODE_1, CEC_KEYCODE_NUMBERS_1),
        new KeycodeEntry(IKeyEvent::KEYCODE_2, CEC_KEYCODE_NUMBERS_2),
        new KeycodeEntry(IKeyEvent::KEYCODE_3, CEC_KEYCODE_NUMBERS_3),
        new KeycodeEntry(IKeyEvent::KEYCODE_4, CEC_KEYCODE_NUMBERS_4),
        new KeycodeEntry(IKeyEvent::KEYCODE_5, CEC_KEYCODE_NUMBERS_5),
        new KeycodeEntry(IKeyEvent::KEYCODE_6, CEC_KEYCODE_NUMBERS_6),
        new KeycodeEntry(IKeyEvent::KEYCODE_7, CEC_KEYCODE_NUMBERS_7),
        new KeycodeEntry(IKeyEvent::KEYCODE_8, CEC_KEYCODE_NUMBERS_8),
        new KeycodeEntry(IKeyEvent::KEYCODE_9, CEC_KEYCODE_NUMBERS_9),
        new KeycodeEntry(IKeyEvent::KEYCODE_PERIOD, CEC_KEYCODE_DOT),
        new KeycodeEntry(IKeyEvent::KEYCODE_NUMPAD_ENTER, CEC_KEYCODE_ENTER),
        new KeycodeEntry(IKeyEvent::KEYCODE_CLEAR, CEC_KEYCODE_CLEAR),
        // RESERVED
        // No Android keycode defined for CEC_KEYCODE_NEXT_FAVORITE
        new KeycodeEntry(UNSUPPORTED_KEYCODE, CEC_KEYCODE_NEXT_FAVORITE),
        new KeycodeEntry(IKeyEvent::KEYCODE_CHANNEL_UP, CEC_KEYCODE_CHANNEL_UP),
        new KeycodeEntry(IKeyEvent::KEYCODE_CHANNEL_DOWN, CEC_KEYCODE_CHANNEL_DOWN),
        new KeycodeEntry(IKeyEvent::KEYCODE_LAST_CHANNEL, CEC_KEYCODE_PREVIOUS_CHANNEL),
        // No Android keycode defined for CEC_KEYCODE_SOUND_SELECT
        new KeycodeEntry(UNSUPPORTED_KEYCODE, CEC_KEYCODE_SOUND_SELECT),
        new KeycodeEntry(IKeyEvent::KEYCODE_TV_INPUT, CEC_KEYCODE_INPUT_SELECT),
        new KeycodeEntry(IKeyEvent::KEYCODE_INFO, CEC_KEYCODE_DISPLAY_INFORMATION),
        // No Android keycode defined for CEC_KEYCODE_HELP
        new KeycodeEntry(UNSUPPORTED_KEYCODE, CEC_KEYCODE_HELP),
        new KeycodeEntry(IKeyEvent::KEYCODE_PAGE_UP, CEC_KEYCODE_PAGE_UP),
        new KeycodeEntry(IKeyEvent::KEYCODE_PAGE_DOWN, CEC_KEYCODE_PAGE_DOWN),
        // RESERVED
        new KeycodeEntry(IKeyEvent::KEYCODE_POWER, CEC_KEYCODE_POWER, FALSE),
        new KeycodeEntry(IKeyEvent::KEYCODE_VOLUME_UP, CEC_KEYCODE_VOLUME_UP),
        new KeycodeEntry(IKeyEvent::KEYCODE_VOLUME_DOWN, CEC_KEYCODE_VOLUME_DOWN),
        new KeycodeEntry(IKeyEvent::KEYCODE_VOLUME_MUTE, CEC_KEYCODE_MUTE, FALSE),
        new KeycodeEntry(IKeyEvent::KEYCODE_MEDIA_PLAY, CEC_KEYCODE_PLAY),
        new KeycodeEntry(IKeyEvent::KEYCODE_MEDIA_STOP, CEC_KEYCODE_STOP),
        new KeycodeEntry(IKeyEvent::KEYCODE_MEDIA_PAUSE, CEC_KEYCODE_PAUSE),
        new KeycodeEntry(IKeyEvent::KEYCODE_MEDIA_RECORD, CEC_KEYCODE_RECORD),
        new KeycodeEntry(IKeyEvent::KEYCODE_MEDIA_REWIND, CEC_KEYCODE_REWIND),
        new KeycodeEntry(IKeyEvent::KEYCODE_MEDIA_FAST_FORWARD, CEC_KEYCODE_FAST_FORWARD),
        new KeycodeEntry(IKeyEvent::KEYCODE_MEDIA_EJECT, CEC_KEYCODE_EJECT),
        new KeycodeEntry(IKeyEvent::KEYCODE_MEDIA_NEXT, CEC_KEYCODE_FORWARD),
        new KeycodeEntry(IKeyEvent::KEYCODE_MEDIA_PREVIOUS, CEC_KEYCODE_BACKWARD),
        // No Android keycode defined for CEC_KEYCODE_STOP_RECORD
        new KeycodeEntry(UNSUPPORTED_KEYCODE, CEC_KEYCODE_STOP_RECORD),
        // No Android keycode defined for CEC_KEYCODE_PAUSE_RECORD
        new KeycodeEntry(UNSUPPORTED_KEYCODE, CEC_KEYCODE_PAUSE_RECORD),
        // No Android keycode defined for CEC_KEYCODE_RESERVED
        new KeycodeEntry(UNSUPPORTED_KEYCODE, CEC_KEYCODE_RESERVED),
        // No Android keycode defined for CEC_KEYCODE_ANGLE
        new KeycodeEntry(UNSUPPORTED_KEYCODE, CEC_KEYCODE_ANGLE),
        // No Android keycode defined for CEC_KEYCODE_SUB_PICTURE
        new KeycodeEntry(UNSUPPORTED_KEYCODE, CEC_KEYCODE_SUB_PICTURE),
        // No Android keycode defined for CEC_KEYCODE_VIDEO_ON_DEMAND
        new KeycodeEntry(UNSUPPORTED_KEYCODE, CEC_KEYCODE_VIDEO_ON_DEMAND),
        new KeycodeEntry(IKeyEvent::KEYCODE_GUIDE, CEC_KEYCODE_ELECTRONIC_PROGRAM_GUIDE),
        // No Android keycode defined for CEC_KEYCODE_TIMER_PROGRAMMING
        new KeycodeEntry(UNSUPPORTED_KEYCODE, CEC_KEYCODE_TIMER_PROGRAMMING),
        // No Android keycode defined for CEC_KEYCODE_INITIAL_CONFIGURATION
        new KeycodeEntry(UNSUPPORTED_KEYCODE, CEC_KEYCODE_INITIAL_CONFIGURATION),
        // No Android keycode defined for CEC_KEYCODE_SELECT_BROADCAST_TYPE
        new KeycodeEntry(UNSUPPORTED_KEYCODE, CEC_KEYCODE_SELECT_BROADCAST_TYPE),
        // No Android keycode defined for CEC_KEYCODE_SELECT_SOUND_PRESENTATION
        new KeycodeEntry(UNSUPPORTED_KEYCODE, CEC_KEYCODE_SELECT_SOUND_PRESENTATION),
        // RESERVED
        // The following deterministic key definitions do not need key mapping
        // since they are supposed to be generated programmatically only.
        // No Android keycode defined for CEC_KEYCODE_PLAY_FUNCTION
        new KeycodeEntry(UNSUPPORTED_KEYCODE, CEC_KEYCODE_PLAY_FUNCTION),
        // No Android keycode defined for CEC_KEYCODE_PAUSE_PLAY_FUNCTION
        new KeycodeEntry(UNSUPPORTED_KEYCODE, CEC_KEYCODE_PAUSE_PLAY_FUNCTION),
        // No Android keycode defined for CEC_KEYCODE_RECORD_FUNCTION
        new KeycodeEntry(UNSUPPORTED_KEYCODE, CEC_KEYCODE_RECORD_FUNCTION),
        // No Android keycode defined for CEC_KEYCODE_PAUSE_RECORD_FUNCTION
        new KeycodeEntry(UNSUPPORTED_KEYCODE, CEC_KEYCODE_PAUSE_RECORD_FUNCTION),
        // No Android keycode defined for CEC_KEYCODE_STOP_FUNCTION
        new KeycodeEntry(UNSUPPORTED_KEYCODE, CEC_KEYCODE_STOP_FUNCTION),
        // No Android keycode defined for CEC_KEYCODE_MUTE_FUNCTION
        new KeycodeEntry(UNSUPPORTED_KEYCODE, CEC_KEYCODE_MUTE_FUNCTION, FALSE),
        // No Android keycode defined for CEC_KEYCODE_RESTORE_VOLUME_FUNCTION
        new KeycodeEntry(UNSUPPORTED_KEYCODE, CEC_KEYCODE_RESTORE_VOLUME_FUNCTION, FALSE),
        // No Android keycode defined for CEC_KEYCODE_TUNE_FUNCTION
        new KeycodeEntry(UNSUPPORTED_KEYCODE, CEC_KEYCODE_TUNE_FUNCTION),
        // No Android keycode defined for CEC_KEYCODE_SELECT_MEDIA_FUNCTION
        new KeycodeEntry(UNSUPPORTED_KEYCODE, CEC_KEYCODE_SELECT_MEDIA_FUNCTION),
        // No Android keycode defined for CEC_KEYCODE_SELECT_AV_INPUT_FUNCTION
        new KeycodeEntry(UNSUPPORTED_KEYCODE, CEC_KEYCODE_SELECT_AV_INPUT_FUNCTION),
        // No Android keycode defined for CEC_KEYCODE_SELECT_AUDIO_INPUT_FUNCTION
        new KeycodeEntry(UNSUPPORTED_KEYCODE, CEC_KEYCODE_SELECT_AUDIO_INPUT_FUNCTION),
        // No Android keycode defined for CEC_KEYCODE_POWER_TOGGLE_FUNCTION
        new KeycodeEntry(UNSUPPORTED_KEYCODE, CEC_KEYCODE_POWER_TOGGLE_FUNCTION),
        // No Android keycode defined for CEC_KEYCODE_POWER_OFF_FUNCTION
        new KeycodeEntry(UNSUPPORTED_KEYCODE, CEC_KEYCODE_POWER_OFF_FUNCTION),
        // No Android keycode defined for CEC_KEYCODE_POWER_ON_FUNCTION
        new KeycodeEntry(UNSUPPORTED_KEYCODE, CEC_KEYCODE_POWER_ON_FUNCTION, FALSE),
        // RESERVED
        new KeycodeEntry(IKeyEvent::KEYCODE_PROG_BLUE, CEC_KEYCODE_F1_BLUE),
        new KeycodeEntry(IKeyEvent::KEYCODE_PROG_RED, CEC_KEYCODE_F2_RED),
        new KeycodeEntry(IKeyEvent::KEYCODE_PROG_GREEN, CEC_KEYCODE_F3_GREEN),
        new KeycodeEntry(IKeyEvent::KEYCODE_PROG_YELLOW, CEC_KEYCODE_F4_YELLOW),
        new KeycodeEntry(IKeyEvent::KEYCODE_F5, CEC_KEYCODE_F5),
        new KeycodeEntry(IKeyEvent::KEYCODE_TV_DATA_SERVICE, CEC_KEYCODE_DATA),
        // RESERVED
        // Add a new key mapping here if new keycode is introduced.
    };
    Int32 size = ArraySize(tmp);
    AutoPtr<ArrayOf<KeycodeEntry*> > rev = ArrayOf<KeycodeEntry*>::Alloc(size);
    rev->Copy(tmp, size);
    return rev;
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
