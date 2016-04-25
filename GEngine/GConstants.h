#pragma once

#include "GDefines.h"

#ifndef GCONSTANTS_H
#define GCONSTANTS_H

//Название
#define G_CLASS_NAME "DeEngine"
#define G_TITLE_NAME "Depthern Engine 0.1.9 pre-alpha"

//Максимально допустимая длинна имен
#define G_MAX_NAME_LEN       256

//Максимальное кол-во строк в логе
#define G_MAX_LOG_ROWS       256

//Максимальный размер истории ввода консоли
#define G_MAX_CONSOLE_HISTORY_SIZE 30

//Максимальные/минимальные константы
#define G_MAX_INT            0x7FFFFFFF
#define G_MIN_INT            0x80000000
#define G_MAX_UINT           0xFFFFFFFF

//Previous Key State Bit
#define G_KEYSTATEDOWN       0x40000000

//UI Top Z Order
#define G_TOP_ZORDER         0xFFFF

//UI Border Size
#define G_UI_DEF_BORDER_SIZE      5
#define G_UI_WINDOW_BORDER_SIZE   7
#define G_UI_WINDOW_CAPTION_SIZE  27

//UI Update Interval (ms)
#define G_UI_UPDATE_INTERVAL          40
#define G_UI_REGULAR_UPDATE_INTERVAL  100
#define G_UI_SCROLL_INTERVAL          100

//UI Default Texture Offsets
const g_vec2 G_UI_DEF_TEXOFFSET[6] = {g_vec2(0, 0),
                                      g_vec2(0, 85.0f/256.0f),
                                      g_vec2(0, 170.0f/256.0f),
                                      g_vec2(0, 0.5f),
                                      g_vec2(0.5f, 0),
                                      g_vec2(0.5f, 0.5f)};

//UI Maximum Icon Size
#define G_UI_MAX_ICON_SIZE  64

//UI Length Constants
const g_uint32  G_UI_MAX_HINT_LENGTH     = 255;
const g_uint32  G_UI_MAX_CAPTION_LENGTH  = 255;

//UI Z-Order
const g_uint8   G_UI_DEF_ZORD_LEVEL      = 128;

//Anchors
enum g_anchors_state {g_anst_center, g_anst_low, g_anst_high, g_anst_both};

//Число Пи
const g_float G_PI = 3.141593f;
const g_float G_PI_2 = 1.570796f;
const g_float G_PI_180 = 0.0174533f;
const g_float G_PI_360 = 0.00872665f;

//Эпсилон
#define G_F_EPS     FLT_EPSILON
#define G_D_EPS     DBL_EPSILON
#define G_HI_EPS    0.0001f

//Clear Colors
const g_color def_clearcolor = g_color(0.0f, 0.125f, 0.6f, 1.0f);
const g_float def_cleardepth = 1.0f;
const g_uint8 def_clearstencil = 0;

//NAN
const unsigned long l_nan[2] = {0xffffffff, 0x7fffffff};
const double NAN = *(double*)l_nan;

// Типы объектов
#define G_OT_EMPTY           0
#define G_OT_FONT            1
#define G_OT_SPRITE          2
#define G_OT_MATERIAL        3
#define G_OT_EFFECT          4
#define G_OT_TEXTURE         5
#define G_OT_CAMERA          6
#define G_OT_EFFECTPOOL      7
#define G_OT_MODEL           8

//Типы объектов UI
#define G_OT_UI_EMPTY        100
#define G_OT_UI_WINDOW       101
#define G_OT_UI_BUTTON       102
#define G_OT_UI_EDIT         103
#define G_OT_UI_LABEL        104
#define G_OT_UI_PROGRESSBAR  105
#define G_OT_UI_CHECKBOX     106
#define G_OT_UI_RADIOBUTTON  107
#define G_OT_UI_LISTBOX      108
#define G_OT_UI_COMBOBOX     109
#define G_OT_UI_IMAGE        110
#define G_OT_UI_MEMO         111
#define G_OT_UI_PAGES        112
#define G_OT_UI_SCROLLBAR    113
#define G_OT_UI_SCROLLLIST   114
#define G_OT_UI_TRACKBAR     115
#define G_OT_UI_BUTTONGRID   116
#define G_OT_UI_DESKTOP      117
#define G_OT_UI_PANEL        118

