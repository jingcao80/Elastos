//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __ELASTOS_VKEY_H__
#define __ELASTOS_VKEY_H__

//
// Virtual Keys, Standard Set
//

#define VK_LBUTTON          0x01        // Left mouse button
#define VK_RBUTTON          0x02        // Right mouse button
#define VK_CANCEL           0x03        // Control-break processing
#define VK_MBUTTON          0x04        // Middle mouse button on a
                                        // three-button mouse

#define VK_BACK             0x08        // BACKSPACE key
#define VK_TAB              0x09        // TAB key

#define VK_CLEAR            0x0C        // CLEAR key    //? what's the key?
#define VK_RETURN           0x0D        // ENTER key

#define VK_SHIFT            0x10        // SHIFT key
#define VK_CONTROL          0x11        // CTRL key
#define VK_MENU             0x12        // ALT key
#define VK_PAUSE            0x13        // PAUSE key
#define VK_CAPITAL          0x14        // CAPS LOCK key

#define VK_KANA             0x15
#define VK_HANGEUL          0x15
#define VK_HANGUL           0x15
#define VK_JUNJA            0x17
#define VK_FINAL            0x18
#define VK_HANJA            0x19
#define VK_KANJI            0x19

#define VK_ESCAPE           0x1B    // ESC key

#define VK_CONVERT          0x1C
#define VK_NONCONVERT       0x1D
#define VK_ACCEPT           0x1E
#define VK_MODECHANGE       0x1F

#define VK_SPACE            0x20    // SPACEBAR
#define VK_PRIOR            0x21    // PAGE UP key
#define VK_NEXT             0x22    // PAGE DOWN key
#define VK_END              0x23    // END key
#define VK_HOME             0x24    // HOME key
#define VK_LEFT             0x25    // LEFT ARROW key
#define VK_UP               0x26    // UP ARROW key
#define VK_RIGHT            0x27    // RIGHT ARROW key
#define VK_DOWN             0x28    // DOWN ARROW key
#define VK_SELECT           0x29    // SELECT key

#define VK_ASTERISK         0x2A    //'*'key
#define VK_POUND            0x2B    //'#'key

#define VK_PRINT            0x2A

#define VK_EXECUTE          0x2B    // EXECUTE key
#define VK_SNAPSHOT         0x2C    // PRINT SCREEN key
#define VK_INSERT           0x2D    // INS key
#define VK_DELETE           0x2E    // DEL key
#define VK_HELP             0x2F    // HELP key

#define VK_0                0x30    // 0 key
#define VK_1                0x31    // 1 key
#define VK_2                0x32    // 2 key
#define VK_3                0x33    // 3 key
#define VK_4                0x34    // 4 key
#define VK_5                0x35    // 5 key
#define VK_6                0x36    // 6 key
#define VK_7                0x37    // 7 key
#define VK_8                0x38    // 8 key
#define VK_9                0x39    // 9 key

#define VK_A                0x41    // A key
#define VK_B                0x42    // B key
#define VK_C                0x43    // C key
#define VK_D                0x44    // D key
#define VK_E                0x45    // E key
#define VK_F                0x46    // F key
#define VK_G                0x47    // G key
#define VK_H                0x48    // H key
#define VK_I                0x49    // I key
#define VK_J                0x4A    // J key
#define VK_K                0x4B    // K key
#define VK_L                0x4C    // L key
#define VK_M                0x4D    // M key
#define VK_N                0x4E    // N key
#define VK_O                0x4F    // O key
#define VK_P                0x50    // P key
#define VK_Q                0x51    // Q key
#define VK_R                0x52    // R key
#define VK_S                0x53    // S key
#define VK_T                0x54    // T key
#define VK_U                0x55    // U key
#define VK_V                0x56    // V key
#define VK_W                0x57    // W key
#define VK_X                0x58    // X key
#define VK_Y                0x59    // Y key
#define VK_Z                0x5A    // Z key

#define VK_LWIN             0x5B    // Left Windows key
#define VK_RWIN             0x5C    // Right Windows key
#define VK_APPS             0x5D    // Applications key

#define VK_Power            0x5E    // Power
#define VK_Sleep            0x5F    // Sleep
#define VK_Lock             0x5F    // Lock Sreen

