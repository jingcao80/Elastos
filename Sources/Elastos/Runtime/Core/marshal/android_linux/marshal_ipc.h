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

#ifndef __MARSHAL_H__
#define __MARSHAL_H__

#include <binder/Binder.h>
#include <elastos.h>
#include <marshal_comm.h>

namespace Elastos {
namespace IPC {

typedef struct InterfacePack
{
    EMuid               mClsid;            // clsid of object
    UInt32              mIndex;           // interface index in class
    android::sp<android::IBinder> mBinder;// binder object of stub
} InterfacePack;

extern ECode StdMarshalInterface(
    /* [in] */ IInterface* object,
    /* [out] */ InterfacePack* interfacePack);

extern ECode StdUnmarshalInterface(
    /* [in] */ UnmarshalFlag flag,
    /* [in] */ InterfacePack* interfacePack,
    /* [out] */ IInterface** object);

} // namespace IPC
} // namespace Elastos

#endif //__MARSHAL_H__
