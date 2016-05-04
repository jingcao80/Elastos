/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.internal.telephony.cdma;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Utility::IXml;

using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Internal::Utility::IXmlUtils;


using Org::Xmlpull::V1::IXmlPullParser;

using Elastos::IO::IFileInputStream;
using Elastos::Utility::IHashMap;

/**
 * EriManager loads the ERI file definitions and manages the CDMA roaming information.
 *
 */
public class EriManager {

    class EriFile {

        Int32 mVersionNumber;                      // File version number
        Int32 mNumberOfEriEntries;                 // Number of entries
        Int32 mEriFileType;                        // Eri Phase 0/1
        //Int32 mNumberOfIconImages;               // reserved for future use
        //Int32 mIconImageType;                    // reserved for future use
        String[] mCallPromptId;                  // reserved for future use
        HashMap<Integer, EriInfo> mRoamIndTable; // Roaming Indicator Table

        EriFile() {
            mVersionNumber = -1;
            mNumberOfEriEntries = 0;
            mEriFileType = -1;
            mCallPromptId = new String[] { "", "", "" };
            mRoamIndTable = new HashMap<Integer, EriInfo>();
        }
    }

    class EriDisplayInformation {
        Int32 mEriIconIndex;
        Int32 mEriIconMode;
        String mEriIconText;

        EriDisplayInformation(Int32 eriIconIndex, Int32 eriIconMode, String eriIconText) {
            mEriIconIndex = eriIconIndex;
            mEriIconMode = eriIconMode;
            mEriIconText = eriIconText;
        }

//        CARAPI SetParameters(Int32 eriIconIndex, Int32 eriIconMode, String eriIconText){
//            mEriIconIndex = eriIconIndex;
//            mEriIconMode = eriIconMode;
//            mEriIconText = eriIconText;
//        }

        //@Override
        CARAPI ToString(
        /* [out] */ String* str)
    {
            return "EriDisplayInformation: {" + " IconIndex: " + mEriIconIndex + " EriIconMode: "
                    + mEriIconMode + " EriIconText: " + mEriIconText + " }";
        }
    }

    private static const String LOG_TAG = "CDMA";
    private static const Boolean DBG = TRUE;
    private static const Boolean VDBG = FALSE;

    static const Int32 ERI_FROM_XML          = 0;
    static const Int32 ERI_FROM_FILE_SYSTEM  = 1;
    static const Int32 ERI_FROM_MODEM        = 2;

    private Context mContext;
    private Int32 mEriFileSource = ERI_FROM_XML;
    private Boolean mIsEriFileLoaded;
    private EriFile mEriFile;

    public EriManager(PhoneBase phone, Context context, Int32 eriFileSource) {
        mContext = context;
        mEriFileSource = eriFileSource;
        mEriFile = new EriFile();
    }

    CARAPI Dispose() {
        mEriFile = new EriFile();
        mIsEriFileLoaded = FALSE;
    }


