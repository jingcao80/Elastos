//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include <stdio.h>

#include <guidseed.h>

DWORD GetWordValue(const DictWord *pDict, const char *pszSeed, DWORD *pValue)
{
    DWORD n = 0;

    while (0 != pDict[n].nSize) {
        if (!strncmp(pDict[n].pszWord, pszSeed, pDict[n].nSize)) {
            *pValue = n + 64;
            return pDict[n].nSize;
        }
        n++;
    }
    *pValue = EncodeCSymbol(*pszSeed);
    return 1;
}

DWORD GetWordByValue(const DictWord *pDict, DWORD dwValue, char *pResult)
{
    if (dwValue < 64) {
        *pResult = (char)DecodeCSymbol(dwValue);
        return 1;
    }
    dwValue -= 64;
    memcpy(pResult, pDict[dwValue].pszWord, pDict[dwValue].nSize);
    return pDict[dwValue].nSize;
}

IMPL_GUID_ENCODE(StaticDict)(
    void *pvArg, const char *pszSeed, DWORD *pResult)
{
    DWORD dwValue, dwOffset;
    DWORD cCount, cMatched, cEncoded;
    DWORD *pPrevResult, dwPrevOffset;

    *pResult = EncodeCSymbol(*pszSeed++);
    cCount = 0;
    cEncoded = 1;
    dwOffset = dwPrevOffset = 6;
    pPrevResult = pResult;

    while (0 != *pszSeed) {
        cMatched = GetWordValue((DictWord *)pvArg, pszSeed, &dwValue);
        pszSeed += cMatched;
        cEncoded += cMatched;

        *pResult |= dwValue << dwOffset;
        dwOffset += 9;

        if (dwOffset >= 32) {
            if (2 == cCount) break;
            cCount++;
            pResult++;
            dwOffset -= 32;
            *pResult |= dwValue >> (9 - dwOffset);
        }
    }

    if (cCount < 2) {
        while (TRUE) {
            dwValue = (*pPrevResult >> dwPrevOffset) & 0x1ff;
            dwPrevOffset += 9;
            if (dwPrevOffset >= 32) {
                pPrevResult++;
                dwPrevOffset -= 32;
                dwValue |= (*pPrevResult << (9 - dwPrevOffset)) & 0x1ff;
            }

            *pResult |= dwValue << dwOffset;
            dwOffset += 9;

            if (dwOffset >= 32) {
                if (2 == cCount) break;
                cCount++;
                pResult++;
                dwOffset -= 32;
                *pResult |= dwValue >> (9 - dwOffset);
            }
        }
    }

    return cEncoded;
}

IMPL_GUID_DECODE(StaticDict)(
    void *pvArg, const DWORD *pSource, char *pszResult)
{
    DWORD cCount, dwOffset, dwValue;

    *pszResult++ = (char)DecodeCSymbol(*pSource & 0x3f);
    cCount = 0;
    dwOffset = 6;

    while (TRUE) {
        dwValue = (*pSource >> dwOffset) & 0x1ff;
        dwOffset += 9;
        if (dwOffset >= 32) {
            if (2 == cCount) break;
            cCount++;
            pSource++;
            dwOffset -= 32;
            dwValue |= (*pSource << (9 - dwOffset)) & 0x1ff;
        }
        pszResult += GetWordByValue((DictWord *)pvArg, dwValue, pszResult);
    }

    *pszResult = 0;
    return 0;
}
