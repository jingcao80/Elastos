#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.CoreLibrary.Utility.h"

#include "elastos/droid/internal/telephony/cdma/EriManager.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

//=====================================================================
//                         EriManager::EriFile
//=====================================================================
EriManager::EriFile::EriFile()
{
    // ==================before translated======================
    // mVersionNumber = -1;
    // mNumberOfEriEntries = 0;
    // mEriFileType = -1;
    // mCallPromptId = new String[] { "", "", "" };
    // mRoamIndTable = new HashMap<Integer, EriInfo>();
}

//=====================================================================
//                  EriManager::EriDisplayInformation
//=====================================================================
EriManager::EriDisplayInformation::EriDisplayInformation(
    /* [in] */ Int32 eriIconIndex,
    /* [in] */ Int32 eriIconMode,
    /* [in] */ const String& eriIconText)
{
    // ==================before translated======================
    // mEriIconIndex = eriIconIndex;
    // mEriIconMode = eriIconMode;
    // mEriIconText = eriIconText;
}

ECode EriManager::EriDisplayInformation::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return "EriDisplayInformation: {" + " IconIndex: " + mEriIconIndex + " EriIconMode: "
    //         + mEriIconMode + " EriIconText: " + mEriIconText + " }";
    assert(0);
    return NOERROR;
}

//=====================================================================
//                              EriManager
//=====================================================================
const Int32 EriManager::ERI_FROM_XML;
const Int32 EriManager::ERI_FROM_FILE_SYSTEM;
const Int32 EriManager::ERI_FROM_MODEM;
const String EriManager::LOGTAG("CDMA");
const Boolean EriManager::DBG = TRUE;
const Boolean EriManager::VDBG = FALSE;

EriManager::EriManager(
    /* [in] */ IPhoneBase* phone,
    /* [in] */ IContext* context,
    /* [in] */ Int32 eriFileSource)
{
    // ==================before translated======================
    // mContext = context;
    // mEriFileSource = eriFileSource;
    // mEriFile = new EriFile();
}

ECode EriManager::Dispose()
{
    // ==================before translated======================
    // mEriFile = new EriFile();
    // mIsEriFileLoaded = false;
    assert(0);
    return NOERROR;
}

ECode EriManager::LoadEriFile()
{
    // ==================before translated======================
    // switch (mEriFileSource) {
    // case ERI_FROM_MODEM:
    //     loadEriFileFromModem();
    //     break;
    //
    // case ERI_FROM_FILE_SYSTEM:
    //     loadEriFileFromFileSystem();
    //     break;
    //
    // case ERI_FROM_XML:
    // default:
    //     loadEriFileFromXml();
    //     break;
    // }
    assert(0);
    return NOERROR;
}

ECode EriManager::GetEriFileVersion(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mEriFile.mVersionNumber;
    assert(0);
    return NOERROR;
}

ECode EriManager::GetEriNumberOfEntries(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mEriFile.mNumberOfEriEntries;
    assert(0);
    return NOERROR;
}

ECode EriManager::GetEriFileType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mEriFile.mEriFileType;
    assert(0);
    return NOERROR;
}

ECode EriManager::IsEriFileLoaded(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mIsEriFileLoaded;
    assert(0);
    return NOERROR;
}

ECode EriManager::GetCdmaEriIconIndex(
    /* [in] */ Int32 roamInd,
    /* [in] */ Int32 defRoamInd,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return getEriDisplayInformation(roamInd, defRoamInd).mEriIconIndex;
    assert(0);
    return NOERROR;
}

ECode EriManager::GetCdmaEriIconMode(
    /* [in] */ Int32 roamInd,
    /* [in] */ Int32 defRoamInd,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return getEriDisplayInformation(roamInd, defRoamInd).mEriIconMode;
    assert(0);
    return NOERROR;
}

ECode EriManager::GetCdmaEriText(
    /* [in] */ Int32 roamInd,
    /* [in] */ Int32 defRoamInd,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return getEriDisplayInformation(roamInd, defRoamInd).mEriIconText;
    assert(0);
    return NOERROR;
}

void EriManager::LoadEriFileFromModem()
{
    // ==================before translated======================
    // // NOT IMPLEMENTED, Chipset vendor/Operator specific
    assert(0);
}

void EriManager::LoadEriFileFromFileSystem()
{
    // ==================before translated======================
    // // NOT IMPLEMENTED, Chipset vendor/Operator specific
    assert(0);
}

