/*
PLEASE READ THE CYBERON END USER LICENSE AGREEMENT ("LICENSE(Cyberon)") CAREFULLY BEFORE DOWNLOADING, INSTALLING, COPYING, OR USING THIS SOFTWARE AND ACCOMPANYING DOCUMENTATION.
BY DOWNLOADING, INSTALLING, COPYING OR USING THE SOFTWARE, YOU ARE AGREEING TO BE BOUND BY THE AGREEMENT.
IF YOU DO NOT AGREE TO ALL OF THE TERMS OF THE AGREEMENT, PROMPTLY RETURN AND DO NOT USE THE SOFTWARE.
*/

#ifndef _CYB_MODEL_INFOR_H
#define _CYB_MODEL_INFOR_H

#include "ConvertUTF.h"

//This module is used to handle the packed model of DSpotter.

//The contents of DSpotter packed binary model.
//XXX_pack.bin                       => CYBase.mod/Group_1.mod/.../Group_N.mod
//XXX_pack_WithTxt.bin               => CYBase.mod/Group_1.mod/.../Group_N.mod/Group_1.txt/.../Group_N.txt
//XXX_pack_WithTxtAndTri.bin         => CYBase.mod/Group_1.mod/.../Group_N.mod/Group_1.txt/.../Group_N.txt/CYTrimap.mod
//XXX_pack_WithTxtAndTriAndMapID.bin => CYBase.mod/Group_1.mod/.../Group_N.mod/Group_1.txt/.../Group_N.txt/CYTrimap.mod/AllGroup_MapID_pack.bin

#define CYB_MODEL_SUCCESS                      0
#define CYB_MODEL_INVALID_PARAM                -1
#define CYB_MODEL_NOT_ENOUGH_MEMORY            -2
#define CYB_MODEL_INVALID_FORMAT               -3

typedef struct _SCybModel
{
    const BYTE *pbyCyModelPack;
    BOOL bAllocateMemory;
    BOOL bWithTxt;
    BOOL bWithMapID;
    BOOL bWithTriMap;
    int nPackCount;
    int nGroupCount;
}   SCybModel;


#define CYBMODEL_GET_MEM_USAGE() (sizeof(SCybModel))


