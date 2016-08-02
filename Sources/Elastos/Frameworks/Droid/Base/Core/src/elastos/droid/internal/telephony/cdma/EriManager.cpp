#include "Elastos.Droid.Internal.h"
#include "Elastos.CoreLibrary.Utility.h"

#include "elastos/droid/internal/telephony/cdma/CEriInfo.h"
#include "elastos/droid/internal/telephony/cdma/EriManager.h"
#include "elastos/droid/internal/utility/XmlUtils.h"
#include "elastos/droid/R.h"
#include "elastos/droid/utility/Xml.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Droid::R;
using Elastos::Droid::Utility::Xml;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::IO::CFileInputStream;
using Elastos::IO::ICloseable;
using Elastos::IO::IFileInputStream;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Logging::Logger;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

//=====================================================================
//                         EriManager::EriFile
//=====================================================================
EriManager::EriFile::EriFile()
    : mVersionNumber(-1)
    , mNumberOfEriEntries(0)
    , mEriFileType(-1)
{
    mCallPromptId = ArrayOf<String>::Alloc(3);
    (*mCallPromptId)[0] = String("");
    (*mCallPromptId)[1] = String("");
    (*mCallPromptId)[2] = String("");
    CHashMap::New((IHashMap**)&mRoamIndTable);
}

//=====================================================================
//                  EriManager::EriDisplayInformation
//=====================================================================
EriManager::EriDisplayInformation::EriDisplayInformation(
    /* [in] */ Int32 eriIconIndex,
    /* [in] */ Int32 eriIconMode,
    /* [in] */ const String& eriIconText)
    : mEriIconIndex(eriIconIndex)
    , mEriIconMode(eriIconMode)
    , mEriIconText(eriIconText)
{
}

ECode EriManager::EriDisplayInformation::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String("EriDisplayInformation: {")
            + " IconIndex: " + StringUtils::ToString(mEriIconIndex)
            + " EriIconMode: " + StringUtils::ToString(mEriIconMode)
            + " EriIconText: " + mEriIconText + " }";
    return NOERROR;
}

//=====================================================================
//                              EriManager
//=====================================================================
CAR_INTERFACE_IMPL(EriManager, Object, IEriManager);

const Int32 EriManager::ERI_FROM_XML;
const Int32 EriManager::ERI_FROM_FILE_SYSTEM;
const Int32 EriManager::ERI_FROM_MODEM;
const String EriManager::LOGTAG("CDMA");
const Boolean EriManager::DBG = TRUE;
const Boolean EriManager::VDBG = FALSE;

EriManager::EriManager()
    : mEriFileSource(ERI_FROM_XML)
    , mIsEriFileLoaded(FALSE)
{
}

ECode EriManager::constructor(
    /* [in] */ IPhoneBase* phone,
    /* [in] */ IContext* context,
    /* [in] */ Int32 eriFileSource)
{
    mContext = context;
    mEriFileSource = eriFileSource;
    mEriFile = new EriFile();
    return NOERROR;
}

ECode EriManager::Dispose()
{
    mEriFile = new EriFile();
    mIsEriFileLoaded = FALSE;
    return NOERROR;
}

ECode EriManager::LoadEriFile()
{
    switch (mEriFileSource) {
    case ERI_FROM_MODEM:
        LoadEriFileFromModem();
        break;

    case ERI_FROM_FILE_SYSTEM:
        LoadEriFileFromFileSystem();
        break;

    case ERI_FROM_XML:
    default:
        LoadEriFileFromXml();
        break;
    }
    return NOERROR;
}

ECode EriManager::GetEriFileVersion(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mEriFile->mVersionNumber;
    return NOERROR;
}

ECode EriManager::GetEriNumberOfEntries(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mEriFile->mNumberOfEriEntries;
    return NOERROR;
}

ECode EriManager::GetEriFileType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mEriFile->mEriFileType;
    return NOERROR;
}

ECode EriManager::IsEriFileLoaded(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIsEriFileLoaded;
    return NOERROR;
}

ECode EriManager::GetCdmaEriIconIndex(
    /* [in] */ Int32 roamInd,
    /* [in] */ Int32 defRoamInd,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = GetEriDisplayInformation(roamInd, defRoamInd)->mEriIconIndex;
    return NOERROR;
}

ECode EriManager::GetCdmaEriIconMode(
    /* [in] */ Int32 roamInd,
    /* [in] */ Int32 defRoamInd,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = GetEriDisplayInformation(roamInd, defRoamInd)->mEriIconMode;
    return NOERROR;
}

ECode EriManager::GetCdmaEriText(
    /* [in] */ Int32 roamInd,
    /* [in] */ Int32 defRoamInd,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = GetEriDisplayInformation(roamInd, defRoamInd)->mEriIconText;
    return NOERROR;
}

