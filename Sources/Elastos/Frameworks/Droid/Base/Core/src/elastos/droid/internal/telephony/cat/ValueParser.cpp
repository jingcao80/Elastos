
#include "Elastos.Droid.Internal.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.CoreLibrary.Utility.Concurrent.h"
#include "elastos/droid/internal/telephony/cat/ValueParser.h"
#include "elastos/droid/internal/telephony/cat/CatLog.h"
#include "elastos/droid/internal/telephony/cat/CatService.h"
#include "elastos/droid/internal/telephony/cat/CDuration.h"
#include "elastos/droid/internal/telephony/cat/CItem.h"
#include "elastos/droid/internal/telephony/cat/CTextAttribute.h"
#include "elastos/droid/internal/telephony/CGsmAlphabet.h"
#include "elastos/droid/internal/telephony/uicc/CIccUtils.h"
#include "elastos/droid/content/res/CResourcesHelper.h"
#include "elastos/droid/R.h"

#include <elastos/core/StringUtils.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IResourcesHelper;
using Elastos::Droid::Content::Res::CResourcesHelper;
using Elastos::Droid::Internal::Telephony::IGsmAlphabet;
using Elastos::Droid::Internal::Telephony::CGsmAlphabet;
using Elastos::Droid::Internal::Telephony::Uicc::IIccUtils;
using Elastos::Droid::Internal::Telephony::Uicc::CIccUtils;
using Elastos::Droid::R;

using Elastos::Core::StringUtils;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

//=====================================================================
//                             ValueParser
//=====================================================================
AutoPtr<CommandDetails> ValueParser::RetrieveCommandDetails(
    /* [in] */ ComprehensionTlv* ctlv)
{
    AutoPtr<CommandDetails> cmdDet = new CommandDetails();
    AutoPtr<ArrayOf<Byte> > rawValue;
    ctlv->GetRawValue((ArrayOf<Byte>**)&rawValue);
    Int32 valueIndex = 0;
    ctlv->GetValueIndex(&valueIndex);
    // try {
        ctlv->IsComprehensionRequired(&(cmdDet->mCompRequired));
        cmdDet->mCommandNumber = (*rawValue)[valueIndex] & 0xff;
        cmdDet->mTypeOfCommand = (*rawValue)[valueIndex + 1] & 0xff;
        cmdDet->mCommandQualifier = (*rawValue)[valueIndex + 2] & 0xff;
        return cmdDet;
    // } catch (IndexOutOfBoundsException e) {
    //     throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
    // }
}

AutoPtr<DeviceIdentities> ValueParser::RetrieveDeviceIdentities(
    /* [in] */ ComprehensionTlv* ctlv)
{
    AutoPtr<DeviceIdentities> devIds = new DeviceIdentities();
    AutoPtr<ArrayOf<Byte> > rawValue;
    ctlv->GetRawValue((ArrayOf<Byte>**)&rawValue);
    Int32 valueIndex = 0;
    ctlv->GetValueIndex(&valueIndex);
    // try {
        devIds->mSourceId = (*rawValue)[valueIndex] & 0xff;
        devIds->mDestinationId = (*rawValue)[valueIndex + 1] & 0xff;
        return devIds;
    // } catch (IndexOutOfBoundsException e) {
    //     throw new ResultException(ResultCode.REQUIRED_VALUES_MISSING);
    // }
}

AutoPtr<IDuration> ValueParser::RetrieveDuration(
    /* [in] */ ComprehensionTlv* ctlv)
{
    Int32 timeInterval = 0;
    IDurationTimeUnit timeUnit = TimeUnit_SECOND;

    AutoPtr<ArrayOf<Byte> > rawValue;
    ctlv->GetRawValue((ArrayOf<Byte>**)&rawValue);
    Int32 valueIndex = 0;
    ctlv->GetValueIndex(&valueIndex);

    // try {
        Int32 index = (*rawValue)[valueIndex] & 0xff;
        timeUnit = index;
        timeInterval = (*rawValue)[valueIndex + 1] & 0xff;
    // } catch (IndexOutOfBoundsException e) {
    //     throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
    // }
    AutoPtr<IDuration> p;
    CDuration::New(timeInterval, timeUnit, (IDuration**)&p);
    return p;
}

