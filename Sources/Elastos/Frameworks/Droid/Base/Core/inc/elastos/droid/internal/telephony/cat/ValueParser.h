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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_VALUEPARSER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_VALUEPARSER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"
#include "elastos/droid/internal/telephony/cat/ComprehensionTlv.h"
#include "elastos/droid/internal/telephony/cat/CommandDetails.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

class ValueParser
    : public Object
{
public:
    /**
      * Search for a Command Details object from a list.
      *
      * @param ctlv List of ComprehensionTlv objects used for search
      * @return An CtlvCommandDetails object found from the objects. If no
      *         Command Details object is found, ResultException is thrown.
      * @throws ResultException
      */
    static CARAPI_(AutoPtr<CommandDetails>) RetrieveCommandDetails(
        /* [in] */ ComprehensionTlv* ctlv);

    /**
      * Search for a Device Identities object from a list.
      *
      * @param ctlv List of ComprehensionTlv objects used for search
      * @return An CtlvDeviceIdentities object found from the objects. If no
      *         Command Details object is found, ResultException is thrown.
      * @throws ResultException
      */
    static CARAPI_(AutoPtr<DeviceIdentities>) RetrieveDeviceIdentities(
        /* [in] */ ComprehensionTlv* ctlv);

    /**
      * Retrieves Duration information from the Duration COMPREHENSION-TLV
      * object.
      *
      * @param ctlv A Text Attribute COMPREHENSION-TLV object
      * @return A Duration object
      * @throws ResultException
      */
    static CARAPI_(AutoPtr<IDuration>) RetrieveDuration(
        /* [in] */ ComprehensionTlv* ctlv);

    /**
      * Retrieves Item information from the COMPREHENSION-TLV object.
      *
      * @param ctlv A Text Attribute COMPREHENSION-TLV object
      * @return An Item
      * @throws ResultException
      */
    static CARAPI_(AutoPtr<IItem>) RetrieveItem(
        /* [in] */ ComprehensionTlv* ctlv);

    /**
      * Retrieves Item id information from the COMPREHENSION-TLV object.
      *
      * @param ctlv A Text Attribute COMPREHENSION-TLV object
      * @return An Item id
      * @throws ResultException
      */
    static CARAPI_(Int32) RetrieveItemId(
        /* [in] */ ComprehensionTlv* ctlv);

    /**
      * Retrieves icon id from an Icon Identifier COMPREHENSION-TLV object
      *
      * @param ctlv An Icon Identifier COMPREHENSION-TLV object
      * @return IconId instance
      * @throws ResultException
      */
    static CARAPI_(AutoPtr<IconId>) RetrieveIconId(
        /* [in] */ ComprehensionTlv* ctlv);

    /**
      * Retrieves item icons id from an Icon Identifier List COMPREHENSION-TLV
      * object
      *
      * @param ctlv An Item Icon List Identifier COMPREHENSION-TLV object
      * @return ItemsIconId instance
      * @throws ResultException
      */
    static CARAPI_(AutoPtr<ItemsIconId>) RetrieveItemsIconId(
        /* [in] */ ComprehensionTlv* ctlv);

    /**
      * Retrieves text attribute information from the Text Attribute
      * COMPREHENSION-TLV object.
      *
      * @param ctlv A Text Attribute COMPREHENSION-TLV object
      * @return A list of TextAttribute objects
      * @throws ResultException
      */
    static CARAPI_(AutoPtr<IList/*< AutoPtr<TextAttribute> > */>) RetrieveTextAttribute(
        /* [in] */ ComprehensionTlv* ctlv);

    /**
      * Retrieves alpha identifier from an Alpha Identifier COMPREHENSION-TLV
      * object.
      *
      * @param ctlv An Alpha Identifier COMPREHENSION-TLV object
      * @return String corresponding to the alpha identifier
      * @throws ResultException
      */
    static CARAPI_(String) RetrieveAlphaId(
        /* [in] */ ComprehensionTlv* ctlv);

    /**
      * Retrieves text from the Text COMPREHENSION-TLV object, and decodes it
      * into a Java String.
      *
      * @param ctlv A Text COMPREHENSION-TLV object
      * @return A Java String object decoded from the Text object
      * @throws ResultException
      */
    static CARAPI_(String) RetrieveTextString(
        /* [in] */ ComprehensionTlv* ctlv);

    static CARAPI_(Int32) RetrieveTarget(
        /* [in] */ ComprehensionTlv* ctlv);

    /**
      * Samsung STK: Read SMSC Address
      *
      * @param ctlv A SMSC Address COMPREHENSION-TLV object
      * @return A Java String object decoded from the SMSC Address object
      * @throws ResultException
      */
    static CARAPI_(String) RetrieveSMSCaddress(
        /* [in] */ ComprehensionTlv* ctlv);

    /**
      * Samsung STK: Read SMS TPDU Address
      *
      * @param ctlv A SMS TPDU COMPREHENSION-TLV object
      * @return A Java String object decoded from the SMS TPDU object
      * @throws ResultException
      */
    static CARAPI_(String) RetrieveSMSTPDU(
        /* [in] */ ComprehensionTlv* ctlv);

    /**
      * Samsung STK: Read USSD String
      *
      * @param ctlv A USSD String COMPREHENSION-TLV object
      * @return A String object decoded from the USSD String object
      * @throws ResultException
      */
    static CARAPI_(String) RetrieveUSSDString(
        /* [in] */ ComprehensionTlv* ctlv);
};

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_VALUEPARSER_H__