void EriManager::LoadEriFileFromModem()
{
    // NOT IMPLEMENTED, Chipset vendor/Operator specific
}

void EriManager::LoadEriFileFromFileSystem()
{
    // NOT IMPLEMENTED, Chipset vendor/Operator specific
}

void EriManager::LoadEriFileFromXml()
{
    AutoPtr<IXmlPullParser> parser;
    AutoPtr<IFileInputStream> stream;
    AutoPtr<IResources> r;
    mContext->GetResources((IResources**)&r);

    // try {
    if (DBG) Logger::D(LOGTAG, "loadEriFileFromXml: check for alternate file");
    String str;
    r->GetString(R::string::alternate_eri_file, &str);
    CFileInputStream::New(str, (IFileInputStream**)&stream);
    Xml::NewPullParser((IXmlPullParser**)&parser);
    parser->SetInput(IInputStream::Probe(stream), String(NULL));
    if (DBG) Logger::D(LOGTAG, "loadEriFileFromXml: opened alternate file");
    // } catch (FileNotFoundException e) {
    //     if (DBG) Logger::D(LOGTAG, "loadEriFileFromXml: no alternate file");
    //     parser = NULL;
    // } catch (XmlPullParserException e) {
    //     if (DBG) Logger::D(LOGTAG, "loadEriFileFromXml: no parser for alternate file");
    //     parser = NULL;
    // }

    if (parser == NULL) {
        if (DBG) Logger::D(LOGTAG, "loadEriFileFromXml: open normal file");
        AutoPtr<IXmlResourceParser> p;
        r->GetXml(R::xml::eri, (IXmlResourceParser**)&p);
        parser = IXmlPullParser::Probe(p);
    }

    // try {
    XmlUtils::BeginDocument(parser, String("EriFile"));
    parser->GetAttributeValue(String(NULL), String("VersionNumber"), &str);
    mEriFile->mVersionNumber = StringUtils::ParseInt32(str);
    parser->GetAttributeValue(String(NULL), String("NumberOfEriEntries"), &str);
    mEriFile->mNumberOfEriEntries = StringUtils::ParseInt32(str);
    parser->GetAttributeValue(String(NULL), String("EriFileType"), &str);
    mEriFile->mEriFileType = StringUtils::ParseInt32(str);

    Int32 parsedEriEntries = 0;
    while(TRUE) {
        XmlUtils::NextElement(parser);
        String name;
        parser->GetName(&name);
        if (name == NULL) {
            if (parsedEriEntries != mEriFile->mNumberOfEriEntries)
                Logger::E(LOGTAG, "Error Parsing ERI file: %d defined, %d parsed!",
                        mEriFile->mNumberOfEriEntries, parsedEriEntries);
            break;
        }
        else if (name.Equals("CallPromptId")) {
            parser->GetAttributeValue(String(NULL), String("Id"), &str);
            Int32 id = StringUtils::ParseInt32(str);
            String text;
            parser->GetAttributeValue(String(NULL), String("CallPromptText"), &text);
            if (id >= 0 && id <= 2) {
                (*mEriFile->mCallPromptId)[id] = text;
            }
            else {
                Logger::E(LOGTAG, "Error Parsing ERI file: found %d CallPromptId", id);
            }

        }
        else if (name.Equals("EriInfo")) {
            parser->GetAttributeValue(String(NULL), String("RoamingIndicator"), &str);
            Int32 roamingIndicator = StringUtils::ParseInt32(str);

            parser->GetAttributeValue(String(NULL), String("IconIndex"), &str);
            Int32 iconIndex = StringUtils::ParseInt32(str);

            parser->GetAttributeValue(String(NULL), String("IconMode"), &str);
            Int32 iconMode = StringUtils::ParseInt32(str);

            String eriText;
            parser->GetAttributeValue(String(NULL), String("EriText"), &eriText);

            parser->GetAttributeValue(String(NULL), String("CallPromptId"), &str);
            Int32 callPromptId = StringUtils::ParseInt32(str);

            parser->GetAttributeValue(String(NULL), String("AlertId"), &str);
            Int32 alertId = StringUtils::ParseInt32(str);

            parsedEriEntries++;
            AutoPtr<IEriInfo> eriInfo;
            CEriInfo::New(roamingIndicator, iconIndex, iconMode, eriText,
                    callPromptId, alertId, (IEriInfo**)&eriInfo);

            mEriFile->mRoamIndTable->Put(CoreUtils::Convert(roamingIndicator), eriInfo);
        }
    }

    if (DBG) Logger::D(LOGTAG, "loadEriFileFromXml: eri parsing successful, file loaded");
    mIsEriFileLoaded = TRUE;

    // } catch (Exception e) {
    //     Logger::E(LOGTAG, "Got exception while loading ERI file.", e);
    // } finally {
    if (IXmlResourceParser::Probe(parser) != NULL) {
        IXmlResourceParser::Probe(parser)->Close();
    }
    // try {
    if (stream != NULL) {
        ICloseable::Probe(stream)->Close();
    }
    // } catch (IOException e) {
    //     // Ignore
    // }
    // }
}