//Common Event Messages
#define G_MSG_EMPTY          0
#define G_MSG_ONMOUSEDOWN    1
#define G_MSG_ONMOUSEUP      2
#define G_MSG_ONCLICK        3
#define G_MSG_ONMOUSEENTER   4
#define G_MSG_ONMOUSELEAVE   5
#define G_MSG_ONMOUSEMOVE    6
#define G_MSG_ONKEYDOWN      7
#define G_MSG_ONKEYUP        8
#define G_MSG_ONKEYPRESS     9
#define G_MSG_ONCHAR         10
#define G_MSG_ONFOCUSSET     11
#define G_MSG_ONFOCUSLOST    12
#define G_MSG_ONSHOW         13
#define G_MSG_ONHIDE         14
#define G_MSG_ONRESIZE       15
#define G_MSG_ONSETPOS       16
#define G_MSG_ONDRAW         17

//Custom Event Messages
#define G_MSG_ONSELECTITEM   100
#define G_MSG_ONSETVALUE     101

//Buffer Types
#define G_BT_VERTEX          1
#define G_BT_INDEX           2
#define G_BT_CONSTANT        4
#define G_BT_SHADERRES       8

//Buffer Flags
#define G_BF_DEFAULT         1
#define G_BF_DYNAMIC         2
#define G_BF_IMMUTABLE       3

//Primitive Topology
#define G_PT_TRIANGLE_LIST   4
#define G_PT_LINE_LIST       2
#define G_PT_POINT_LIST      1

//Типы шейдерных переменных
#define G_SVT_CUSTOM         0
#define G_SVT_MWORLD         1
#define G_SVT_MPROJ          2
#define G_SVT_MVIEW          3
#define G_SVT_LIGHT          4
#define G_SVT_RESOURCE       5
#define G_SVT_MVP            6

//Типы шейдерных техник

//Cooperative Levels
#define G_EXCLUSIVE          0
#define G_NONEXCLUSIVE       1

//Config Files UIDs
#define G_CUID_GAMECONFIG    252
#define G_CUID_CUBE_TERRAIN  253
#define G_CUID_DEVICECONFIG  254
#define G_CUID_UICONFIG      255

//Default Path
#define G_PATH_TEXS  "..\\Textures";
#define G_PATH_MAPS  "..\\Maps";
#define G_PATH_SHDR  "..\\Shaders";
#define G_PATH_BIN   "..\\Bin";

#define G_DEFPATH_UICONF   "..\\Settings\\UISet.conf"
#define G_DEFPATH_GAMECONF "..\\Settings\\GameSet.conf"

//Object Align Flags
#define G_OAF_AL_DEFAULT       0
#define G_OAF_AL_RIGHT         1
#define G_OAF_AL_BOTTOM        2
#define G_OAF_AL_HCENTER       4
#define G_OAF_AL_VCENTER       8

//Animation States
#define G_AS_INACTIVE          0
#define G_AS_ACTIVE            1
#define G_AS_PAUSE             2

//New Animation States
enum g_en_anim_state {g_as_inactive, g_as_active, g_as_pause};

//Animation Types
#define G_AT_MATRIX            0
#define G_AT_BONE              1

/* Resources UIDS */
//Engine UIDS: 0..9999
#define RES_UID_NULL              0
#define RES_UID_UI_DRAWMODEL      253
#define RES_UID_UI_DRAWSPRITE     254
#define RES_UID_UI_DEF_FONT       255
#define RES_UID_UI_DEF_EFFECT     256
#define RES_UID_UI_DEF_MATERIAL   257

#define RES_UID_UI_TEXTURE_0      1000
#define RES_UID_UI_TEXTURE_MAX    1499

//Custom UIDS
#define RES_UID_USERDEFINE        10000 

/* Input Layout UIDS*/
//Engine UIDS: 0..9999
#define IL_UID_UI_NULL            0
#define IL_UID_UI_DEFAULT         50 

//Custom UIDS
#define IL_UID_USERDEFINE         10000 

