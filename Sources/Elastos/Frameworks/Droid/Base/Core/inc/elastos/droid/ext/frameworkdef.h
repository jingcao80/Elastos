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

#ifndef __ELASTOS_DROID_FRAMEWORKDEF_H__
#define __ELASTOS_DROID_FRAMEWORKDEF_H__

#ifndef __ELASTOS_CORE_DEF_H__
#include <elastos/coredef.h>
#endif

#include "Elastos.Droid.Core.h"

#ifndef TO_ATTRS_ARRAYOF
#define TO_ATTRS_ARRAYOF(attrs) (ArrayOf<Int32>::AllocInplace(const_cast<Int32 *>(attrs), ArraySize(attrs)))
#endif

// #define MAX_PATH    256

// #define XML_NAMESPACE   "http://schemas.elastos.com/capsule/res/elastos"

#ifndef DUMP_CLSID
#define DUMP_CLSID(intf) \
    do { \
        ClassID clsid; \
        IObject* obj = IObject::Probe(intf); \
        assert(obj != NULL); \
        obj->GetClassID(&clsid); \
        printf("======== DUMP_CLSID ========\n"); \
        printf("{%p, %p, %p, {%p, %p, %p, %p, %p, %p, %p, %p} }\n", \
                clsid.clsid.Data1, clsid.clsid.Data2, clsid.clsid.Data3, \
                clsid.clsid.Data4[0], clsid.clsid.Data4[1], \
                clsid.clsid.Data4[2], clsid.clsid.Data4[3], \
                clsid.clsid.Data4[4], clsid.clsid.Data4[5], \
                clsid.clsid.Data4[6], clsid.clsid.Data4[7]); \
        printf("============================\n"); \
    } while(0);
#endif

#ifndef DUMP_ITFID
#define DUMP_ITFID(intf) \
    do { \
        InterfaceID iid; \
        intf->GetInterfaceID(intf, &iid); \
        printf("======== DUMP_ITFID ========\n"); \
        printf("{%p, %p, %p, {%p, %p, %p, %p, %p, %p, %p, %p} }\n", \
                iid.Data1, iid.Data2, iid.Data3, \
                iid.Data4[0], iid.Data4[1], \
                iid.Data4[2], iid.Data4[3], \
                iid.Data4[4], iid.Data4[5], \
                iid.Data4[6], iid.Data4[7]); \
        printf("============================\n"); \
    } while(0);
#endif


#endif //__ELASTOS_DROID_FRAMEWORKDEF_H__