void EriManager::LoadEriFileFromXml()
{
    // ==================before translated======================
    // XmlPullParser parser = null;
    // FileInputStream stream = null;
    // Resources r = mContext.getResources();
    //
    // try {
    //     if (DBG) Rlog.d(LOGTAG, "loadEriFileFromXml: check for alternate file");
    //     stream = new FileInputStream(
    //                     r.getString(com.android.internal.R.string.alternate_eri_file));
    //     parser = Xml.newPullParser();
    //     parser.setInput(stream, null);
    //     if (DBG) Rlog.d(LOGTAG, "loadEriFileFromXml: opened alternate file");
    // } catch (FileNotFoundException e) {
    //     if (DBG) Rlog.d(LOGTAG, "loadEriFileFromXml: no alternate file");
    //     parser = null;
    // } catch (XmlPullParserException e) {
    //     if (DBG) Rlog.d(LOGTAG, "loadEriFileFromXml: no parser for alternate file");
    //     parser = null;
    // }
    //
    // if (parser == null) {
    //     if (DBG) Rlog.d(LOGTAG, "loadEriFileFromXml: open normal file");
    //     parser = r.getXml(com.android.internal.R.xml.eri);
    // }
    //
    // try {
    //     XmlUtils.beginDocument(parser, "EriFile");
    //     mEriFile.mVersionNumber = Integer.parseInt(
    //             parser.getAttributeValue(null, "VersionNumber"));
    //     mEriFile.mNumberOfEriEntries = Integer.parseInt(
    //             parser.getAttributeValue(null, "NumberOfEriEntries"));
    //     mEriFile.mEriFileType = Integer.parseInt(
    //             parser.getAttributeValue(null, "EriFileType"));
    //
    //     int parsedEriEntries = 0;
    //     while(true) {
    //         XmlUtils.nextElement(parser);
    //         String name = parser.getName();
    //         if (name == null) {
    //             if (parsedEriEntries != mEriFile.mNumberOfEriEntries)
    //                 Rlog.e(LOGTAG, "Error Parsing ERI file: " +  mEriFile.mNumberOfEriEntries
    //                         + " defined, " + parsedEriEntries + " parsed!");
    //             break;
    //         } else if (name.equals("CallPromptId")) {
    //             int id = Integer.parseInt(parser.getAttributeValue(null, "Id"));
    //             String text = parser.getAttributeValue(null, "CallPromptText");
    //             if (id >= 0 && id <= 2) {
    //                 mEriFile.mCallPromptId[id] = text;
    //             } else {
    //                 Rlog.e(LOGTAG, "Error Parsing ERI file: found" + id + " CallPromptId");
    //             }
    //
    //         } else if (name.equals("EriInfo")) {
    //             int roamingIndicator = Integer.parseInt(
    //                     parser.getAttributeValue(null, "RoamingIndicator"));
    //             int iconIndex = Integer.parseInt(parser.getAttributeValue(null, "IconIndex"));
    //             int iconMode = Integer.parseInt(parser.getAttributeValue(null, "IconMode"));
    //             String eriText = parser.getAttributeValue(null, "EriText");
    //             int callPromptId = Integer.parseInt(
    //                     parser.getAttributeValue(null, "CallPromptId"));
    //             int alertId = Integer.parseInt(parser.getAttributeValue(null, "AlertId"));
    //             parsedEriEntries++;
    //             mEriFile.mRoamIndTable.put(roamingIndicator, new EriInfo (roamingIndicator,
    //                     iconIndex, iconMode, eriText, callPromptId, alertId));
    //         }
    //     }
    //
    //     if (DBG) Rlog.d(LOGTAG, "loadEriFileFromXml: eri parsing successful, file loaded");
    //     mIsEriFileLoaded = true;
    //
    // } catch (Exception e) {
    //     Rlog.e(LOGTAG, "Got exception while loading ERI file.", e);
    // } finally {
    //     if (parser instanceof XmlResourceParser) {
    //         ((XmlResourceParser)parser).close();
    //     }
    //     try {
    //         if (stream != null) {
    //             stream.close();
    //         }
    //     } catch (IOException e) {
    //         // Ignore
    //     }
    // }
    assert(0);
}

AutoPtr<EriInfo> EriManager::GetEriInfo(
    /* [in] */ Int32 roamingIndicator)
{
    // ==================before translated======================
    // if (mEriFile.mRoamIndTable.containsKey(roamingIndicator)) {
    //     return mEriFile.mRoamIndTable.get(roamingIndicator);
    // } else {
    //     return null;
    // }
    assert(0);
    AutoPtr<EriInfo> empty;
    return empty;
}

