//==========================================================================
// Copyright (c) 2000-2009,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include "parser.h"
#include <clsdef.h>

typedef struct AttribEntry {
    DWORD           dwValue;
    const char*     mName;
}   AttribEntry;

#define _ATTRIB_ENTRY(obj, attrib) { obj##Attrib_##attrib, #attrib }
#define _TYPE_ENTRY(obj, type) { obj##Attrib_t_##type, #type }

static AttribEntry s_aModuleAttribs[] = {
_ATTRIB_ENTRY(CAR, hasSinkObject),
};

static AttribEntry s_aModuleTypes[] = {
_ATTRIB_ENTRY(CAR, library),
};

static AttribEntry s_aClassAttribs[] = {
_ATTRIB_ENTRY(Class, freethreaded),
_ATTRIB_ENTRY(Class, naked),
_ATTRIB_ENTRY(Class, singleton),
_ATTRIB_ENTRY(Class, aggregate),
_ATTRIB_ENTRY(Class, aspect),
_ATTRIB_ENTRY(Class, specialAspect),
_ATTRIB_ENTRY(Class, hascallback),
_ATTRIB_ENTRY(Class, hascoalescence),
_ATTRIB_ENTRY(Class, hasvirtual),
_ATTRIB_ENTRY(Class, hasctor),
_ATTRIB_ENTRY(Class, hasDefaultCtor),
_ATTRIB_ENTRY(Class, hasTrivialCtor),
_ATTRIB_ENTRY(Class, hasparent),
_ATTRIB_ENTRY(Class, hasasynchronous),
_ATTRIB_ENTRY(Class, private),
_ATTRIB_ENTRY(Class, local),
_ATTRIB_ENTRY(Class, hasfilter),
};
#define s_aAspectAttribs s_aClassAttribs
#define s_aSuperAttribs s_aClassAttribs

static AttribEntry s_aClassTypes[] = {
_TYPE_ENTRY(Class, aspect),
_TYPE_ENTRY(Class, generic),
_TYPE_ENTRY(Class, regime),
_TYPE_ENTRY(Class, sink),
_TYPE_ENTRY(Class, clsobj),
};
#define s_aAspectTypes s_aClassTypes
#define s_aSuperTypes s_aClassTypes

static AttribEntry s_aClsIntfAttribs[] = {
_ATTRIB_ENTRY(ClassInterface, virtual),
_ATTRIB_ENTRY(ClassInterface, callback),
_ATTRIB_ENTRY(ClassInterface, sink),
_ATTRIB_ENTRY(ClassInterface, hidden),
_ATTRIB_ENTRY(ClassInterface, privileged),
_ATTRIB_ENTRY(ClassInterface, handler),
_ATTRIB_ENTRY(ClassInterface, inherited),
_ATTRIB_ENTRY(ClassInterface, autoadded),
_ATTRIB_ENTRY(ClassInterface, filter),
_ATTRIB_ENTRY(ClassInterface, async),
_ATTRIB_ENTRY(ClassInterface, delegate),
_ATTRIB_ENTRY(ClassInterface, callbacksink),
_ATTRIB_ENTRY(ClassInterface, delegatesink),
};

static AttribEntry s_aInterfaceAttribs[] = {
_ATTRIB_ENTRY(Interface, local),
_ATTRIB_ENTRY(Interface, sink),
_ATTRIB_ENTRY(Interface, dual),
_ATTRIB_ENTRY(Interface, clsobj),
_ATTRIB_ENTRY(Interface, generic),
_ATTRIB_ENTRY(Interface, defined),
};
const int c_cIntfAttribs = sizeof(s_aInterfaceAttribs) / sizeof(AttribEntry);
#define s_aParentAttribs s_aInterfaceAttribs

static AttribEntry s_aInterfaceTypes[] = {
_TYPE_ENTRY(Interface, normal),
_TYPE_ENTRY(Interface, callbacks),
_TYPE_ENTRY(Interface, delegates),
};
const int c_cIntfTypes = sizeof(s_aInterfaceTypes) / sizeof(AttribEntry);
#define s_aParentTypes s_aInterfaceTypes

static AttribEntry s_aClassMethodAttribs[] = {
_ATTRIB_ENTRY(Method, TrivialCtor),
_ATTRIB_ENTRY(Method, DefaultCtor),
_ATTRIB_ENTRY(Method, NonDefaultCtor),
_ATTRIB_ENTRY(Method, WithCoalescence),
};

static AttribEntry s_aIntfMethodAttribs[] = {
_ATTRIB_ENTRY(Method, TrivialCtor),
_ATTRIB_ENTRY(Method, DefaultCtor),
_ATTRIB_ENTRY(Method, NonDefaultCtor),
_ATTRIB_ENTRY(Method, WithCoalescence),
};

static AttribEntry s_aParamAttribs[] = {
_ATTRIB_ENTRY(Param, in),
_ATTRIB_ENTRY(Param, out),
};

typedef struct ObjectAttribs {
    ObjectType      object;
    const char*     mName;
    AttribEntry *   pAttribs;
    int             cAttribs;
}   ObjectAttribs;

