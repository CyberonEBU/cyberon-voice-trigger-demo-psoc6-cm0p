/*
PLEASE READ THE CYBERON END USER LICENSE AGREEMENT ("LICENSE(Cyberon)") CAREFULLY BEFORE DOWNLOADING, INSTALLING, COPYING, OR USING THIS SOFTWARE AND ACCOMPANYING DOCUMENTATION.
BY DOWNLOADING, INSTALLING, COPYING OR USING THE SOFTWARE, YOU ARE AGREEING TO BE BOUND BY THE AGREEMENT.
IF YOU DO NOT AGREE TO ALL OF THE TERMS OF THE AGREEMENT, PROMPTLY RETURN AND DO NOT USE THE SOFTWARE.
*/

#include <stdlib.h>
#include <string.h>
#include "base_types.h"
#include "DSpotterSDKApi.h"
#include "ConvertUTF.h"
#include "CybModelInfor.h"

static BOOL IsModelFile(const BYTE* lpbyDataFile, int nDataFileSize);
static BOOL IsGroupTextFile(const BYTE* lpbyDataFile, int nDataFileSize);
static BOOL IsAllMapIdFile(const BYTE* lpbyDataFile, int nDataFileSize);
static BOOL IsTrimapFile(const BYTE* lpbyDataFile, int nDataFileSize);
static int  GetModelSize(const BYTE* lpbyModel);

int CybModelGetMemoryUsage()
{
	return (int)sizeof(SCybModel);
}

HANDLE CybModelInit(const BYTE *pbyCyModelPack, BYTE *lpbyMem, int nMemSize, int *pnErr)
{
	SCybModel *pSCybModel = NULL;
	int nErr = CYB_MODEL_SUCCESS;

	if (lpbyMem != NULL)
	{
		if (nMemSize >= (int)sizeof(SCybModel))
		{
			pSCybModel = (SCybModel *)lpbyMem;
			pSCybModel->bAllocateMemory = FALSE;
		}
		else
		{
			nErr = CYB_MODEL_NOT_ENOUGH_MEMORY;
		}
	}
	else
	{
		pSCybModel = (SCybModel *)malloc(sizeof(SCybModel));
		if (pSCybModel != NULL)
			pSCybModel->bAllocateMemory = TRUE;
		else
			nErr = CYB_MODEL_NOT_ENOUGH_MEMORY;
	}

	while (pSCybModel != NULL)
	{
		int *lpnBin = (int *)pbyCyModelPack;
		int nPackCount = lpnBin[0];
		int nUnpackBinSize;
		const BYTE* lpbyDataFile;

		nErr = CYB_MODEL_INVALID_FORMAT;
		if (nPackCount <= 0)
			break;
		lpbyDataFile = UnpackBin(pbyCyModelPack, 0, &nUnpackBinSize);
		if (!IsModelFile(lpbyDataFile, nUnpackBinSize))
			break;

		pSCybModel->pbyCyModelPack = pbyCyModelPack;
		pSCybModel->nPackCount = nPackCount;

		lpbyDataFile = UnpackBin(pbyCyModelPack, nPackCount - 1, &nUnpackBinSize);
		pSCybModel->bWithMapID = IsAllMapIdFile(lpbyDataFile, nUnpackBinSize);
		if (pSCybModel->bWithMapID)
			nPackCount--;

		lpbyDataFile = UnpackBin(pbyCyModelPack, nPackCount - 1, &nUnpackBinSize);
		pSCybModel->bWithTriMap = IsTrimapFile(lpbyDataFile, nUnpackBinSize);
		if (pSCybModel->bWithTriMap)
			nPackCount--;

		lpbyDataFile = UnpackBin(pbyCyModelPack, nPackCount - 1, &nUnpackBinSize);
		pSCybModel->bWithTxt = IsGroupTextFile(lpbyDataFile, nUnpackBinSize);

		nPackCount--;   //CYBase
		if (pSCybModel->bWithTxt)
			pSCybModel->nGroupCount = nPackCount / 2;
		else
			pSCybModel->nGroupCount = nPackCount;

		nErr = CYB_MODEL_SUCCESS;
		break;
	}

	if (pnErr != NULL)
		*pnErr = nErr;

	return (HANDLE)pSCybModel;
}