//GInput Key Codes
#define  GI_KC_NOKEY           0x00
#define  GI_KC_ESCAPE          0x01
#define  GI_KC_1               0x02
#define  GI_KC_2               0x03
#define  GI_KC_3               0x04
#define  GI_KC_4               0x05
#define  GI_KC_5               0x06
#define  GI_KC_6               0x07
#define  GI_KC_7               0x08
#define  GI_KC_8               0x09
#define  GI_KC_9               0x0A
#define  GI_KC_0               0x0B
#define  GI_KC_MINUS           0x0C    /* - on main keyboard */
#define  GI_KC_EQUALS          0x0D
#define  GI_KC_BACK            0x0E    /* backspace */
#define  GI_KC_TAB             0x0F
#define  GI_KC_Q               0x10
#define  GI_KC_W               0x11
#define  GI_KC_E               0x12
#define  GI_KC_R               0x13
#define  GI_KC_T               0x14
#define  GI_KC_Y               0x15
#define  GI_KC_U               0x16
#define  GI_KC_I               0x17
#define  GI_KC_O               0x18
#define  GI_KC_P               0x19
#define  GI_KC_LBRACKET        0x1A
#define  GI_KC_RBRACKET        0x1B
#define  GI_KC_RETURN          0x1C    /* Enter on main keyboard */
#define  GI_KC_LCONTROL        0x1D
#define  GI_KC_A               0x1E
#define  GI_KC_S               0x1F
#define  GI_KC_D               0x20
#define  GI_KC_F               0x21
#define  GI_KC_G               0x22
#define  GI_KC_H               0x23
#define  GI_KC_J               0x24
#define  GI_KC_K               0x25
#define  GI_KC_L               0x26
#define  GI_KC_SEMICOLON       0x27
#define  GI_KC_APOSTROPHE      0x28
#define  GI_KC_GRAVE           0x29
#define  GI_KC_LSHIFT          0x2A
#define  GI_KC_BACKSLASH       0x2B
#define  GI_KC_Z               0x2C
#define  GI_KC_X               0x2D
#define  GI_KC_C               0x2E
#define  GI_KC_V               0x2F
#define  GI_KC_B               0x30
#define  GI_KC_N               0x31
#define  GI_KC_M               0x32
#define  GI_KC_COMMA           0x33
#define  GI_KC_PERIOD          0x34    /* . on main keyboard */
#define  GI_KC_SLASH           0x35    /* / on main keyboard */
#define  GI_KC_RSHIFT          0x36
#define  GI_KC_MULTIPLY        0x37    /* * on numeric keypad */
#define  GI_KC_LMENU           0x38    /* left Alt */
#define  GI_KC_SPACE           0x39
#define  GI_KC_CAPITAL         0x3A
#define  GI_KC_F1              0x3B
#define  GI_KC_F2              0x3C
#define  GI_KC_F3              0x3D
#define  GI_KC_F4              0x3E
#define  GI_KC_F5              0x3F
#define  GI_KC_F6              0x40
#define  GI_KC_F7              0x41
#define  GI_KC_F8              0x42
#define  GI_KC_F9              0x43
#define  GI_KC_F10             0x44
#define  GI_KC_NUMLOCK         0x45
#define  GI_KC_SCROLL          0x46
#define  GI_KC_NUMPAD7         0x47
#define  GI_KC_NUMPAD8         0x48
#define  GI_KC_NUMPAD9         0x49
#define  GI_KC_SUBTRACT        0x4A    /* - on numeric keypad */
#define  GI_KC_NUMPAD4         0x4B
#define  GI_KC_NUMPAD5         0x4C
#define  GI_KC_NUMPAD6         0x4D
#define  GI_KC_ADD             0x4E    /* + on numeric keypad */
#define  GI_KC_NUMPAD1         0x4F
#define  GI_KC_NUMPAD2         0x50
#define  GI_KC_NUMPAD3         0x51
#define  GI_KC_NUMPAD0         0x52
#define  GI_KC_DECIMAL         0x53    /* . on numeric keypad */
#define  GI_KC_OEM_102         0x56    /* <> or \| on RT 102-key keyboard (Non-U.S.) */
#define  GI_KC_F11             0x57
#define  GI_KC_F12             0x58
#define  GI_KC_F13             0x64    /*                     (NEC PC98) */
#define  GI_KC_F14             0x65    /*                     (NEC PC98) */
#define  GI_KC_F15             0x66    /*                     (NEC PC98) */
#define  GI_KC_KANA            0x70    /* (Japanese keyboard)            */
#define  GI_KC_ABNT_C1         0x73    /* /? on Brazilian keyboard */
#define  GI_KC_CONVERT         0x79    /* (Japanese keyboard)            */
#define  GI_KC_NOCONVERT       0x7B    /* (Japanese keyboard)            */
#define  GI_KC_YEN             0x7D    /* (Japanese keyboard)            */
#define  GI_KC_ABNT_C2         0x7E    /* Numpad . on Brazilian keyboard */
#define  GI_KC_NUMPADEQUALS    0x8D    /* = on numeric keypad (NEC PC98) */
#define  GI_KC_PREVTRACK       0x90    /* Previous Track (DIK_CIRCUMFLEX on Japanese keyboard) */
#define  GI_KC_AT              0x91    /*                     (NEC PC98) */
#define  GI_KC_COLON           0x92    /*                     (NEC PC98) */
#define  GI_KC_UNDERLINE       0x93    /*                     (NEC PC98) */
#define  GI_KC_KANJI           0x94    /* (Japanese keyboard)            */
#define  GI_KC_STOP            0x95    /*                     (NEC PC98) */
#define  GI_KC_AX              0x96    /*                     (Japan AX) */
#define  GI_KC_UNLABELED       0x97
#define  GI_KC_NEXTTRACK       0x99 
#define  GI_KC_NUMPADENTER     0x9C
#define  GI_KC_RCONTROL        0x9D
#define  GI_KC_MUTE            0xA0
#define  GI_KC_CALCULATOR      0xA1
#define  GI_KC_PLAYPAUSE       0xA2
#define  GI_KC_MEDIASTOP       0xA4
#define  GI_KC_VOLUMEDOWN      0xAE
#define  GI_KC_VOLUMEUP        0xB0
#define  GI_KC_WEBHOME         0xB2
#define  GI_KC_NUMPADCOMMA     0xB3    /* , on numeric keypad (NEC PC98) */
#define  GI_KC_DIVIDE          0xB5    /* / on numeric keypad */
#define  GI_KC_SYSRQ           0xB7
#define  GI_KC_RMENU           0xB8    /* right Alt */
#define  GI_KC_PAUSE           0xC5    /* Pause */
#define  GI_KC_HOME            0xC7    /* Home on arrow keypad */
#define  GI_KC_UP              0xC8    /* UpArrow on arrow keypad */
#define  GI_KC_PRIOR           0xC9    /* PgUp on arrow keypad */
#define  GI_KC_LEFT            0xCB    /* LeftArrow on arrow keypad */
#define  GI_KC_RIGHT           0xCD    /* RightArrow on arrow keypad */
#define  GI_KC_END             0xCF    /* End on arrow keypad */
#define  GI_KC_DOWN            0xD0    /* DownArrow on arrow keypad */
#define  GI_KC_NEXT            0xD1    /* PgDn on arrow keypad */
#define  GI_KC_INSERT          0xD2    /* Insert on arrow keypad */
#define  GI_KC_DELETE          0xD3    /* Delete on arrow keypad */
#define  GI_KC_LWIN            0xDB
#define  GI_KC_RWIN            0xDC 
#define  GI_KC_APPS            0xDD    /* AppMenu key */
#define  GI_KC_POWER           0xDE    /* System Power */
#define  GI_KC_SLEEP           0xDF    /* System Sleep */
#define  GI_KC_WAKE            0xE3    /* System Wake */
#define  GI_KC_WEBSEARCH       0xE5
#define  GI_KC_WEBFAVORITES    0xE6
#define  GI_KC_WEBREFRESH      0xE7
#define  GI_KC_WEBSTOP         0xE8
#define  GI_KC_WEBFORWARD      0xE9
#define  GI_KC_WEBBACK         0xEA
#define  GI_KC_MYCOMPUTER      0xEB
#define  GI_KC_MAIL            0xEC
#define  GI_KC_MEDIASELECT     0xED