// Numeric keypad
#define VK_NUMPAD0          0x60    // 0 key
#define VK_NUMPAD1          0x61    // 1 key
#define VK_NUMPAD2          0x62    // 2 key
#define VK_NUMPAD3          0x63    // 3 key
#define VK_NUMPAD4          0x64    // 4 key
#define VK_NUMPAD5          0x65    // 5 key
#define VK_NUMPAD6          0x66    // 6 key
#define VK_NUMPAD7          0x67    // 7 key
#define VK_NUMPAD8          0x68    // 8 key
#define VK_NUMPAD9          0x69    // 9 key

#define VK_MULTIPLY         0x6A    // Multiply key     '*'
#define VK_ADD              0x6B    // Add key          '+'
#define VK_SEPARATOR        0x6C    // Separator key
#define VK_SUBTRACT         0x6D    // Subtract key     '-'
#define VK_DECIMAL          0x6E    // Decimal key      '.'
#define VK_DIVIDE           0x6F    // Divide key       '/'

#define VK_F1               0x70    // F1 key
#define VK_F2               0x71    // F2 key
#define VK_F3               0x72    // F3 key
#define VK_F4               0x73    // F4 key
#define VK_F5               0x74    // F5 key
#define VK_F6               0x75    // F6 key
#define VK_F7               0x76    // F7 key
#define VK_F8               0x77    // F8 key
#define VK_F9               0x78    // F9 key
#define VK_F10              0x79    // F10 key
#define VK_F11              0x7A    // F11 key
#define VK_F12              0x7B    // F12 key
#define VK_F13              0x7C    // F13 key
#define VK_F14              0x7D    // F14 key
#define VK_F15              0x7E    // F15 key
#define VK_F16              0x7F    // F16 key
#define VK_F17              0x80    // F17 key
#define VK_F18              0x81    // F18 key
#define VK_F19              0x82    // F19 key
#define VK_F20              0x83    // F20 key
#define VK_F21              0x84    // F21 key
#define VK_F22              0x85    // F22 key
#define VK_F23              0x86    // F23 key
#define VK_F24              0x87    // F24 key

#define VK_PHONE_CALL       0x88    // call
#define VK_PHONE_HANG_UP    0x89    // hang up
#define VK_PHONE_SELECT_LEFT   0x8A // select left
#define VK_PHONE_SELECT_RIGHT  0x8B // select right
#define VK_CAMERA           0x8C
#define VK_RECORDER         0x8D
#define VK_AUTOFOCUS        0x8E
#define VK_LINECONTROL     0x8F    // button at headset line

#define VK_NUMLOCK          0x90    // NUM LOCK key
#define VK_SCROLL           0x91    // SCROLL LOCK key

#define VK_LSHIFT           0xA0    // Left SHIFT
#define VK_RSHIFT           0xA1    // Right SHIFT
#define VK_LCONTROL         0xA2    // Left CTRL
#define VK_RCONTROL         0xA3    // Right CTRL
#define VK_LMENU            0xA4    // Left ALT
#define VK_RMENU            0xA5    // Right ALT

#define VK_VOLUME_MUTE      0xAD    // Volume Mute key
#define VK_VOLUME_DOWN      0xAE    // Volume Down key
#define VK_VOLUME_UP        0xAF    // Volume Up key

#define VK_MEDIA_NEXT_TRACK 0xB0    // Next Track key
#define VK_MEDIA_PREV_TRACK 0xB1    // Previous Track key
#define VK_MEDIA_STOP       0xB2    // Stop Media key
#define VK_MEDIA_PLAY_PAUSE 0xB3    // Play/Pause Media key

#define VK_SEMICOLON        0xBA    // ; key
#define VK_EQUAL            0xBB    // = key
#define VK_COMMA            0xBC    // , key
#define VK_HYPHEN           0xBD    // - key
#define VK_PERIOD           0xBE    // . key
#define VK_SLASH            0xBF    // / key
#define VK_BACKQUOTE        0xC0    // ` key
#define VK_LBRACKET         0xDB    // [ key
#define VK_BACKSLASH        0xDC    // \ key
#define VK_RBRACKET         0xDD    // ] key
#define VK_APOSTROPHE       0xDE    // ' key

#define VK_NOSUPPORT        0xDF
#define VK_ERROR            0xDF

#define VK_TD_POWEROFF      0xEB    //Time to poweroff td
#define VK_LCD_ON           0xEC    // Send when LCD is off
#define VK_SWITCH_OFF       0xED    // Modem switch off
#define VK_SWITCH_ON        0xEE    // Modem switch on
#define VK_WakeUp           0xEF    // Wake Up

#define VK_RESERVED         0xFF    // Reserved
#endif // __ELASTOS_VKEY_H__