int CybModelRelease(HANDLE hCybModel)
{
	SCybModel *pSCybModel = (SCybModel *)hCybModel;
	
	if (pSCybModel != NULL && pSCybModel->bAllocateMemory)
	{
		pSCybModel->bAllocateMemory = FALSE;
		free(pSCybModel);
	}

	return CYB_MODEL_SUCCESS;
}

int CybModelGetGroupCount(HANDLE hCybModel)
{
	SCybModel *pSCybModel = (SCybModel *)hCybModel;

	if (pSCybModel == NULL)
		return CYB_MODEL_INVALID_PARAM;

	return pSCybModel->nGroupCount;
}

const BYTE* CybModelGetBase(HANDLE hCybModel)
{
	SCybModel *pSCybModel = (SCybModel *)hCybModel;

	if (pSCybModel == NULL)
		return NULL;

	return UnpackBin(pSCybModel->pbyCyModelPack, 0, NULL);
}

const BYTE* CybModelGetGroup(HANDLE hCybModel, int nGroupIndex)
{
	SCybModel *pSCybModel = (SCybModel *)hCybModel;

	if (pSCybModel == NULL || nGroupIndex >= pSCybModel->nGroupCount)
		return NULL;

	return UnpackBin(pSCybModel->pbyCyModelPack, nGroupIndex + 1, NULL);
}

const BYTE* CybModelGetGroupTxt(HANDLE hCybModel, int nGroupIndex, int *pnSize)
{
	SCybModel *pSCybModel = (SCybModel *)hCybModel;

	if (pSCybModel == NULL || nGroupIndex >= pSCybModel->nGroupCount || !pSCybModel->bWithTxt)
		return NULL;

	return UnpackBin(pSCybModel->pbyCyModelPack, nGroupIndex + 1 + pSCybModel->nGroupCount, pnSize);
}

const BYTE* CybModelGetAllMapID(HANDLE hCybModel)
{
	SCybModel *pSCybModel = (SCybModel *)hCybModel;

	if (pSCybModel == NULL || !pSCybModel->bWithMapID)
		return NULL;

	int nPackCount = pSCybModel->nPackCount;

	return UnpackBin(pSCybModel->pbyCyModelPack, nPackCount - 1, NULL);
}

const BYTE* CybModelGetMapID(HANDLE hCybModel, int nGroupIndex)
{
	SCybModel *pSCybModel = (SCybModel *)hCybModel;
	const BYTE* lpbyAllMapID;

	if (pSCybModel == NULL || nGroupIndex >= pSCybModel->nGroupCount)
		return NULL;

	lpbyAllMapID = CybModelGetAllMapID(hCybModel);

	return UnpackBin(lpbyAllMapID, nGroupIndex, NULL);
}

const BYTE* CybModelGetTriMap(HANDLE hCybModel)
{
	SCybModel *pSCybModel = (SCybModel *)hCybModel;

	if (pSCybModel == NULL || !pSCybModel->bWithTriMap)
		return NULL;

	int nPackCount = pSCybModel->nPackCount;

	if (pSCybModel->bWithMapID)
		nPackCount--;

	return UnpackBin(pSCybModel->pbyCyModelPack, nPackCount - 1, NULL);
}

int CybModelGetCommandCount(HANDLE hCybModel, int nGroupIndex)
{
	return DSpotter_GetNumWord((BYTE *)CybModelGetGroup(hCybModel, nGroupIndex));
}