#define _OBJECT_ATTRIBS_ENTRY(obj) \
{ \
Object_##obj, #obj, s_a##obj##Attribs, \
sizeof(s_a##obj##Attribs) / sizeof(AttribEntry) \
}

#define _OBJECT_TYPES_ENTRY(obj) \
{ \
Object_##obj, #obj, s_a##obj##Types, \
sizeof(s_a##obj##Types) / sizeof(AttribEntry) \
}

static const ObjectAttribs s_attribTable[] = {
_OBJECT_ATTRIBS_ENTRY(Module),
_OBJECT_ATTRIBS_ENTRY(Class),
_OBJECT_ATTRIBS_ENTRY(Aspect),
_OBJECT_ATTRIBS_ENTRY(Super),
_OBJECT_ATTRIBS_ENTRY(Interface),
_OBJECT_ATTRIBS_ENTRY(ClsIntf),
_OBJECT_ATTRIBS_ENTRY(Parent),
_OBJECT_ATTRIBS_ENTRY(ClassMethod),
_OBJECT_ATTRIBS_ENTRY(IntfMethod),
_OBJECT_ATTRIBS_ENTRY(Param),
};
const int c_cAttribEntry = sizeof(s_attribTable) / sizeof(ObjectAttribs);

static const ObjectAttribs s_typeTable[] = {
_OBJECT_TYPES_ENTRY(Module),
_OBJECT_TYPES_ENTRY(Class),
_OBJECT_TYPES_ENTRY(Aspect),
_OBJECT_TYPES_ENTRY(Super),
_OBJECT_TYPES_ENTRY(Interface),
_OBJECT_TYPES_ENTRY(Parent),
};
const int c_cTypeEntry = sizeof(s_typeTable) / sizeof(ObjectAttribs);

void CheckObjectAttrib(PSTATEDESC pDesc, const char *pszConst)
{
    int n, m;
    AttribEntry *pAttribs;

    for (n = 0; n < c_cAttribEntry; n++) {
        if (pDesc->object == s_attribTable[n].object) {
            for (m = 0; m < s_attribTable[n].cAttribs; m++) {
                pAttribs = &s_attribTable[n].pAttribs[m];
                if (!_stricmp(pszConst, pAttribs->mName)) {
                    pDesc->dwExtra |= pAttribs->dwValue;
                    return;
                }
            }
            if (Object_ClsIntf == pDesc->object) {
                for (m = 0; m < c_cIntfAttribs; m++) {
                    if (!_stricmp(pszConst, s_aInterfaceAttribs[m].mName)) {
                        pDesc->object = Object_Interface;
                        pDesc->dwExtra |= s_aInterfaceAttribs[m].dwValue;
                        return;
                    }
                }
            }
            ErrorReport(Lube_E_HasNoAttrib,
                        s_attribTable[n].mName, pszConst);
            return;
        }
    }

    if (Object_Typedef == pDesc->object) {
        if (!_stricmp(pszConst, "dummytype")) {
            pDesc->dwExtra = TRUE;
            return;
        }
    }
    if (Object_Enum == pDesc->object) {
        if (!_stricmp(pszConst, "callback")) {
            pDesc->dwExtra = TRUE;
            return;
        }
    }
    ErrorReport(Lube_E_InvalidCondMem);
}

int CheckObjectType(PSTATEDESC pDesc, const char *pszConst)
{
    int n, m;
    AttribEntry *pAttribs;

    for (n = 0; n < c_cAttribEntry; n++) {
        if (pDesc->object == s_typeTable[n].object) {
            for (m = 0; m < s_typeTable[n].cAttribs; m++) {
                pAttribs = &s_typeTable[n].pAttribs[m];
                if (!_stricmp(pszConst, pAttribs->mName)) {
                    pDesc->dwExtra |= pAttribs->dwValue;
                    return LUBE_OK;
                }
            }
            ErrorReport(Lube_E_HasNoType,
                        s_attribTable[n].mName, pszConst);
            return LUBE_OK;
        }
    }
    if (Object_ClsIntf == pDesc->object) {
        for (m = 0; m < c_cIntfTypes; m++) {
            if (!_stricmp(pszConst, s_aInterfaceTypes[m].mName)) {
                pDesc->object = Object_Interface;
                pDesc->dwExtra |= s_aInterfaceTypes[m].dwValue;
                return LUBE_OK;
            }
        }
        ErrorReport(Lube_E_HasNoType, "interface", pszConst);
        return LUBE_OK;
    }

    return LUBE_FAIL;
}

int CheckMemberCondition(PSTATEDESC pDesc, const char *pszConst)
{
    switch (pDesc->member) {
        case Member_Type:
            if (CheckObjectType(pDesc, pszConst) >= 0) {
                break;
            }
            // else - same as Member_Name: save the const string
        case Member_Name:
            pDesc->uDataSize = strlen(pszConst) + 1;
            pDesc->pvData = (PVOID)new char[pDesc->uDataSize];
            if (!pDesc->pvData) {
                ErrorReport(Lube_E_OutOfMemory);
                return Ret_AbortOnError;
            }
            strcpy((char *)pDesc->pvData, pszConst);
            break;
        case Member_Attrib:
            CheckObjectAttrib(pDesc, pszConst);
            break;
        default:
            ErrorReport(Lube_E_InvalidCondMem);
            break;
    }
    return Ret_Continue;
}
