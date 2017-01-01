
#include <lube.h>

int CheckUserFuncArg(int nIndex, PSTATEDESC pDesc)
{
    assert(nIndex < c_cUserFuncs);
    assert(State_Func == pDesc->type || State_Condition == pDesc->type);

    ObjectType object;
    MemberType member;

    if (pDesc->pvData) {
        return ARGTYPE_STRING == \
                g_userFuncs[nIndex].dwArgType ? LUBE_OK:LUBE_FAIL;
    }

    object = (ObjectType)(g_userFuncs[nIndex].dwArgType >> 16);
    member = (MemberType)(g_userFuncs[nIndex].dwArgType & 0xffff);

    if (ARGTYPE_STRING == g_userFuncs[nIndex].dwArgType) {
        if (Member_Name == pDesc->member
            || Member_FullName == pDesc->member
            || Member_Uunm == pDesc->member) {
            return LUBE_OK;
        }
        return LUBE_FAIL;
    }
    else if (member != pDesc->member) {
        if (Member_Uuid != member) return LUBE_FAIL;
        if (Member_Clsid != pDesc->member && Member_Iid != pDesc->member) {
            return LUBE_FAIL;
        }
    }

    if (object != Object_None)
        switch (object) {
            case Object_Class:
                if (Object_Class != pDesc->object
                    && Object_Super != pDesc->object
                    && Object_Aspect != pDesc->object
                    && Object_Aggregate != pDesc->object
                    && Object_ClassForAspect != pDesc->object) {
                    return LUBE_FAIL;
                }
                break;
            case Object_Interface:
                if (Object_Interface != pDesc->object
                    && Object_Parent != pDesc->object) {
                    return LUBE_FAIL;
                }
                break;
            case Object_IntfMethod:
                if (Object_IntfMethod != pDesc->object
                    && Object_ClassMethod != pDesc->object
                    && Object_CParentMtd != pDesc->object
                    && Object_IParentMtd != pDesc->object) {
                    return LUBE_FAIL;
                }
                break;
            default:
                if (object != pDesc->object) return LUBE_FAIL;
                break;
        }

    return LUBE_OK;
}
