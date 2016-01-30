/**
 * @file hotkeys.h
 * PCBNew hotkeys
 */
#ifndef _PCBNEW_KOTKEYS_H
#define _PCBNEW_HOTKEYS_H

#include "hotkeys_basic.h"

// List of hot keys id.
// see also enum common_hotkey_id_commnand in hotkeys_basic.h
// for shared hotkeys id
enum hotkey_id_commnand {
    HK_DELETE = HK_COMMON_END,
    HK_BACK_SPACE,
    HK_ROTATE_FOOTPRINT,
    HK_MOVE_FOOTPRINT,
    HK_DRAG_FOOTPRINT,
    HK_FLIP_FOOTPRINT,
    HK_GET_AND_MOVE_FOOTPRINT,
    HK_LOCK_UNLOCK_FOOTPRINT,
    HK_ADD_NEW_TRACK,
    HK_ADD_VIA,
    HK_ADD_MICROVIA,
    HK_END_TRACK,
    HK_SAVE_BOARD, HK_LOAD_BOARD,
    HK_SWITCH_UNITS,
    HK_SWITCH_TRACK_DISPLAY_MODE,
    HK_FIND_ITEM,
    HK_SWITCH_LAYER_TO_COPPER,
    HK_SWITCH_LAYER_TO_COMPONENT,
    HK_SWITCH_LAYER_TO_NEXT,
    HK_SWITCH_LAYER_TO_PREVIOUS,
    HK_SWITCH_LAYER_TO_INNER1,
    HK_SWITCH_LAYER_TO_INNER2,
    HK_SWITCH_LAYER_TO_INNER3,
    HK_SWITCH_LAYER_TO_INNER4,
    HK_SWITCH_LAYER_TO_INNER5,
    HK_SWITCH_LAYER_TO_INNER6,
    HK_SWITCH_LAYER_TO_INNER7,
    HK_SWITCH_LAYER_TO_INNER8,
    HK_SWITCH_LAYER_TO_INNER9,
    HK_SWITCH_LAYER_TO_INNER10,
    HK_SWITCH_LAYER_TO_INNER11,
    HK_SWITCH_LAYER_TO_INNER12,
    HK_SWITCH_LAYER_TO_INNER13,
    HK_SWITCH_LAYER_TO_INNER14,
    HK_ADD_MODULE,
    HK_MOVE_TRACK,
    HK_SLIDE_TRACK
};

// List of hotkey descriptors for pcbnew
extern struct Ki_HotkeyInfoSectionDescriptor s_Pcbnew_Editor_Hokeys_Descr[];

// List of hotkey descriptors for the board editor only
extern struct Ki_HotkeyInfoSectionDescriptor s_Board_Editor_Hokeys_Descr[];

// List of hotkey descriptors for the footprint editor only
extern struct Ki_HotkeyInfoSectionDescriptor s_Module_Editor_Hokeys_Descr[];

#endif /* _PCBNEW_HOTKEYS_H_ */