AutoPtr<IItem> ValueParser::RetrieveItem(
    /* [in] */ ComprehensionTlv* ctlv)
{
    AutoPtr<IItem> item;

    AutoPtr<ArrayOf<Byte> > rawValue;
    ctlv->GetRawValue((ArrayOf<Byte>**)&rawValue);
    Int32 valueIndex = 0;
    ctlv->GetValueIndex(&valueIndex);
    Int32 length = 0;
    ctlv->GetLength(&length);

    if (length != 0) {
        Int32 textLen = length - 1;

        // try {
            Int32 id = (*rawValue)[valueIndex] & 0xff;
            AutoPtr<Elastos::Droid::Internal::Telephony::Uicc::IIccUtils> iccu;
            CIccUtils::AcquireSingleton((Elastos::Droid::Internal::Telephony::Uicc::IIccUtils**)&iccu);
            String text;
            iccu->AdnStringFieldToString(rawValue,
                    valueIndex + 1, textLen, &text);
            CItem::New(id, text, (IItem**)&item);
        // } catch (IndexOutOfBoundsException e) {
        //     throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
        // }
    }

    return item;
}

Int32 ValueParser::RetrieveItemId(
    /* [in] */ ComprehensionTlv* ctlv)
{
    Int32 id = 0;

    AutoPtr<ArrayOf<Byte> > rawValue;
    ctlv->GetRawValue((ArrayOf<Byte>**)&rawValue);
    Int32 valueIndex = 0;
    ctlv->GetValueIndex(&valueIndex);

    // try {
        id = (*rawValue)[valueIndex] & 0xff;
    // } catch (IndexOutOfBoundsException e) {
    //     throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
    // }

    return id;
}

AutoPtr<IconId> ValueParser::RetrieveIconId(
    /* [in] */ ComprehensionTlv* ctlv)
{
    AutoPtr<IconId> id = new IconId();

    AutoPtr<ArrayOf<Byte> > rawValue;
    ctlv->GetRawValue((ArrayOf<Byte>**)&rawValue);
    Int32 valueIndex = 0;
    ctlv->GetValueIndex(&valueIndex);
    // try {
        id->mSelfExplanatory = ((*rawValue)[valueIndex++] & 0xff) == 0x00;
        id->mRecordNumber = (*rawValue)[valueIndex] & 0xff;
    // } catch (IndexOutOfBoundsException e) {
    //     throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
    // }

    return id;
}

AutoPtr<ItemsIconId> ValueParser::RetrieveItemsIconId(
    /* [in] */ ComprehensionTlv* ctlv)
{
    CatLog::D(String("ValueParser"), String("retrieveItemsIconId:"));
    AutoPtr<ItemsIconId> id = new ItemsIconId();

    AutoPtr<ArrayOf<Byte> > rawValue;
    ctlv->GetRawValue((ArrayOf<Byte>**)&rawValue);
    Int32 valueIndex = 0;
    ctlv->GetValueIndex(&valueIndex);
    Int32 numOfItems = 0;
    ctlv->GetLength(&numOfItems);
    numOfItems -= 1;
    id->mRecordNumbers = ArrayOf<Int32>::Alloc(numOfItems);

    // try {
        // get icon self-explanatory
        id->mSelfExplanatory = ((*rawValue)[valueIndex++] & 0xff) == 0x00;

        for (Int32 index = 0; index < numOfItems;) {
            (*(id->mRecordNumbers))[index++] = (*rawValue)[valueIndex++];
        }
    // } catch (IndexOutOfBoundsException e) {
    //     throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
    // }
    return id;
}

AutoPtr<IList/*< AutoPtr<TextAttribute> >*/> ValueParser::RetrieveTextAttribute(
    /* [in] */ ComprehensionTlv* ctlv)
{
    AutoPtr<IArrayList> lst;
    CArrayList::New((IArrayList**)&lst);

    AutoPtr<ArrayOf<Byte> > rawValue;
    ctlv->GetRawValue((ArrayOf<Byte>**)&rawValue);
    Int32 valueIndex = 0;
    ctlv->GetValueIndex(&valueIndex);
    Int32 length = 0;
    ctlv->GetLength(&length);

    if (length != 0) {
        // Each attribute is consisted of four bytes
        Int32 itemCount = length / 4;

        // try {
            for (Int32 i = 0; i < itemCount; i++, valueIndex += 4) {
                Int32 start = (*rawValue)[valueIndex] & 0xff;
                Int32 textLength = (*rawValue)[valueIndex + 1] & 0xff;
                Int32 format = (*rawValue)[valueIndex + 2] & 0xff;
                Int32 colorValue = (*rawValue)[valueIndex + 3] & 0xff;

                Int32 alignValue = format & 0x03;
                TextAlignment align = alignValue;

                Int32 sizeValue = (format >> 2) & 0x03;
                FontSize size = sizeValue;
                assert(0 && "TODO");
                // if (size == NULL) {
                //     // Font size value is not defined. Use default.
                //     size = FontSize.NORMAL;
                // }

                Boolean bold = (format & 0x10) != 0;
                Boolean italic = (format & 0x20) != 0;
                Boolean underlined = (format & 0x40) != 0;
                Boolean strikeThrough = (format & 0x80) != 0;

                TextColor color = colorValue;

                AutoPtr<ITextAttribute> attr;
                CTextAttribute::New(start, textLength,
                        align, size, bold, italic, underlined,
                        strikeThrough, color, (ITextAttribute**)&attr);
                lst->Add(attr);
            }

            return IList::Probe(lst);

        // } catch (IndexOutOfBoundsException e) {
        //     throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
        // }
    }
    return NULL;
}