char *CybModelGetCommandInfo(HANDLE hCybModel, int nGroupIndex, int nCommandIndex, char *lpszCommand, int nCommandLen, int *pnMapID)
{
	SCybModel *pSCybModel = (SCybModel *)hCybModel;

	if (pSCybModel == NULL || nGroupIndex >= pSCybModel->nGroupCount)
		return NULL;

	int nUnpackBinSize;
	const UTF16* pchGroupText = (const UTF16*)CybModelGetGroupTxt(hCybModel, nGroupIndex, &nUnpackBinSize);
	int i = 0;
	int nLine = 0; // 0 base
	const UTF16 *lpszLineWord[5] = { NULL };

	if (pchGroupText != NULL)
	{
		while ((i < nUnpackBinSize / 2) && (nLine <= nCommandIndex + 1))
		{
			if (lpszLineWord[0] == NULL)
				lpszLineWord[0] = pchGroupText + i;

			if (pchGroupText[i] == '\t')
			{
				if (lpszLineWord[1] == NULL)
					lpszLineWord[1] = pchGroupText + i;
				else if (lpszLineWord[2] == NULL)
					lpszLineWord[2] = pchGroupText + i;
				else if (lpszLineWord[3] == NULL)
					lpszLineWord[3] = pchGroupText + i;
				else if (lpszLineWord[4] == NULL)
					lpszLineWord[4] = pchGroupText + i;  // For new Group.txt, the SG is the last one.
			}

			if (pchGroupText[i] == '\n' || i == nUnpackBinSize / 2 - 1)
			{
				if (nLine == nCommandIndex + 1)
				{
					const UTF16 *sourceStart;
					const UTF16 *sourceEnd;
					UTF8 *targetStart;
					UTF8 *targetEnd;
					char szTemp[10] = { 0 };

					if (lpszLineWord[4] == NULL)
					{
						// Old Group.txt doesn't have SG, then program will go to here.
						if (pchGroupText[i] == '\n')
							lpszLineWord[4] = pchGroupText + i - 1;  // Skip '\r' for old Group.txt, the map ID is the last one.
						else
							lpszLineWord[4] = pchGroupText + i;
					}

					// Abstract UTF16LE alphabet/digit to Ascii code.
					sourceStart = lpszLineWord[2] + 1;
					sourceEnd = lpszLineWord[3];
					targetStart = (UTF8*)szTemp;
					targetEnd = targetStart + sizeof(szTemp) - 1;
					ConvertUTF16toUTF8(&sourceStart, sourceEnd, &targetStart, targetEnd, strictConversion);
					*targetStart = 0;

					// Check for garbage command (reward = -100).
					if (lpszCommand != NULL)
					{
						int nReward = atoi(szTemp);
						if (nReward == -100)
						{
							lpszCommand[0] = 0;
						}
						else
						{
							sourceStart = lpszLineWord[0];
							sourceEnd = lpszLineWord[1];
							targetStart = (UTF8*)lpszCommand;
							targetEnd = targetStart + nCommandLen - 1;
							ConvertUTF16toUTF8(&sourceStart, sourceEnd, &targetStart, targetEnd, strictConversion);
							*targetStart = 0;
						}
					}

					if (pnMapID != NULL)
					{
						sourceStart = lpszLineWord[3] + 1;
						sourceEnd = lpszLineWord[4];
						targetStart = (UTF8*)szTemp;
						targetEnd = targetStart + sizeof(szTemp) - 1;
						ConvertUTF16toUTF8(&sourceStart, sourceEnd, &targetStart, targetEnd, strictConversion);
						*targetStart = 0;

						*pnMapID = atoi(szTemp);
					}

					break;
				}
				nLine++;
				memset((void *)lpszLineWord, 0, sizeof(lpszLineWord));
			}

			i++;
		}
	}
	else
	{
		lpszCommand = NULL;
	}

	return lpszCommand;
}