AutoPtr<EriManager::EriDisplayInformation> EriManager::GetEriDisplayInformation(
    /* [in] */ Int32 roamInd,
    /* [in] */ Int32 defRoamInd)
{
    // ==================before translated======================
    // EriDisplayInformation ret;
    //
    // // Carrier can use eri.xml to customize any built-in roaming display indications
    // if (mIsEriFileLoaded) {
    //     EriInfo eriInfo = getEriInfo(roamInd);
    //     if (eriInfo != null) {
    //         if (VDBG) Rlog.v(LOGTAG, "ERI roamInd " + roamInd + " found in ERI file");
    //         ret = new EriDisplayInformation(
    //                 eriInfo.iconIndex,
    //                 eriInfo.iconMode,
    //                 eriInfo.eriText);
    //         return ret;
    //     }
    // }
    //
    // switch (roamInd) {
    // // Handling the standard roaming indicator (non-ERI)
    // case EriInfo.ROAMING_INDICATOR_ON:
    //     ret = new EriDisplayInformation(
    //             EriInfo.ROAMING_INDICATOR_ON,
    //             EriInfo.ROAMING_ICON_MODE_NORMAL,
    //             mContext.getText(com.android.internal.R.string.roamingText0).toString());
    //     break;
    //
    // case EriInfo.ROAMING_INDICATOR_OFF:
    //     ret = new EriDisplayInformation(
    //             EriInfo.ROAMING_INDICATOR_OFF,
    //             EriInfo.ROAMING_ICON_MODE_NORMAL,
    //             mContext.getText(com.android.internal.R.string.roamingText1).toString());
    //     break;
    //
    // case EriInfo.ROAMING_INDICATOR_FLASH:
    //     ret = new EriDisplayInformation(
    //             EriInfo.ROAMING_INDICATOR_FLASH,
    //             EriInfo.ROAMING_ICON_MODE_FLASH,
    //             mContext.getText(com.android.internal.R.string.roamingText2).toString());
    //     break;
    //
    //
    // // Handling the standard ERI
    // case 3:
    //     ret = new EriDisplayInformation(
    //             roamInd,
    //             EriInfo.ROAMING_ICON_MODE_NORMAL,
    //             mContext.getText(com.android.internal.R.string.roamingText3).toString());
    //     break;
    //
    // case 4:
    //     ret = new EriDisplayInformation(
    //             roamInd,
    //             EriInfo.ROAMING_ICON_MODE_NORMAL,
    //             mContext.getText(com.android.internal.R.string.roamingText4).toString());
    //     break;
    //
    // case 5:
    //     ret = new EriDisplayInformation(
    //             roamInd,
    //             EriInfo.ROAMING_ICON_MODE_NORMAL,
    //             mContext.getText(com.android.internal.R.string.roamingText5).toString());
    //     break;
    //
    // case 6:
    //     ret = new EriDisplayInformation(
    //             roamInd,
    //             EriInfo.ROAMING_ICON_MODE_NORMAL,
    //             mContext.getText(com.android.internal.R.string.roamingText6).toString());
    //     break;
    //
    // case 7:
    //     ret = new EriDisplayInformation(
    //             roamInd,
    //             EriInfo.ROAMING_ICON_MODE_NORMAL,
    //             mContext.getText(com.android.internal.R.string.roamingText7).toString());
    //     break;
    //
    // case 8:
    //     ret = new EriDisplayInformation(
    //             roamInd,
    //             EriInfo.ROAMING_ICON_MODE_NORMAL,
    //             mContext.getText(com.android.internal.R.string.roamingText8).toString());
    //     break;
    //
    // case 9:
    //     ret = new EriDisplayInformation(
    //             roamInd,
    //             EriInfo.ROAMING_ICON_MODE_NORMAL,
    //             mContext.getText(com.android.internal.R.string.roamingText9).toString());
    //     break;
    //
    // case 10:
    //     ret = new EriDisplayInformation(
    //             roamInd,
    //             EriInfo.ROAMING_ICON_MODE_NORMAL,
    //             mContext.getText(com.android.internal.R.string.roamingText10).toString());
    //     break;
    //
    // case 11:
    //     ret = new EriDisplayInformation(
    //             roamInd,
    //             EriInfo.ROAMING_ICON_MODE_NORMAL,
    //             mContext.getText(com.android.internal.R.string.roamingText11).toString());
    //     break;
    //
    // case 12:
    //     ret = new EriDisplayInformation(
    //             roamInd,
    //             EriInfo.ROAMING_ICON_MODE_NORMAL,
    //             mContext.getText(com.android.internal.R.string.roamingText12).toString());
    //     break;
    //
    // // Handling the non standard Enhanced Roaming Indicator (roamInd > 63)
    // default:
    //     if (!mIsEriFileLoaded) {
    //         // ERI file NOT loaded
    //         if (DBG) Rlog.d(LOGTAG, "ERI File not loaded");
    //         if(defRoamInd > 2) {
    //             if (VDBG) Rlog.v(LOGTAG, "ERI defRoamInd > 2 ...flashing");
    //             ret = new EriDisplayInformation(
    //                     EriInfo.ROAMING_INDICATOR_FLASH,
    //                     EriInfo.ROAMING_ICON_MODE_FLASH,
    //                     mContext.getText(com.android.internal
    //                                                     .R.string.roamingText2).toString());
    //         } else {
    //             if (VDBG) Rlog.v(LOGTAG, "ERI defRoamInd <= 2");
    //             switch (defRoamInd) {
    //             case EriInfo.ROAMING_INDICATOR_ON:
    //                 ret = new EriDisplayInformation(
    //                         EriInfo.ROAMING_INDICATOR_ON,
    //                         EriInfo.ROAMING_ICON_MODE_NORMAL,
    //                         mContext.getText(com.android.internal
    //                                                     .R.string.roamingText0).toString());
    //                 break;
    //
    //             case EriInfo.ROAMING_INDICATOR_OFF:
    //                 ret = new EriDisplayInformation(
    //                         EriInfo.ROAMING_INDICATOR_OFF,
    //                         EriInfo.ROAMING_ICON_MODE_NORMAL,
    //                         mContext.getText(com.android.internal
    //                                                     .R.string.roamingText1).toString());
    //                 break;
    //
    //             case EriInfo.ROAMING_INDICATOR_FLASH:
    //                 ret = new EriDisplayInformation(
    //                         EriInfo.ROAMING_INDICATOR_FLASH,
    //                         EriInfo.ROAMING_ICON_MODE_FLASH,
    //                         mContext.getText(com.android.internal
    //                                                     .R.string.roamingText2).toString());
    //                 break;
    //
    //             default:
    //                 ret = new EriDisplayInformation(-1, -1, "ERI text");
    //             }
    //         }
    //     } else {
    //         // ERI file loaded
    //         EriInfo eriInfo = getEriInfo(roamInd);
    //         EriInfo defEriInfo = getEriInfo(defRoamInd);
    //         if (eriInfo == null) {
    //             if (VDBG) {
    //                 Rlog.v(LOGTAG, "ERI roamInd " + roamInd
    //                     + " not found in ERI file ...using defRoamInd " + defRoamInd);
    //             }
    //             if(defEriInfo == null) {
    //                 Rlog.e(LOGTAG, "ERI defRoamInd " + defRoamInd
    //                         + " not found in ERI file ...on");
    //                 ret = new EriDisplayInformation(
    //                         EriInfo.ROAMING_INDICATOR_ON,
    //                         EriInfo.ROAMING_ICON_MODE_NORMAL,
    //                         mContext.getText(com.android.internal
    //                                                      .R.string.roamingText0).toString());
    //
    //             } else {
    //                 if (VDBG) {
    //                     Rlog.v(LOGTAG, "ERI defRoamInd " + defRoamInd + " found in ERI file");
    //                 }
    //                 ret = new EriDisplayInformation(
    //                         defEriInfo.iconIndex,
    //                         defEriInfo.iconMode,
    //                         defEriInfo.eriText);
    //             }
    //         } else {
    //             if (VDBG) Rlog.v(LOGTAG, "ERI roamInd " + roamInd + " found in ERI file");
    //             ret = new EriDisplayInformation(
    //                     eriInfo.iconIndex,
    //                     eriInfo.iconMode,
    //                     eriInfo.eriText);
    //         }
    //     }
    //     break;
    // }
    // if (VDBG) Rlog.v(LOGTAG, "Displaying ERI " + ret.toString());
    // return ret;
    assert(0);
    AutoPtr<EriDisplayInformation> empty;
    return empty;
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