String ValueParser::RetrieveAlphaId(
    /* [in] */ ComprehensionTlv* ctlv)
{
    if (ctlv != NULL) {
        AutoPtr<ArrayOf<Byte> > rawValue;
        ctlv->GetRawValue((ArrayOf<Byte>**)&rawValue);
        Int32 valueIndex = 0;
        ctlv->GetValueIndex(&valueIndex);
        Int32 length = 0;
        ctlv->GetLength(&length);
        if (length != 0) {
            // try {
            AutoPtr<Elastos::Droid::Internal::Telephony::Uicc::IIccUtils> iccu;
            CIccUtils::AcquireSingleton((Elastos::Droid::Internal::Telephony::Uicc::IIccUtils**)&iccu);
            String res;
            iccu->AdnStringFieldToString(rawValue, valueIndex,
                    length, &res);
            return res;
            // } catch (IndexOutOfBoundsException e) {
            //     throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
            // }
        }
        else {
            CatLog::D(String("ValueParser"), String("Alpha Id length=") + StringUtils::ToString(length));
            return String(NULL);
        }
    }
    else {
        /* Per 3GPP specification 102.223,
         * if the alpha identifier is not provided by the UICC,
         * the terminal MAY give information to the user
         * noAlphaUsrCnf defines if you need to show user confirmation or not
         */
        Boolean noAlphaUsrCnf = FALSE;
        AutoPtr<IResourcesHelper> hlp;
        CResourcesHelper::AcquireSingleton((IResourcesHelper**)&hlp);
        AutoPtr<IResources> resource;
        hlp->GetSystem((IResources**)&resource);
        // try {
            resource->GetBoolean(
                    R::bool_::config_stkNoAlphaUsrCnf, &noAlphaUsrCnf);
        // } catch (NotFoundException e) {
        //     noAlphaUsrCnf = false;
        // }
        return (noAlphaUsrCnf ? String(NULL) : CatService::STK_DEFAULT);
    }
    return String(NULL);
}

String ValueParser::RetrieveTextString(
    /* [in] */ ComprehensionTlv* ctlv)
{
    AutoPtr<ArrayOf<Byte> > rawValue;
    ctlv->GetRawValue((ArrayOf<Byte>**)&rawValue);
    Int32 valueIndex = 0;
    ctlv->GetValueIndex(&valueIndex);
    Byte codingScheme = 0x00;
    String text(NULL);
    Int32 textLen = 0;
    ctlv->GetLength(&textLen);

    // In case the text length is 0, return a NULL string.
    if (textLen == 0) {
        return text;
    }
    else {
        // one byte is coding scheme
        textLen -= 1;
    }

    // try {
        codingScheme = (Byte) ((*rawValue)[valueIndex] & 0x0c);

        AutoPtr<IGsmAlphabet> gsm;
        CGsmAlphabet::AcquireSingleton((IGsmAlphabet**)&gsm);
        if (codingScheme == 0x00) { // GSM 7-bit packed
            gsm->Gsm7BitPackedToString(rawValue,
                    valueIndex + 1, (textLen * 8) / 7, &text);
        }
        else if (codingScheme == 0x04) { // GSM 8-bit unpacked
            gsm->Gsm8BitUnpackedToString(rawValue,
                    valueIndex + 1, textLen, &text);
        }
        else if (codingScheme == 0x08) { // UCS2
            text = String(*rawValue, valueIndex + 1, textLen); // , String("UTF-16")
        }
        else {
            // throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
        }

        return text;
    // } catch (IndexOutOfBoundsException e) {
    //     throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
    // } catch (UnsupportedEncodingException e) {
    //     // This should never happen.
    //     throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
    // }
}