UTF16 *CybModelGetCommandInfoUTF16(HANDLE hCybModel, int nGroupIndex, int nCommandIndex, UTF16 *lpszCommand, int nCommandLen, int *pnMapID)
{
	SCybModel *pSCybModel = (SCybModel *)hCybModel;

	if (pSCybModel == NULL || nGroupIndex >= pSCybModel->nGroupCount)
		return NULL;

	int nUnpackBinSize;
	const UTF16* pchGroupText = (const UTF16*)CybModelGetGroupTxt(hCybModel, nGroupIndex, &nUnpackBinSize);
	int i = 0;
	int nLine = 0;
	const UTF16 *lpszLineWord[5] = { NULL };

	if (pchGroupText != NULL)
	{
		while (i < nUnpackBinSize / 2)
		{
			if (lpszLineWord[0] == NULL)
				lpszLineWord[0] = pchGroupText + i;

			if (pchGroupText[i] == '\t')
			{
				if (lpszLineWord[1] == NULL)
					lpszLineWord[1] = pchGroupText + i;
				else if (lpszLineWord[2] == NULL)
					lpszLineWord[2] = pchGroupText + i;
				else if (lpszLineWord[3] == NULL)
					lpszLineWord[3] = pchGroupText + i;
				else if (lpszLineWord[4] == NULL)
					lpszLineWord[4] = pchGroupText + i;  // For new Group.txt, the SG is the last one.
			}

			if (pchGroupText[i] == '\n' || i == nUnpackBinSize / 2 - 1)
			{
				if (nLine == nCommandIndex + 1)
				{
					const UTF16 *sourceStart;
					const UTF16 *sourceEnd;
					UTF8 *targetStart;
					UTF8 *targetEnd;
					char szTemp[10] = { 0 };

					if (lpszLineWord[4] == NULL)
					{
						// Old Group.txt doesn't have SG, then program will go to here.
						if (pchGroupText[i] == '\n')
							lpszLineWord[4] = pchGroupText + i - 1;  // Skip '\r' for old Group.txt, the map ID is the last one.
						else
							lpszLineWord[4] = pchGroupText + i;
					}

					// Abstract UTF16LE alphabet/digit to Ascii code.
					sourceStart = lpszLineWord[2] + 1;
					sourceEnd = lpszLineWord[3];
					targetStart = (UTF8*)szTemp;
					targetEnd = targetStart + sizeof(szTemp) - 1;
					ConvertUTF16toUTF8(&sourceStart, sourceEnd, &targetStart, targetEnd, strictConversion);
					*targetStart = 0;

					if (lpszCommand != NULL)
					{
						// Check for garbage command (reward = -100).
						int nReward = atoi(szTemp);
						if (nReward == -100)
						{
							lpszCommand[0] = 0;
						}
						else
						{
							int j;
							memset(lpszCommand, 0, (UINT)nCommandLen * sizeof(UTF16));
							sourceStart = lpszLineWord[0];
							sourceEnd = lpszLineWord[1];
							for (j = 0; j < nCommandLen - 1; j++)
							{
								if (sourceStart + j == sourceEnd)
									break;
								lpszCommand[j] = sourceStart[j];
							}
						}
					}

					if (pnMapID != NULL)
					{
						sourceStart = lpszLineWord[3] + 1;
						sourceEnd = lpszLineWord[4];
						targetStart = (UTF8*)szTemp;
						targetEnd = targetStart + sizeof(szTemp) - 1;
						ConvertUTF16toUTF8(&sourceStart, sourceEnd, &targetStart, targetEnd, strictConversion);
						*targetStart = 0;

						*pnMapID = atoi(szTemp);
					}

					break;
				}
				nLine++;
				memset((void *)lpszLineWord, 0, sizeof(lpszLineWord));
			}

			i++;
		}
	}
	else
	{
		lpszCommand = NULL;
	}

	return lpszCommand;
}