#define  GI_KC_MOUSE_LEFT      0xFC
#define  GI_KC_MOUSE_RIGHT     0xFD
#define  GI_KC_MOUSE_MID       0xFE

//GInput Action Type
#define  GI_AT_KEYPRESS       1
#define  GI_AT_KEYUP          2
#define  GI_AT_KEYDOWN        3

//Sound Types
#define  G_ST_WAV             1

//FourCC L-E
#define  fourccRIFF 'FFIR'
#define  fourccDATA 'atad'
#define  fourccFMT  ' tmf'
#define  fourccWAVE 'EVAW'
#define  fourccXWMA 'AMWX'
#define  fourccDPDS 'sdpd'

//Default 2D Texture Desc              
const D3D10_TEXTURE2D_DESC DEF_DESC = {0, 0, 1, 1, DXGI_FORMAT_R8G8B8A8_UNORM, //w  h  mip arr fmt
                                       {1, 0}, D3D10_USAGE_DEFAULT, 0, 0, 0};  //smp{cnt qual} usg bind cpu misc
//Default 1D Texture Desc              
const D3D10_TEXTURE1D_DESC DEF_DESC_1D = {0, 1, 1, DXGI_FORMAT_R8_UINT, //w  h  mip arr fmt
                                         D3D10_USAGE_DEFAULT, 0, 0, 0};  //smp{cnt qual} usg bind cpu misc


#endif /* GCONSTANTS_H */