#ifdef __cplusplus
extern "C" {
#endif

/* Get memory usage of this module.
 * Return the memory usage. */
int CybModelGetMemoryUsage();

/* To initialize this module.
 *   pbyCyModelPack(in): The packed model of DSpotter.
 *   lpbyMem(in): The memory buffer for this module. If this parameter is NULL, this API will use malloc() to allocate memory.
 *   nMemSize(in): The size of memory buffer.
 *   pnErr(out): CYB_MODEL_SUCCESS indicates success, else error code. It can be NULL.
 * Return a module handle or NULL if an error occurs. */
HANDLE CybModelInit(const BYTE *pbyCyModelPack, BYTE *lpbyMem, int nMemSize, int *pnErr);

/* To release this module.
 *   hCybModel(in): The handle of CybModel.
 * Return CYB_MODEL_SUCCESS or negative value if an error occurs. */
int CybModelRelease(HANDLE hCybModel);

/* Get group count of packed model.
 *   hCybModel(in): The handle of CybModel.
 * Return the total number of group model. or negative value if an error occurs. */
int CybModelGetGroupCount(HANDLE hCybModel);

/* Get the memory address of base model.
 *   hCybModel(in): The handle of CybModel.
 * Return the memory address of base model. or NULL if an error occurs. */
const BYTE* CybModelGetBase(HANDLE hCybModel);

/* Get the memory address of group model.
 *   hCybModel(in): The handle of CybModel.
 *   nGroupIndex(in): The group index(0 based) in the packed model.
 * Return the memory address of group model or NULL if an error occurs. */
const BYTE* CybModelGetGroup(HANDLE hCybModel, int nGroupIndex);

/* Get the text infor of group model.
 *   hCybModel(in): The handle of CybModel.
 *   nGroupIndex(in): The group index(0 based) in the packed model.
 *   pnSize(out): The size of text infor.
 * Return the memory address of text infor or NULL if an error occurs. */
const BYTE* CybModelGetGroupTxt(HANDLE hCybModel, int nGroupIndex, int *pnSize);

/* Get AllGroup_MapID_pack.
 *   hCybModel(in): The handle of CybModel.
 * Return the memory address of AllGroup_MapID_pack or NULL if an error occurs. */
const BYTE* CybModelGetAllMapID(HANDLE hCybModel);

/* Get the map ID memory address of group model in AllGroup_MapID_pack.
 *   hCybModel(in): The handle of CybModel.
 *   nGroupIndex(in): The group index(0 based) in the packed model.
 * Return the map ID memory address of group model or NULL if an error occurs. */
const BYTE* CybModelGetMapID(HANDLE hCybModel, int nGroupIndex);

/* Get the memory address of tri-map model.
 *   hCybModel(in): The handle of CybModel.
 * Return thememory address of group model or NULL if an error occurs. */
const BYTE* CybModelGetTriMap(HANDLE hCybModel);

/* Get the total command number of group model.
	*   hCybModel(in): The handle of CybModel.
	*   nGroupIndex(in): The group index(0 based) in the packed model.
	* Return the total command number of group model. */
int CybModelGetCommandCount(HANDLE hCybModel, int nGroupIndex);

/* Get the command information.
	*   hCybModel(in): The handle of CybModel.
	*   nGroupIndex(in): The group index(0 based) in the packed model.
	*   nCommandIndex(in): The command index(0 based) in the group.
	*   lpszCommand(out): The buffer for UTF8 command text, could be NULL.
	*   nCommandLen(in): The buffer length of UTF8 command text.
	*   pnMapID(out): The map ID of the command which is defined in DSMT, could be NULL.
	* Return the UTF8 command text. If it is a garbage command(reward = -100 in DSMT), empty string will return. */
char *CybModelGetCommandInfo(HANDLE hCybModel, int nGroupIndex, int nCommandIndex, char *lpszCommand, int nCommandLen, int *pnMapID);

/* Get the command information.
	*   hCybModel(in): The handle of CybModel.
	*   nGroupIndex(in): The group index(0 based) in the packed model.
	*   nCommandIndex(in): The command index(0 based) in the group.
	*   lpszCommand(out): The buffer for UTF8 command text, could be NULL.
	*   nCommandLen(in): The buffer length of UTF8 command text.
	*   pnMapID(out): The map ID of the command which is defined in DSMT, could be NULL.
	* Return the UTF8 command text. If it is a garbage command(reward = -100 in DSMT), empty string will return. */
UTF16 *CybModelGetCommandInfoUTF16(HANDLE hCybModel, int nGroupIndex, int nCommandIndex, UTF16 *lpszCommand, int nCommandLen, int *pnMapID);

/* Get the command information.
	*   hCybModel(in): The handle of CybModel.
	*   nID(in): The map ID of command.
	*   pnGroupIndex(out): The group index(0 based) in the packed model.
	*   pnCommandIndex(out): The command index(0 based) in the group.
	*   lpszCommand(out): The buffer for UTF8 command text, could be NULL.
	*   nCommandLen(in): The buffer length of UTF8 command text.
	* Return the UTF8 command text. If it is a garbage command(reward = -100 in DSMT), empty string will return. */
char *CybModelGetCommandInfoByID(HANDLE hCybModel, int nID, int *pnGroupIndex, int *pnCommandIndex, char *lpszCommand, int nCommandLen);

/* Get the model level.
 *   hCybModel(in): The handle of CybModel.
 * Return the model level. */
int CybModelGetLevel(HANDLE hCybModel);

/* Get a specific packed address from the packed data.
 *   lpbyBin(in): The packed data.
 *   nPackIndex(in): The index of the specific data.
 *   pnUnpackBinSize(out): The size of the specific data.
 * Return the memory address of the specific data. */
const BYTE* UnpackBin(const BYTE *lpbyBin, int nPackIndex, int *pnUnpackBinSize);


#ifdef __cplusplus
}
#endif

#endif