char *CybModelGetCommandInfoByID(HANDLE hCybModel, int nID, int *pnGroupIndex, int *pnCommandIndex, char *lpszCommand, int nCommandLen)
{
	SCybModel *pSCybModel = (SCybModel *)hCybModel;
	int nGroupIndex;
	int nModelMapID = -10000;
	char *lpszRet = NULL;
	int nLine = 0; // 0 base

	if (pSCybModel == NULL || nID < 0)
		return NULL;
	
	for (nGroupIndex = 0; nGroupIndex < pSCybModel->nGroupCount; nGroupIndex++)
	{
		int nUnpackBinSize;
		const UTF16* pchGroupText = (const UTF16*)CybModelGetGroupTxt(hCybModel, nGroupIndex, &nUnpackBinSize);
		int i = 0;
		const UTF16 *lpszLineWord[5] = { NULL };

		if (pchGroupText == NULL)
			continue;

		nLine = 0;
		{
			while (i < nUnpackBinSize / 2)
			{
				if (lpszLineWord[0] == NULL)
					lpszLineWord[0] = pchGroupText + i;

				if (pchGroupText[i] == '\t')
				{
					if (lpszLineWord[1] == NULL)
						lpszLineWord[1] = pchGroupText + i;
					else if (lpszLineWord[2] == NULL)
						lpszLineWord[2] = pchGroupText + i;
					else if (lpszLineWord[3] == NULL)
						lpszLineWord[3] = pchGroupText + i;
					else if (lpszLineWord[4] == NULL)
						lpszLineWord[4] = pchGroupText + i;  // For new Group.txt, the SG is the last one.
				}

				if (pchGroupText[i] == '\n' || i == nUnpackBinSize / 2 - 1)
				{
					nLine++;

					if ((nLine == 1) || (lpszLineWord[1] == NULL) || (lpszLineWord[2] == NULL) || (lpszLineWord[3] == NULL))
					{
						i++;
						memset((void *)lpszLineWord, 0, sizeof(lpszLineWord));
						continue;
					}
					else if (lpszLineWord[4] == NULL)
					{
						// Old Group.txt doesn't have SG, then program will go to here.
						if (pchGroupText[i] == '\n')
							lpszLineWord[4] = pchGroupText + i - 1;  // Skip '\r' for old Group.txt, the map ID is the last one.
						else
							lpszLineWord[4] = pchGroupText + i;
					}

					const UTF16 *sourceStart;
					const UTF16 *sourceEnd;
					UTF8 *targetStart;
					UTF8 *targetEnd;
					char szTemp[10] = { 0 };

					sourceStart = lpszLineWord[3] + 1;
					sourceEnd = lpszLineWord[4];
					targetStart = (UTF8*)szTemp;
					targetEnd = targetStart + sizeof(szTemp) - 1;
					ConvertUTF16toUTF8(&sourceStart, sourceEnd, &targetStart, targetEnd, strictConversion);
					*targetStart = 0;

					nModelMapID = atoi(szTemp);
					if (nModelMapID != nID)
					{
						i++;
						memset((void *)lpszLineWord, 0, sizeof(lpszLineWord));
						continue;
					}

					// Abstract UTF16LE alphabet/digit to Ascii code.
					sourceStart = lpszLineWord[2] + 1;
					sourceEnd = lpszLineWord[3];
					targetStart = (UTF8*)szTemp;
					targetEnd = targetStart + sizeof(szTemp) - 1;
					ConvertUTF16toUTF8(&sourceStart, sourceEnd, &targetStart, targetEnd, strictConversion);
					*targetStart = 0;

					// Check for garbage command (reward = -100).
					if (lpszCommand != NULL)
					{
						int nReward = atoi(szTemp);

						lpszRet = lpszCommand;
						if (nReward == -100)
						{
							// Return NULL string if it is garbage word.
							lpszCommand[0] = 0;
							nModelMapID = -10000;
						}
						else
						{
							sourceStart = lpszLineWord[0];
							sourceEnd = lpszLineWord[1];
							targetStart = (UTF8*)lpszCommand;
							targetEnd = targetStart + nCommandLen - 1;
							ConvertUTF16toUTF8(&sourceStart, sourceEnd, &targetStart, targetEnd, strictConversion);
							*targetStart = 0;
						}
					}

					break;
				}

				i++;
			}
			if (nModelMapID == nID)
				break;
		}
	}

	if (nModelMapID == nID)
	{
		if (pnGroupIndex != NULL)
			*pnGroupIndex = nGroupIndex;
		if (pnCommandIndex != NULL)
			*pnCommandIndex = nLine - 2;
	}

	return lpszRet;
}

