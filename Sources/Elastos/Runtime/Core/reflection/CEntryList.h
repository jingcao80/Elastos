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

#ifndef __CENTRYLIST_H__
#define __CENTRYLIST_H__

#include "CClsModule.h"
#include "CObjInfoList.h"

struct ObjElement
{
    UInt32  mIndex;
    void*   mDesc;
    char*   mName;
    char*   mNamespaceOrSignature;
    IInterface* mObject;
};

typedef enum IFAttrib
{
    IFAttrib_normal         = 0x01,
    IFAttrib_callback       = 0x02,
}IFAttrib;

struct IFIndexEntry
{
    UInt32                  mIndex;
    UInt32                  mBeginNo;
    char*                   mName;
    char*                   mNameSpace;
    InterfaceDescriptor*    mDesc;
    UInt32                  mAttribs;
};

typedef ArrayOf<IInterface *>*  PTypeInfos;

class CClassInfo;

class CEntryList : public ElLightRefBase
{
public:
    CEntryList(
        /* [in] */ EntryType type,
        /* [in] */ void* desc,
        /* [in] */ UInt32 totalCount,
        /* [in] */ CClsModule* clsModule,
        /* [in] */ IFIndexEntry* ifList = NULL,
        /* [in] */ UInt32 listCount = 0,
        /* [in] */ CClassInfo* clsInfo = NULL);

    ~CEntryList();

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI InitElemList();

    CARAPI AcquireObjByName(
        /* [in] */ const String& name,
        /* [out] */ IInterface** object);

    CARAPI AcquireObjByIndex(
        /* [in] */ UInt32 index,
        /* [out] */ IInterface** object);

    CARAPI GetAllObjInfos(
        /* [out] */ ArrayOf<IInterface *>* objInfos);

public:
    AutoPtr<CClsModule> mClsModule;
    UInt32              mTotalCount;

private:
    CLSModule*          mClsMod;
    void*               mDesc;
    EntryType           mType;
    CClassInfo*         mClsInfo;
    ObjElement*         mObjElement;
    Int32               mBase;

    IFIndexEntry*       mIFList;
    UInt32              mListCount;

    HashTable<UInt32, Type_UInt32> mHTIndexs;
};

#endif // __CENTRYLIST_H__
