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
#include <string.h>
#include "prefix.h"

char* GenerateCamelPrefix(const char* pszType)
{
    int nLength = strlen(pszType);
    bool bIsLastUpperCase = false;
    char cLastUpperChar = '\0';
    char* pszPrefix = new char[nLength];
    int nIndex = 0;
    for (int i = 0; i < nLength; i++) {
        char c = *(pszType + i);
        if (c <= 'Z' && c >= 'A') {
            if (!bIsLastUpperCase) {
                pszPrefix[nIndex++] = c + ('a' - 'A');
            }
            else {
                cLastUpperChar = c;
            }
            bIsLastUpperCase = true;
        }
        else {
            if (c <= 'z' && c >= 'a' && bIsLastUpperCase) {
                if (cLastUpperChar != '\0') {
                    pszPrefix[nIndex++] = cLastUpperChar + ('a' - 'A');
                    cLastUpperChar = '\0';
                }
            }
            bIsLastUpperCase = false;
        }
    }
    if (nIndex > 0) {
        char* pszPrefixCopy = new char[nIndex + 1];
        pszPrefixCopy[nIndex] = '\0';
        memcpy(pszPrefixCopy, pszPrefix, nIndex);
        delete[] pszPrefix;
        return pszPrefixCopy;
    }
    else {
        delete[] pszPrefix;
        return NULL;
    }
}

/**
 * Return prefix variable name with the given prefix and variable name.
 * If returned prefix is not NULL, user must release it manually!
 */
char* GeneratePrefixalVarName(const char* pszPrefix, const char* pszVariable)
{
    if (pszPrefix != NULL) {
        int nLength = strlen(pszPrefix);
        char* pszName = new char[nLength + strlen(pszVariable) + 1];
        strcpy(pszName, pszPrefix);
        strcat(pszName, pszVariable);
        char c = *(pszName + nLength);
        if (c >= 'a' && c <= 'z') {
            *(pszName + nLength) = c - 'a' + 'A';
        }
        *(pszName + (nLength + strlen(pszVariable))) = '\0';
        return pszName;
    }
    else {
        char* pszName = new char[strlen(pszVariable) + 1];
        strcpy(pszName, pszVariable);
        *(pszName + strlen(pszVariable)) = '\0';
        return pszName;
    }
}

/**
 * Return the true type name of given type. Basically this
 * method just remove the "_" tail.
 * User must release the returned string!
 */
char* TrimOutTypeNameBar(const char* pszType)
{
    int nLength = strlen(pszType);
    if (nLength != 0 && *(pszType + nLength - 1) == '_') {
        char* pszNewType = new char[nLength - 1];
        memcpy(pszNewType, pszType, nLength - 1);
        return pszNewType;
    }
    char* pszNewType = new char[nLength];
    strcpy(pszNewType, pszType);
    return pszNewType;
}

/**
 * Return the true type name of given type. Basically this
 * method just remove the "_" tail.
 * User must release the returned string!
 */
char* GetTypeName(const char* pszType)
{
    int nLength = strlen(pszType);
    int nIndex = 0;
    for (int i = 0; i < nLength; i++) {
        if (*(pszType + i) == ' ') {
            if (i == 4) {
                if (*(pszType) == 'e' && *(pszType + 1) == 'n' &&
                    *(pszType + 2) == 'u' && *(pszType + 3) == 'm') {
                    nIndex = i + 1;
                    continue;
                }
            }
            else if (i == 6) {
                if (*(pszType) == 's' && *(pszType + 1) == 't' &&
                    *(pszType + 2) == 'r' && *(pszType + 3) == 'u' &&
                    *(pszType + 4) == 'c' && *(pszType + 5) == 't') {
                    nIndex = i + 1;
                    continue;
                }
            }
            nLength = i;
            break;
        }
    }
    char* pszNewType = new char[nLength - nIndex + 1];
    strncpy(pszNewType, pszType + nIndex, nLength - nIndex);
    pszNewType[nLength - nIndex] = '\0';
    return pszNewType;
}