const BYTE* UnpackBin(const BYTE *lpbyBin, int nIndex, int *pnUnpackBinSize)
{
	DWORD *lpnBin = (DWORD *)lpbyBin;
	DWORD nNumBin;
	DWORD *lpnBinSize = lpnBin + 1;
	const BYTE *lpbyModel = NULL;
	DWORD i;

	if (lpbyBin == NULL)
		return NULL;

	nNumBin = lpnBin[0];
	lpbyModel = (const BYTE *)(lpnBinSize + nNumBin);
	if (pnUnpackBinSize != NULL)
		*pnUnpackBinSize = (int)lpnBinSize[nIndex];
	if (nIndex == 0)
		return lpbyModel;

	for (i = 1; i < nNumBin; i++)
	{
		lpbyModel = lpbyModel + lpnBinSize[i - 1];
		if (i == (DWORD)nIndex)
			break;
	}
	return lpbyModel;
}

int CybModelGetLevel(HANDLE hCybModel)
{
	SCybModel *pSCybModel = (SCybModel *)hCybModel;
	int nBaseModelSize = -1;

	if (pSCybModel == NULL)
		return CYB_MODEL_INVALID_PARAM;

	UnpackBin(pSCybModel->pbyCyModelPack, 0, &nBaseModelSize);
	if (nBaseModelSize > 650000)
		return 4;
	else if (nBaseModelSize > 400000)
		return 3;
	else if (nBaseModelSize > 200000)
		return 2;
	else if (nBaseModelSize > 120000)
		return 1;
	else if (nBaseModelSize > 70000)
		return 0;
	else
		return CYB_MODEL_INVALID_FORMAT;
}

static BOOL IsModelFile(const BYTE* lpbyDataFile, int nDataFileSize)
{
	return (nDataFileSize >= 8 && GetModelSize(lpbyDataFile) >= 8);
}

static BOOL IsGroupTextFile(const BYTE* lpbyDataFile, int nDataFileSize)
{
	return (nDataFileSize > 2 && lpbyDataFile[0] == 0xFF && lpbyDataFile[1] == 0xFE);
}

static BOOL IsAllMapIdFile(const BYTE* lpbyDataFile, int nDataFileSize)
{
	if (IsModelFile(lpbyDataFile, nDataFileSize))
		return FALSE;

	if (IsGroupTextFile(lpbyDataFile, nDataFileSize))
		return FALSE;

	int *lpnBin = (int *)lpbyDataFile;
	int nNumBin = lpnBin[0];
	int *lpnBinSize = lpnBin + 1;
	const BYTE *lpbyModel = NULL;
	int i;

	lpbyModel = (const BYTE *)(lpnBinSize + nNumBin);
	if (nNumBin > 100 || !IsModelFile(lpbyModel, lpnBinSize[0]))
		return FALSE;

	for (i = 1; i < nNumBin; i++)
	{
		lpbyModel += lpnBinSize[i - 1];

		// Check file size boundary
		if (lpbyModel - lpbyDataFile > nDataFileSize)
			return FALSE;

		if (!IsModelFile(lpbyModel, lpnBinSize[i]))
			return FALSE;
	}
	return TRUE;
}

static BOOL IsTrimapFile(const BYTE* lpbyDataFile, int nDataFileSize)
{
	if (!IsModelFile(lpbyDataFile, nDataFileSize))
		return FALSE;

	for (int i = 8; i < nDataFileSize - 8; i += 8)
	{
		if (GetModelSize(lpbyDataFile + i) >= 8)
			return TRUE;
	}

	return FALSE;
}

static int GetModelSize(const BYTE* lpbyModel)
{
	if (memcmp(lpbyModel, "CYHD", 4) != 0)
		return -1;

	lpbyModel += 4;

	return *((int*)lpbyModel);
}
