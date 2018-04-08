#ifndef SHF_CONFIGURATOR_H_
#define SHF_CONFIGURATOR_H_

#include "shf_define.h"

//[] means array element
//ID: uint8
//array size: uint8
//CID: condition index uint8
//AID: action index uint8
//action: uint8

//in: ID, chip no, item array size, [item], action array size, [action]
//out: ID, chip no, CID
#define SHF_AP_CONDITION_ADD (0x80)
////in: ID, chip no, CID
////out: ID, chip no, status_t
//#define SHF_IPI_CONDITION_REMOVE (0x81)
//in: ID, chip no, CID, item array size, [item], action array size, [action]
//out: ID, chip no, status_t
#define SHF_AP_CONDITION_UPDATE (0x82)
//in: ID, chip no, CID, action array size, [action]
//out: ID, chip no, AID array size, [AID]
#define SHF_AP_CONDITION_ACTION_ADD (0x83)
//in: ID, chip no, CID, AID array size, [AID]
//out: ID, chip no, status_t array size, [status_t]
#define SHF_AP_CONDITION_ACTION_REMOVE (0x84)
////in: ID, chip no, CID, action array size, {AID, action}
////out: ID, chip no, status_t array size, [status_t]
//#define SHF_AP_CONDITION_ACTION_UPDATE (0x85)
////in: ID, chip no, size, index
////out: ID, chip no, size, index, type, cur data
//#define SHF_AP_DATA_GET (0x86)

#define SHF_AP_TRIGGER (0x87)

//for get data operation, scp only support at most 8.
#define SHF_MAX_DATA_GET_COUNT (8)

/**
 * for sending data
 * CID: condition id
 * AID: action id
 * result: action run result
 * other: data
 */
#define TRIGGER_SLOT_MSGID              (0x00)
#define TRIGGER_SLOT_CHIP_NO            (0x01)
#define TRIGGER_SLOT_CID                (0x02)
#define TRIGGER_SLOT_AID1               (0x03)
#define TRIGGER_SLOT_AID2               (0x04)
#define TRIGGER_SLOT_AID3               (0x05)
#define TRIGGER_SLOT_AID4               (0x06)
#define TRIGGER_SLOT_RESULT             (0x07)
#define TRIGGER_SLOT_SIZE               (0x08)
#define TRIGGER_SLOT_RESERVED           (0x09)

#ifdef SHF_DEBUG_MODE
//in: ID
#define SHF_AP_DUMP (0x90)
#endif

//in: ID, chip no, enable/disable, timeout(second)
//out: ID, chip no, status_t
#define SHF_CONSYS_REGISTER_TIMEOUT (0x51)
//in: ID, chip no, enable/disable, confidence
//out: ID, chip no, status_t
#define SHF_CONSYS_REGISTER_CONTEXT (0x52)
////in: ID, chip no, none
////out: ID, chip no, status_t
//#define SHF_CONSYS_WAKEUP_AP (0x53)
//in: ID, chip no, none
//out: ID, chip no, activity, confidence, speed
#define SHF_CONSYS_GET_MOTION_STATE (0x54)
//in: none
//out: ID, chip no, ID
#define SHF_CONSYS_MOTION_TRIGGER (0x55)

/**
* Note: In md32, sizeof(shf_condition_item_t) = 12. But in Android, sizeof(shf_condition_item_t) = 16.
* So we have to skip the 4 bytes padding when copying shf_condition_item_t.
* We do not pad it to 16 bytes for the sake of saving md32 memory.
*/
#define AP_MD32_CONDITION_ITEM_SIZE_DIFF (4)

void shf_configurator_init();

#endif /* SHF_CONFIGURATOR_H_ */
