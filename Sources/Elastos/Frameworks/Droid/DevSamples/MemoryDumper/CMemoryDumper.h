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


#ifndef __ELASTOS_MEMORYDEUMPER_CMEMORYDUMPER_H__
#define __ELASTOS_MEMORYDEUMPER_CMEMORYDUMPER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/List.h>

using Elastos::Core::Object;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace MemoryDumper {

class CMemoryDumper
{
public:
    static CARAPI SendDumpRequest();

    static CARAPI OutputBackTrace(
        /* [in] */ const String& diffFile,
        /* [in] */ Int32 pid);

private:
    class MapInfoManager
        : public Object
    {
    public:
        struct MapInfo {
            struct MapInfo* next;
            unsigned start;
            unsigned end;
            char name[];
        };

        MapInfoManager(
            /* [in] */ Int32 pid);

        ~MapInfoManager();

        MapInfo* miList;

        CARAPI InitMapInfo(
            /* [in] */ Int32 pid);

        void DeinitMapInfo();

        const char *Map2Name(
            /* [in] */ unsigned pc,
            /* [in] */ const char* def);

        const MapInfo *Pc2Mapinfo(
            /* [in] */ unsigned pc,
            /* [out] */ unsigned *rel_pc);

    private:
        MapInfo* ParseMapsLine(
            /* [in] */ char* line);
    };

    static CARAPI ProcessAllocEntry(
        /* [in] */ char* oneLine,
        /* [in] */ Int32 pid);

    static CARAPI Print(
        /* [in] */ List<String>* list,
        /* [in] */ Int32 pid);
};

}
}

#endif // __ELASTOS_MEMORYDEUMPER_CMEMORYDUMPER_H__
