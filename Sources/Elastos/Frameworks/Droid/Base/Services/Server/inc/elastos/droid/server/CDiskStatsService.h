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

#ifndef __ELASTOS_DROID_SERVER_CDISKSTATSSERVICE_H__
#define __ELASTOS_DROID_SERVER_CDISKSTATSSERVICE_H__

#include "_Elastos_Droid_Server_CDiskStatsService.h"
#include <elastos/core/Object.h>
#include <Elastos.Droid.Content.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Content::IContext;
using Elastos::IO::IFile;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Server {

/**
 * This service exists only as a "dumpsys" target which reports
 * statistics about the status of the disk.
 */
CarClass(CDiskStatsService)
    , public Object
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    //@Override
    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    CARAPI_(void) ReportFreeSpace(
        /* [in] */ IFile* path,
        /* [in] */ const String& name,
        /* [in] */ IPrintWriter* pw);

private:
    const static String TAG;
    AutoPtr<IContext> mContext;

};

}//Server
}//Droid
}//Elastos

#endif //__ELASTOS_DROID_SERVER_CDISKSTATSSERVICE_H__