Int32 ValueParser::RetrieveTarget(
    /* [in] */ ComprehensionTlv* ctlv)
{
    AutoPtr<ActivateDescriptor> activateDesc = new ActivateDescriptor();
    AutoPtr<ArrayOf<Byte> > rawValue;
    ctlv->GetRawValue((ArrayOf<Byte>**)&rawValue);
    Int32 valueIndex = 0;
    ctlv->GetValueIndex(&valueIndex);
    // try {
        activateDesc->mTarget = (*rawValue)[valueIndex] & 0xff;
        return activateDesc->mTarget;
    // } catch (IndexOutOfBoundsException e) {
    //     throw new ResultException(ResultCode.REQUIRED_VALUES_MISSING);
    // }
    // return 0;
}

String ValueParser::RetrieveSMSCaddress(
    /* [in] */ ComprehensionTlv* ctlv)
{
    AutoPtr<ArrayOf<Byte> > rawValue;
    ctlv->GetRawValue((ArrayOf<Byte>**)&rawValue);
    Int32 valueIndex = 0;
    ctlv->GetValueIndex(&valueIndex);
    Int32 length = 0;
    ctlv->GetLength(&length);
    AutoPtr<ArrayOf<Byte> > outputValue = ArrayOf<Byte>::Alloc(length + 1);

    for (Int32 k = 0; k <= length; k++) {
        // try {
            (*outputValue)[k] = (*rawValue)[k + (valueIndex - 1)];
        // }
        // catch (IndexOutOfBoundsException indexoutofboundsexception) {
        //     throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
        // }
    }
    if (length != 0) {
        AutoPtr<Elastos::Droid::Internal::Telephony::Uicc::IIccUtils> iccu;
        CIccUtils::AcquireSingleton((Elastos::Droid::Internal::Telephony::Uicc::IIccUtils**)&iccu);
        String res;
        iccu->BytesToHexString(outputValue, &res);
        return res;
    }
    else {
        // throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
        return String(NULL);
    }
}

String ValueParser::RetrieveSMSTPDU(
    /* [in] */ ComprehensionTlv* ctlv)
{
    AutoPtr<ArrayOf<Byte> > rawValue;
    ctlv->GetRawValue((ArrayOf<Byte>**)&rawValue);
    Int32 valueIndex = 0;
    ctlv->GetValueIndex(&valueIndex);
    Int32 pduLength = 0;
    ctlv->GetLength(&pduLength);
    AutoPtr<ArrayOf<Byte> > outputValue;
    Int32 k = 0;
    String result;
    if ((*rawValue)[valueIndex + 2] % 2 == 0) {
        k = (*rawValue)[valueIndex + 2] / 2;
    }
    else {
        k = (1 + (*rawValue)[valueIndex + 2]) / 2;
    }

    if (pduLength == k + 6) {
        outputValue = ArrayOf<Byte>::Alloc(pduLength + 1);
    }
    else {
        outputValue = ArrayOf<Byte>::Alloc(pduLength);
    }

    for (Int32 l = 0; l < pduLength; l++) {
        // try {
            (*outputValue)[l] = (*rawValue)[valueIndex + l];
        // }
        // catch (IndexOutOfBoundsException ex) {
        //     throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
        // }
    }
    if (pduLength != 0) {
        AutoPtr<Elastos::Droid::Internal::Telephony::Uicc::IIccUtils> iccu;
        CIccUtils::AcquireSingleton((Elastos::Droid::Internal::Telephony::Uicc::IIccUtils**)&iccu);
        iccu->BytesToHexString(outputValue, &result);
    }
    else {
        // throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
    }

    return result;
}

String ValueParser::RetrieveUSSDString(
    /* [in] */ ComprehensionTlv* ctlv)
{
    AutoPtr<ArrayOf<Byte> > rawValue;
    ctlv->GetRawValue((ArrayOf<Byte>**)&rawValue);
    Int32 valueIndex = 0;
    ctlv->GetValueIndex(&valueIndex);
    Int32 length = 0;
    ctlv->GetLength(&length);

    // If length is 0 (shouldn't be), return NULL
    if (length == 0) {
        return String(NULL);
    }

    // Should be 0x0f
    if ((*rawValue)[valueIndex] != 0x0f) {
        // throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
    }

    // try {
        AutoPtr<IGsmAlphabet> gsm;
        CGsmAlphabet::AcquireSingleton((IGsmAlphabet**)&gsm);
        String res;
        gsm->Gsm7BitPackedToString(rawValue,
                valueIndex + 1, ((length - 1) * 8) / 7, &res);
        return res;
    // } catch (IndexOutOfBoundsException e) {
    //     throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD);
    // }
}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
