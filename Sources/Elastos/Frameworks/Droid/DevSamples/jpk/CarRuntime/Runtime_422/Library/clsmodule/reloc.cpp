//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include <elaobj.h>
#include <clsbase.h>

static int s_nBase;

void MapClassDescriptor(ClassDescriptor *p)
{
    int n;

    if (0 != p->cAggregates)
        p->pAggrIndexs = (USHORT *)((int)p->pAggrIndexs + s_nBase);
    if (0 != p->cAspects)
        p->pAspectIndexs = (USHORT *)((int)p->pAspectIndexs + s_nBase);
    if (0 != p->cClasses)
        p->pClassIndexs = (USHORT *)((int)p->pClassIndexs + s_nBase);

    if (0 != p->cInterfaces) {
        p->ppInterfaces = \
            (ClassInterface **)((int)p->ppInterfaces + s_nBase);

        for (n = 0; n < p->cInterfaces; n++) {
            p->ppInterfaces[n] = \
                (ClassInterface *)((int)p->ppInterfaces[n] + s_nBase);
        }
    }
}

void MapClassDirEntry(ClassDirEntry *p)
{
    p->pszName += s_nBase;
    if (p->pszNameSpace) p->pszNameSpace += s_nBase;

    p->pDesc = (ClassDescriptor *)((int)p->pDesc + s_nBase);

    MapClassDescriptor(p->pDesc);
}

void MapInterfaceConstDescriptor(InterfaceConstDescriptor *p)
{
    p->pszName += s_nBase;
    if (p->type == TYPE_STRING && p->v.pStrValue != NULL) p->v.pStrValue += s_nBase;
}

void MapMethodDescriptor(MethodDescriptor *p)
{
    int n;

    p->pszName += s_nBase;

    if (0 != p->cParams) {
        p->ppParams = (ParamDescriptor **)((int)p->ppParams + s_nBase);

        for (n = 0; n < p->cParams; n++) {
            p->ppParams[n] = \
                (ParamDescriptor *)((int)p->ppParams[n] + s_nBase);
            p->ppParams[n]->pszName += s_nBase;

            if (p->ppParams[n]->type.pNestedType) {
                p->ppParams[n]->type.pNestedType = (TypeDescriptor *) \
                    ((int)p->ppParams[n]->type.pNestedType + s_nBase);
            }
        }
    }
}

void MapInterfaceDescriptor(InterfaceDescriptor *p)
{
    int n;

    if (0 != p->cConsts) {
        p->ppConsts = (InterfaceConstDescriptor **)((int)p->ppConsts + s_nBase);

        for (n = 0; n < p->cConsts; n++) {
            p->ppConsts[n] = \
                (InterfaceConstDescriptor *)((int)p->ppConsts[n] + s_nBase);
            MapInterfaceConstDescriptor(p->ppConsts[n]);
        }
    }

    if (0 != p->cMethods) {
        p->ppMethods = (MethodDescriptor **)((int)p->ppMethods + s_nBase);

        for (n = 0; n < p->cMethods; n++) {
            p->ppMethods[n] = \
                (MethodDescriptor *)((int)p->ppMethods[n] + s_nBase);
            MapMethodDescriptor(p->ppMethods[n]);
        }
    }
}

void MapInterfaceDirEntry(InterfaceDirEntry *p)
{
    p->pszName += s_nBase;
    if (p->pszNameSpace) p->pszNameSpace += s_nBase;

    p->pDesc = (InterfaceDescriptor *)((int)p->pDesc + s_nBase);

    MapInterfaceDescriptor(p->pDesc);
}

void MapArrayDirEntry(ArrayDirEntry *p)
{
    if (p->pszNameSpace) p->pszNameSpace += s_nBase;

    if (p->type.pNestedType) {
        p->type.pNestedType = (TypeDescriptor *) \
            ((int)p->type.pNestedType + s_nBase);
    }
}

void MapStructDescriptor(StructDescriptor *p)
{
    int n;

    if (0 != p->cElems) {
        p->ppElems = (StructElement **)((int)p->ppElems + s_nBase);

        for (n = 0; n < p->cElems; n++) {
            p->ppElems[n] = (StructElement *)((int)p->ppElems[n] + s_nBase);
            p->ppElems[n]->pszName += s_nBase;

            if (p->ppElems[n]->type.pNestedType) {
                p->ppElems[n]->type.pNestedType = (TypeDescriptor *) \
                    ((int)p->ppElems[n]->type.pNestedType + s_nBase);
            }
        }
    }
}

void MapStructDirEntry(StructDirEntry *p)
{
    p->pszName += s_nBase;
    if (p->pszNameSpace) p->pszNameSpace += s_nBase;

    p->pDesc = (StructDescriptor *)((int)p->pDesc + s_nBase);

    MapStructDescriptor(p->pDesc);
}

void MapEnumDescriptor(EnumDescriptor *p)
{
    int n;

    if (0 != p->cElems) {
        p->ppElems = (EnumElement **)((int)p->ppElems + s_nBase);

        for (n = 0; n < p->cElems; n++) {
            p->ppElems[n] = (EnumElement *)((int)p->ppElems[n] + s_nBase);
            p->ppElems[n]->pszName += s_nBase;
        }
    }
}

void MapEnumDirEntry(EnumDirEntry *p)
{
    p->pszName += s_nBase;
    if (p->pszNameSpace) p->pszNameSpace += s_nBase;

    p->pDesc = (EnumDescriptor *)((int)p->pDesc + s_nBase);

    MapEnumDescriptor(p->pDesc);
}