AutoPtr<IEriInfo> EriManager::GetEriInfo(
    /* [in] */ Int32 roamingIndicator)
{
    Boolean b;
    if (mEriFile->mRoamIndTable->ContainsKey(CoreUtils::Convert(roamingIndicator), &b), b) {
        AutoPtr<IInterface> obj;
        mEriFile->mRoamIndTable->Get(CoreUtils::Convert(roamingIndicator), (IInterface**)&obj);
        return IEriInfo::Probe(obj);
    }
    else {
        return NULL;
    }
}

AutoPtr<EriManager::EriDisplayInformation> EriManager::GetEriDisplayInformation(
    /* [in] */ Int32 roamInd,
    /* [in] */ Int32 defRoamInd)
{
    AutoPtr<EriDisplayInformation> ret;

    // Carrier can use eri.xml to customize any built-in roaming display indications
    if (mIsEriFileLoaded) {
        AutoPtr<EriInfo> eriInfo = (EriInfo*)GetEriInfo(roamInd).Get();
        if (eriInfo != NULL) {
            if (VDBG) Logger::V(LOGTAG, "ERI roamInd %d found in ERI file", &roamInd);
            ret = new EriDisplayInformation(
                    eriInfo->iconIndex,
                    eriInfo->iconMode,
                    eriInfo->eriText);
            return ret;
        }
    }

    AutoPtr<ICharSequence> cs;
    String str;
    switch (roamInd) {
    // Handling the standard roaming indicator (non-ERI)
    case IEriInfo::ROAMING_INDICATOR_ON:
        mContext->GetText(R::string::roamingText0, (ICharSequence**)&cs);
        cs->ToString(&str);
        ret = new EriDisplayInformation(
                IEriInfo::ROAMING_INDICATOR_ON,
                IEriInfo::ROAMING_ICON_MODE_NORMAL, str);
        break;

    case IEriInfo::ROAMING_INDICATOR_OFF:
        mContext->GetText(R::string::roamingText1, (ICharSequence**)&cs);
        cs->ToString(&str);
        ret = new EriDisplayInformation(
                IEriInfo::ROAMING_INDICATOR_OFF,
                IEriInfo::ROAMING_ICON_MODE_NORMAL, str);
        break;

    case IEriInfo::ROAMING_INDICATOR_FLASH:
        mContext->GetText(R::string::roamingText2, (ICharSequence**)&cs);
        cs->ToString(&str);
        ret = new EriDisplayInformation(
                IEriInfo::ROAMING_INDICATOR_FLASH,
                IEriInfo::ROAMING_ICON_MODE_FLASH, str);
        break;

    // Handling the standard ERI
    case 3:
        mContext->GetText(R::string::roamingText3, (ICharSequence**)&cs);
        cs->ToString(&str);
        ret = new EriDisplayInformation(
                roamInd,
                IEriInfo::ROAMING_ICON_MODE_NORMAL, str);
        break;

    case 4:
        mContext->GetText(R::string::roamingText4, (ICharSequence**)&cs);
        cs->ToString(&str);
        ret = new EriDisplayInformation(
                roamInd,
                IEriInfo::ROAMING_ICON_MODE_NORMAL, str);
        break;

    case 5:
        mContext->GetText(R::string::roamingText5, (ICharSequence**)&cs);
        cs->ToString(&str);
        ret = new EriDisplayInformation(
                roamInd,
                IEriInfo::ROAMING_ICON_MODE_NORMAL, str);
        break;

    case 6:
        mContext->GetText(R::string::roamingText6, (ICharSequence**)&cs);
        cs->ToString(&str);
        ret = new EriDisplayInformation(
                roamInd,
                IEriInfo::ROAMING_ICON_MODE_NORMAL, str);
        break;

    case 7:
        mContext->GetText(R::string::roamingText7, (ICharSequence**)&cs);
        cs->ToString(&str);
        ret = new EriDisplayInformation(
                roamInd,
                IEriInfo::ROAMING_ICON_MODE_NORMAL, str);
        break;

    case 8:
        mContext->GetText(R::string::roamingText8, (ICharSequence**)&cs);
        cs->ToString(&str);
        ret = new EriDisplayInformation(
                roamInd,
                IEriInfo::ROAMING_ICON_MODE_NORMAL, str);
        break;

    case 9:
        mContext->GetText(R::string::roamingText9, (ICharSequence**)&cs);
        cs->ToString(&str);
        ret = new EriDisplayInformation(
                roamInd,
                IEriInfo::ROAMING_ICON_MODE_NORMAL, str);
        break;

    case 10:
        mContext->GetText(R::string::roamingText10, (ICharSequence**)&cs);
        cs->ToString(&str);
        ret = new EriDisplayInformation(
                roamInd,
                IEriInfo::ROAMING_ICON_MODE_NORMAL, str);
        break;

    case 11:
        mContext->GetText(R::string::roamingText11, (ICharSequence**)&cs);
        cs->ToString(&str);
        ret = new EriDisplayInformation(
                roamInd,
                IEriInfo::ROAMING_ICON_MODE_NORMAL, str);
        break;

    case 12:
        mContext->GetText(R::string::roamingText12, (ICharSequence**)&cs);
        cs->ToString(&str);
        ret = new EriDisplayInformation(
                roamInd,
                IEriInfo::ROAMING_ICON_MODE_NORMAL, str);
        break;

    // Handling the non standard Enhanced Roaming Indicator (roamInd > 63)
    default:
        if (!mIsEriFileLoaded) {
            // ERI file NOT loaded
            if (DBG) Logger::D(LOGTAG, "ERI File not loaded");
            if(defRoamInd > 2) {
                if (VDBG) Logger::V(LOGTAG, "ERI defRoamInd > 2 ...flashing");
                mContext->GetText(R::string::roamingText2, (ICharSequence**)&cs);
                cs->ToString(&str);
                ret = new EriDisplayInformation(
                        IEriInfo::ROAMING_INDICATOR_FLASH,
                        IEriInfo::ROAMING_ICON_MODE_FLASH, str);
            }
            else {
                if (VDBG) Logger::V(LOGTAG, "ERI defRoamInd <= 2");
                switch (defRoamInd) {
                case IEriInfo::ROAMING_INDICATOR_ON:
                    mContext->GetText(R::string::roamingText0, (ICharSequence**)&cs);
                    cs->ToString(&str);
                    ret = new EriDisplayInformation(
                            IEriInfo::ROAMING_INDICATOR_ON,
                            IEriInfo::ROAMING_ICON_MODE_NORMAL, str);
                    break;

                case IEriInfo::ROAMING_INDICATOR_OFF:
                    mContext->GetText(R::string::roamingText1, (ICharSequence**)&cs);
                    cs->ToString(&str);
                    ret = new EriDisplayInformation(
                            IEriInfo::ROAMING_INDICATOR_OFF,
                            IEriInfo::ROAMING_ICON_MODE_NORMAL, str);
                    break;

                case IEriInfo::ROAMING_INDICATOR_FLASH:
                    mContext->GetText(R::string::roamingText2, (ICharSequence**)&cs);
                    cs->ToString(&str);
                    ret = new EriDisplayInformation(
                            IEriInfo::ROAMING_INDICATOR_FLASH,
                            IEriInfo::ROAMING_ICON_MODE_FLASH, str);
                    break;

                default:
                    ret = new EriDisplayInformation(-1, -1, String("ERI text"));
                }
            }
        }
        else {
            // ERI file loaded
            AutoPtr<EriInfo> eriInfo = (EriInfo*)GetEriInfo(roamInd).Get();
            AutoPtr<EriInfo> defEriInfo = (EriInfo*)GetEriInfo(defRoamInd).Get();

            if (eriInfo == NULL) {
                if (VDBG) {
                    Logger::V(LOGTAG, "ERI roamInd %d not found in ERI file ...using defRoamInd %d",
                            roamInd, defRoamInd);
                }
                if(defEriInfo == NULL) {
                    Logger::E(LOGTAG, "ERI defRoamInd %d not found in ERI file ...on", defRoamInd);
                    mContext->GetText(R::string::roamingText0, (ICharSequence**)&cs);
                    cs->ToString(&str);
                    ret = new EriDisplayInformation(
                            IEriInfo::ROAMING_INDICATOR_ON,
                            IEriInfo::ROAMING_ICON_MODE_NORMAL, str);

                }
                else {
                    if (VDBG) {
                        Logger::V(LOGTAG, "ERI defRoamInd %d found in ERI file", defRoamInd);
                    }
                    ret = new EriDisplayInformation(
                            defEriInfo->iconIndex,
                            defEriInfo->iconMode,
                            defEriInfo->eriText);
                }
            }
            else {
                if (VDBG) Logger::V(LOGTAG, "ERI roamInd %d found in ERI file", &roamInd);
                ret = new EriDisplayInformation(
                        eriInfo->iconIndex,
                        eriInfo->iconMode,
                        eriInfo->eriText);
            }
        }
        break;
    }

    ret->ToString(&str);
    if (VDBG) Logger::V(LOGTAG, "Displaying ERI %s", str.string());
    return ret;
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