    CARAPI LoadEriFile() {
        Switch (mEriFileSource) {
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
    }

    /**
     * Load the ERI file from the MODEM through chipset specific RIL_REQUEST_OEM_HOOK
     *
     * In this case the ERI file can be updated from the Phone Support Tool available
     * from the Chipset vendor
     */
    private void LoadEriFileFromModem() {
        // NOT IMPLEMENTED, Chipset vendor/Operator specific
    }

    /**
     * Load the ERI file from a File System file
     *
     * In this case the a Phone Support Tool to update the ERI file must be provided
     * to the Operator
     */
    private void LoadEriFileFromFileSystem() {
        // NOT IMPLEMENTED, Chipset vendor/Operator specific
    }

    /**
     * Load the ERI file from the application framework resources encoded in XML
     *
     */
    private void LoadEriFileFromXml() {
        XmlPullParser parser = NULL;
        FileInputStream stream = NULL;
        Resources r = mContext->GetResources();

        try {
            If (DBG) Rlog->D(LOG_TAG, "loadEriFileFromXml: check for alternate file");
            stream = new FileInputStream(
                            r->GetString(R::string::alternate_eri_file));
            parser = Xml->NewPullParser();
            parser->SetInput(stream, NULL);
            If (DBG) Rlog->D(LOG_TAG, "loadEriFileFromXml: opened alternate file");
        } Catch (FileNotFoundException e) {
            If (DBG) Rlog->D(LOG_TAG, "loadEriFileFromXml: no alternate file");
            parser = NULL;
        } Catch (XmlPullParserException e) {
            If (DBG) Rlog->D(LOG_TAG, "loadEriFileFromXml: no parser for alternate file");
            parser = NULL;
        }

        If (parser == NULL) {
            If (DBG) Rlog->D(LOG_TAG, "loadEriFileFromXml: open normal file");
            parser = r->GetXml(R.xml.eri);
        }

        try {
            XmlUtils->BeginDocument(parser, "EriFile");
            mEriFile.mVersionNumber = Integer->ParseInt(
                    parser->GetAttributeValue(NULL, "VersionNumber"));
            mEriFile.mNumberOfEriEntries = Integer->ParseInt(
                    parser->GetAttributeValue(NULL, "NumberOfEriEntries"));
            mEriFile.mEriFileType = Integer->ParseInt(
                    parser->GetAttributeValue(NULL, "EriFileType"));

            Int32 parsedEriEntries = 0;
            While(TRUE) {
                XmlUtils->NextElement(parser);
                String name = parser->GetName();
                If (name == NULL) {
                    If (parsedEriEntries != mEriFile.mNumberOfEriEntries)
                        Rlog->E(LOG_TAG, "Error Parsing ERI file: " +  mEriFile.mNumberOfEriEntries
                                + " defined, " + parsedEriEntries + " parsed!");
                    break;
                } else If (name->Equals("CallPromptId")) {
                    Int32 id = Integer->ParseInt(parser->GetAttributeValue(NULL, "Id"));
                    String text = parser->GetAttributeValue(NULL, "CallPromptText");
                    If (id >= 0 && id <= 2) {
                        mEriFile.mCallPromptId[id] = text;
                    } else {
                        Rlog->E(LOG_TAG, "Error Parsing ERI file: found" + id + " CallPromptId");
                    }

                } else If (name->Equals("EriInfo")) {
                    Int32 roamingIndicator = Integer->ParseInt(
                            parser->GetAttributeValue(NULL, "RoamingIndicator"));
                    Int32 iconIndex = Integer->ParseInt(parser->GetAttributeValue(NULL, "IconIndex"));
                    Int32 iconMode = Integer->ParseInt(parser->GetAttributeValue(NULL, "IconMode"));
                    String eriText = parser->GetAttributeValue(NULL, "EriText");
                    Int32 callPromptId = Integer->ParseInt(
                            parser->GetAttributeValue(NULL, "CallPromptId"));
                    Int32 alertId = Integer->ParseInt(parser->GetAttributeValue(NULL, "AlertId"));
                    parsedEriEntries++;
                    mEriFile.mRoamIndTable->Put(roamingIndicator, new EriInfo (roamingIndicator,
                            iconIndex, iconMode, eriText, callPromptId, alertId));
                }
            }

            If (DBG) Rlog->D(LOG_TAG, "loadEriFileFromXml: eri parsing successful, file loaded");
            mIsEriFileLoaded = TRUE;

        } Catch (Exception e) {
            Rlog->E(LOG_TAG, "Got exception while loading ERI file.", e);
        } finally {
            If (parser instanceof XmlResourceParser) {
                ((XmlResourceParser)parser).Close();
            }
            try {
                If (stream != NULL) {
                    stream->Close();
                }
            } Catch (IOException e) {
                // Ignore
            }
        }
    }

    /**
     * Returns the version of the ERI file
     *
     */
    public Int32 GetEriFileVersion() {
        return mEriFile.mVersionNumber;
    }

    /**
     * Returns the number of ERI entries parsed
     *
     */
    public Int32 GetEriNumberOfEntries() {
        return mEriFile.mNumberOfEriEntries;
    }

    /**
     * Returns the ERI file type Value ( 0 for Phase 0, 1 for Phase 1)
     *
     */
    public Int32 GetEriFileType() {
        return mEriFile.mEriFileType;
    }

    /**
     * Returns if the ERI file has been loaded
     *
     */
    public Boolean IsEriFileLoaded() {
        return mIsEriFileLoaded;
    }

    /**
     * Returns the EriInfo record associated with roamingIndicator
     * or NULL if the entry is not found
     */
    private EriInfo GetEriInfo(Int32 roamingIndicator) {
        If (mEriFile.mRoamIndTable->ContainsKey(roamingIndicator)) {
            return mEriFile.mRoamIndTable->Get(roamingIndicator);
        } else {
            return NULL;
        }
    }

    private EriDisplayInformation GetEriDisplayInformation(Int32 roamInd, Int32 defRoamInd){
        EriDisplayInformation ret;

        // Carrier can use eri.xml to customize any built-in roaming display indications
        If (mIsEriFileLoaded) {
            EriInfo eriInfo = GetEriInfo(roamInd);
            If (eriInfo != NULL) {
                If (VDBG) Rlog->V(LOG_TAG, "ERI roamInd " + roamInd + " found in ERI file");
                ret = new EriDisplayInformation(
                        eriInfo.iconIndex,
                        eriInfo.iconMode,
                        eriInfo.eriText);
                return ret;
            }
        }

        Switch (roamInd) {
        // Handling the standard roaming Indicator (non-ERI)
        case EriInfo.ROAMING_INDICATOR_ON:
            ret = new EriDisplayInformation(
                    EriInfo.ROAMING_INDICATOR_ON,
                    EriInfo.ROAMING_ICON_MODE_NORMAL,
                    mContext->GetText(R::string::roamingText0).ToString());
            break;

        case EriInfo.ROAMING_INDICATOR_OFF:
            ret = new EriDisplayInformation(
                    EriInfo.ROAMING_INDICATOR_OFF,
                    EriInfo.ROAMING_ICON_MODE_NORMAL,
                    mContext->GetText(R::string::roamingText1).ToString());
            break;

        case EriInfo.ROAMING_INDICATOR_FLASH:
            ret = new EriDisplayInformation(
                    EriInfo.ROAMING_INDICATOR_FLASH,
                    EriInfo.ROAMING_ICON_MODE_FLASH,
                    mContext->GetText(R::string::roamingText2).ToString());
            break;


        // Handling the standard ERI
        case 3:
            ret = new EriDisplayInformation(
                    roamInd,
                    EriInfo.ROAMING_ICON_MODE_NORMAL,
                    mContext->GetText(R::string::roamingText3).ToString());
            break;

        case 4:
            ret = new EriDisplayInformation(
                    roamInd,
                    EriInfo.ROAMING_ICON_MODE_NORMAL,
                    mContext->GetText(R::string::roamingText4).ToString());
            break;

        case 5:
            ret = new EriDisplayInformation(
                    roamInd,
                    EriInfo.ROAMING_ICON_MODE_NORMAL,
                    mContext->GetText(R::string::roamingText5).ToString());
            break;

        case 6:
            ret = new EriDisplayInformation(
                    roamInd,
                    EriInfo.ROAMING_ICON_MODE_NORMAL,
                    mContext->GetText(R::string::roamingText6).ToString());
            break;

        case 7:
            ret = new EriDisplayInformation(
                    roamInd,
                    EriInfo.ROAMING_ICON_MODE_NORMAL,
                    mContext->GetText(R::string::roamingText7).ToString());
            break;

        case 8:
            ret = new EriDisplayInformation(
                    roamInd,
                    EriInfo.ROAMING_ICON_MODE_NORMAL,
                    mContext->GetText(R::string::roamingText8).ToString());
            break;

        case 9:
            ret = new EriDisplayInformation(
                    roamInd,
                    EriInfo.ROAMING_ICON_MODE_NORMAL,
                    mContext->GetText(R::string::roamingText9).ToString());
            break;

        case 10:
            ret = new EriDisplayInformation(
                    roamInd,
                    EriInfo.ROAMING_ICON_MODE_NORMAL,
                    mContext->GetText(R::string::roamingText10).ToString());
            break;

        case 11:
            ret = new EriDisplayInformation(
                    roamInd,
                    EriInfo.ROAMING_ICON_MODE_NORMAL,
                    mContext->GetText(R::string::roamingText11).ToString());
            break;

        case 12:
            ret = new EriDisplayInformation(
                    roamInd,
                    EriInfo.ROAMING_ICON_MODE_NORMAL,
                    mContext->GetText(R::string::roamingText12).ToString());
            break;

        // Handling the non standard Enhanced Roaming Indicator (roamInd > 63)
        default:
            If (!mIsEriFileLoaded) {
                // ERI file NOT loaded
                If (DBG) Rlog->D(LOG_TAG, "ERI File not loaded");
                If(defRoamInd > 2) {
                    If (VDBG) Rlog->V(LOG_TAG, "ERI defRoamInd > 2 ...flashing");
                    ret = new EriDisplayInformation(
                            EriInfo.ROAMING_INDICATOR_FLASH,
                            EriInfo.ROAMING_ICON_MODE_FLASH,
                            mContext->GetText(com.android.internal
                                                            .R::string::roamingText2).ToString());
                } else {
                    If (VDBG) Rlog->V(LOG_TAG, "ERI defRoamInd <= 2");
                    Switch (defRoamInd) {
                    case EriInfo.ROAMING_INDICATOR_ON:
                        ret = new EriDisplayInformation(
                                EriInfo.ROAMING_INDICATOR_ON,
                                EriInfo.ROAMING_ICON_MODE_NORMAL,
                                mContext->GetText(com.android.internal
                                                            .R::string::roamingText0).ToString());
                        break;

                    case EriInfo.ROAMING_INDICATOR_OFF:
                        ret = new EriDisplayInformation(
                                EriInfo.ROAMING_INDICATOR_OFF,
                                EriInfo.ROAMING_ICON_MODE_NORMAL,
                                mContext->GetText(com.android.internal
                                                            .R::string::roamingText1).ToString());
                        break;

                    case EriInfo.ROAMING_INDICATOR_FLASH:
                        ret = new EriDisplayInformation(
                                EriInfo.ROAMING_INDICATOR_FLASH,
                                EriInfo.ROAMING_ICON_MODE_FLASH,
                                mContext->GetText(com.android.internal
                                                            .R::string::roamingText2).ToString());
                        break;

                    default:
                        ret = new EriDisplayInformation(-1, -1, "ERI text");
                    }
                }
            } else {
                // ERI file loaded
                EriInfo eriInfo = GetEriInfo(roamInd);
                EriInfo defEriInfo = GetEriInfo(defRoamInd);
                If (eriInfo == NULL) {
                    If (VDBG) {
                        Rlog->V(LOG_TAG, "ERI roamInd " + roamInd
                            + " not found in ERI file ...using defRoamInd " + defRoamInd);
                    }
                    If(defEriInfo == NULL) {
                        Rlog->E(LOG_TAG, "ERI defRoamInd " + defRoamInd
                                + " not found in ERI file ...on");
                        ret = new EriDisplayInformation(
                                EriInfo.ROAMING_INDICATOR_ON,
                                EriInfo.ROAMING_ICON_MODE_NORMAL,
                                mContext->GetText(com.android.internal
                                                             .R::string::roamingText0).ToString());

                    } else {
                        If (VDBG) {
                            Rlog->V(LOG_TAG, "ERI defRoamInd " + defRoamInd + " found in ERI file");
                        }
                        ret = new EriDisplayInformation(
                                defEriInfo.iconIndex,
                                defEriInfo.iconMode,
                                defEriInfo.eriText);
                    }
                } else {
                    If (VDBG) Rlog->V(LOG_TAG, "ERI roamInd " + roamInd + " found in ERI file");
                    ret = new EriDisplayInformation(
                            eriInfo.iconIndex,
                            eriInfo.iconMode,
                            eriInfo.eriText);
                }
            }
            break;
        }
        If (VDBG) Rlog->V(LOG_TAG, "Displaying ERI " + ret->ToString());
        return ret;
    }

    public Int32 GetCdmaEriIconIndex(Int32 roamInd, Int32 defRoamInd){
        return GetEriDisplayInformation(roamInd, defRoamInd).mEriIconIndex;
    }

    public Int32 GetCdmaEriIconMode(Int32 roamInd, Int32 defRoamInd){
        return GetEriDisplayInformation(roamInd, defRoamInd).mEriIconMode;
    }

    public String GetCdmaEriText(Int32 roamInd, Int32 defRoamInd){
        return GetEriDisplayInformation(roamInd, defRoamInd).mEriIconText;
    }
}
