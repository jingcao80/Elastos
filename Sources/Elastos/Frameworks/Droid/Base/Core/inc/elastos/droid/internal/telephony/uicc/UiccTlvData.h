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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_UICCTLVDATA_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_UICCTLVDATA_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

/**
  * UICC TLV Data Parser according to ETSI TS 102 221 spec.
  */
class UiccTlvData
    : public Object
    , public IUiccTlvData
{
public:
    CAR_INTERFACE_DECL();

    UiccTlvData();

    CARAPI constructor();

    virtual CARAPI IsIncomplete(
        /* [out] */ Boolean* result);

    static CARAPI_(Boolean) IsUiccTlvData(
        /* [in] */ ArrayOf<Byte>* data);

    static CARAPI_(AutoPtr<IUiccTlvData>) Parse(
        /* [in] */ ArrayOf<Byte>* data);

private:
    CARAPI_(Int32) ParseFileSize(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 currentLocation);

    CARAPI_(Int32) ParseSomeTag(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 currentLocation);

    CARAPI_(Int32) ParseFileDescriptor(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 currentLocation);

public:
    Int32 mRecordSize;
    Int32 mFileSize;
    Int32 mNumRecords;
    Boolean mIsDataEnough;

private:
    static const String LOGTAG;
    static const Int32 TLV_FORMAT_ID = 0x62;
    static const Int32 TAG_FILE_DESCRIPTOR = 0x82;
    static const Int32 TAG_FILE_IDENTIFIER = 0x83;
    static const Int32 TAG_PROPRIETARY_INFO = 0xA5;
    static const Int32 TAG_LIFECYCLE_STATUS = 0x8A;
    static const Int32 TAG_SECURITY_ATTR_1 = 0x8B;
    static const Int32 TAG_SECURITY_ATTR_2 = 0x8C;
    static const Int32 TAG_SECURITY_ATTR_3 = 0xAB;
    static const Int32 TAG_FILE_SIZE = 0x80;
    static const Int32 TAG_TOTAL_FILE_SIZE = 0x81;
    static const Int32 TAG_SHORT_FILE_IDENTIFIER = 0x88;
    static const Int32 TYPE_5 = 5;
    static const Int32 TYPE_2 = 2;
    Int32 mFileType;
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_UICCTLVDATA_H__