void MapConstDirEntry(ConstDirEntry *p)
{
    p->pszName += s_nBase;
    if (p->pszNameSpace) p->pszNameSpace += s_nBase;
    if (p->type == TYPE_STRING && p->v.strValue.pszValue != NULL) p->v.strValue.pszValue += s_nBase;
}

void MapAliasDirEntry(AliasDirEntry *p)
{
    p->pszName += s_nBase;
    if (p->pszNameSpace) p->pszNameSpace += s_nBase;

    if (p->type.pNestedType) {
        p->type.pNestedType = \
            (TypeDescriptor *)((int)p->type.pNestedType + s_nBase);
    }
}

void DoRelocCLS(CLSModule *p)
{
    int n;

    s_nBase = (int)p;

    p->pszName += s_nBase;
    if (p->pszUunm) p->pszUunm += s_nBase;
    if (p->pszServiceName) p->pszServiceName += s_nBase;
    if (p->cDefinedInterfaces) {
        p->pDefinedInterfaceIndex =
            (int *)((int)(p->pDefinedInterfaceIndex) + s_nBase);
    }

    if (0 != p->cClasses) {
        p->ppClassDir = (ClassDirEntry **)((int)p->ppClassDir + s_nBase);

        for (n = 0; n < p->cClasses; n++) {
            p->ppClassDir[n] = \
                (ClassDirEntry *)((int)p->ppClassDir[n] + s_nBase);
            MapClassDirEntry(p->ppClassDir[n]);
        }
    }
    else {
        p->ppClassDir = NULL;
    }

    if (0 != p->cInterfaces) {
        p->ppInterfaceDir = \
            (InterfaceDirEntry **)((int)p->ppInterfaceDir + s_nBase);

        for (n = 0; n < p->cInterfaces; n++) {
            p->ppInterfaceDir[n] = \
                (InterfaceDirEntry *)((int)p->ppInterfaceDir[n] + s_nBase);
            MapInterfaceDirEntry(p->ppInterfaceDir[n]);
        }
    }
    else {
        p->ppInterfaceDir = NULL;
    }

    if (0 != p->cArrays) {
        p->ppArrayDir = (ArrayDirEntry **)((int)p->ppArrayDir + s_nBase);

        for (n = 0; n < p->cArrays; n++) {
            p->ppArrayDir[n] = \
                (ArrayDirEntry *)((int)p->ppArrayDir[n] + s_nBase);
            MapArrayDirEntry(p->ppArrayDir[n]);
        }
    }
    else {
        p->ppArrayDir = NULL;
    }

    if (0 != p->cStructs) {
        p->ppStructDir = (StructDirEntry **)((int)p->ppStructDir + s_nBase);

        for (n = 0; n < p->cStructs; n++) {
            p->ppStructDir[n] = \
                (StructDirEntry *)((int)p->ppStructDir[n] + s_nBase);
            MapStructDirEntry(p->ppStructDir[n]);
        }
    }
    else {
        p->ppStructDir = NULL;
    }

    if (0 != p->cEnums) {
        p->ppEnumDir = (EnumDirEntry **)((int)p->ppEnumDir + s_nBase);

        for (n = 0; n < p->cEnums; n++) {
            p->ppEnumDir[n] = \
                (EnumDirEntry *)((int)p->ppEnumDir[n] + s_nBase);
            MapEnumDirEntry(p->ppEnumDir[n]);
        }
    }
    else {
        p->ppEnumDir = NULL;
    }

    if (0 != p->cConsts) {
        p->ppConstDir = (ConstDirEntry **)((int)p->ppConstDir + s_nBase);

        for (n = 0; n < p->cConsts; n++) {
            p->ppConstDir[n] = \
                (ConstDirEntry *)((int)p->ppConstDir[n] + s_nBase);
            MapConstDirEntry(p->ppConstDir[n]);
        }
    }
    else {
        p->ppConstDir = NULL;
    }

    if (0 != p->cAliases) {
        p->ppAliasDir = (AliasDirEntry **)((int)p->ppAliasDir + s_nBase);

        for (n = 0; n < p->cAliases; n++) {
            p->ppAliasDir[n] = \
                (AliasDirEntry *)((int)p->ppAliasDir[n] + s_nBase);
            MapAliasDirEntry(p->ppAliasDir[n]);
        }
    }
    else {
        p->ppAliasDir = NULL;
    }

    if (0 != p->cLibraries) {
        p->ppLibNames = (char **)((int)p->ppLibNames + s_nBase);
        for (n = 0; n < p->cLibraries; n++) {
            p->ppLibNames[n] += s_nBase;
        }
    }
    else {
        p->ppLibNames = NULL;
    }
}

int RelocFlattedCLS(const void *pvSrc, int nSize, CLSModule **ppDest)
{
    int n;
    CLSModule *pSrc, *pDest;

    pSrc = (CLSModule *)pvSrc;

    pDest = (CLSModule *)new char[pSrc->nSize];
    if (!pDest) _ReturnError (CLSError_OutOfMemory);

    if (pSrc->dwAttribs & CARAttrib_compress) {
        n = UncompressCLS(pvSrc, nSize, pDest);
        if (n != pSrc->nSize) {
            delete [] (char *)pDest;
            _ReturnError (CLSError_FormatSize);
        }
    }
    else {
        memcpy(pDest, pSrc, nSize);
    }

    DoRelocCLS(pDest);
    pDest->dwAttribs |= CARAttrib_inheap;

    *ppDest = pDest;
    _ReturnOK (CLS_NoError);
}

int DisposeFlattedCLS(void *pvDest)
{
    if (((CLSModule *)pvDest)->dwAttribs & CARAttrib_inheap) {
        delete [] (char *)pvDest;
    }
    _ReturnOK (CLS_NoError);
}
