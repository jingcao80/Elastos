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

#ifndef __ORG_ALLJOYN_BUS_ABOUTDATA_H__
#define __ORG_ALLJOYN_BUS_ABOUTDATA_H__

#include "_Org.Alljoyn.Bus.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IMap;
using Elastos::Utility::ISet;
using Elastos::Utility::IUUID;
using Elastos::Core::Object;

namespace Org {
namespace Alljoyn {
namespace Bus {

class AboutData
    : public Object
    , public IAboutData
    , public IAboutDataListener
    , public IAboutKeys
{
protected:
    class FieldDetails : public Object
    {
    public:
        FieldDetails(
            /* [in] */ Int32 aboutFieldMask,
            /* [in] */ const String& signature)
            : mFieldMask(aboutFieldMask)
            , mSignature(signature)
        {
        }

    public:
        static const Int32 EMPTY_MASK = 0x00;
        static const Int32 REQUIRED = 0x01;
        static const Int32 ANNOUNCED = 0x02;
        static const Int32 LOCALIZED = 0x04;

        Int32 mFieldMask;
        String mSignature;
    };

public:
    CAR_INTERFACE_DECL();

    /**
     * Create an AboutData class. The default language will will not be set.
     * Use the constructor that takes a default language tag; or set the
     * language using the SetDefaultLanguage member function, CreateFromMsgArg
     * member function or the CreateFromXml member function.
     *
     * The default language should be specified before any tag that requires
     * localization. These tags are.
     *  - DeviceName
     *  - AppName
     *  - Manufacturer
     *  - Description
     * @throws BusException
     */
    CARAPI constructor();

    /**
     * constructor
     *
     * @param defaultLanguage a Locale containing a IETF language tag specified
     *                        by RFC 5646 specifying the default language for the
     *                        AboutData fields
     * @throws BusException
     */
    CARAPI constructor(
        /* [in] */ const String& defaultLanguage);

    /**
     * constructor
     *
     * All Localized tags will be set to the default language.
     *
     * @param aboutData Map containing the AboutData dictionary
     *
     * @throws BusException
     */
    CARAPI constructor(
        /* [in] */ IMap* aboutData);

    /**
     * constructor
     *
     * @param aboutData Map containing the AboutData dictionary
     * @param language a Locale containing a IETF language tag specified by RFC 5646
     *
     * @throws BusException
     */

    CARAPI constructor(
        /* [in] */ IMap* aboutData,
        /* [in] */ const String& language);

    /**
     * The AboutData has all of the required fields
     *
     * If no language is given default language will be checked
     *
     * @param language a Locale containing a IETF language tag specified by RFC 5646
     *
     * @return true if all required field are listed for the given language
     */
    CARAPI IsValid(
        /* [in] */ const String& _language,
        /* [out] */ Boolean* res);

    /**
     * The AboutData has all of the required fields.
     *
     * Only fields for the default language will be checked.
     *
     * @return true if all required field are listed for the given language
     */
    CARAPI IsValid(
        /* [out] */ Boolean* res);

    /**
     * Fill in the AboutData fields using a String/Variant Map.
     *
     * The MsgArg must contain a dictionary of type a{sv} The expected use of this
     * class is to fill in the AboutData using a Map obtain from the Announce
     * signal or the GetAboutData method from org.alljoyn.about interface.
     *
     * @param aboutData Map containing the AboutData dictionary
     * @param language a Locale containing a IETF language tag specified by RFC 5646
     *
     * @throws BusException indicating that the aboutData Map is missing a
     *                      required field. Typically this means the default
     *                      language was not specified.
     */
    CARAPI CreateFromAnnoncedAboutData(
        /* [in] */ IMap* aboutData,
        /* [in] */ const String& _language);

    /**
     * Fill in the AboutData fields using a String/Variant Map.
     *
     * All Localized tags will be set to the default language.
     *
     * The MsgArg must contain a dictionary of type a{sv} The expected use of this
     * class is to fill in the AboutData using a Map obtain from the Announce
     * signal or the GetAboutData method from org.alljoyn.about interface.
     *
     * @param aboutData Map containing the AboutData dictionary
     *
     * @throws BusException indicating that the aboutData Map is missing a
     *                      required field. Typically this means the default
     *                      language was not specified.
     */
    CARAPI CreateFromAnnouncedAboutData(
        /* [in] */ IMap* aboutData);

    /**
     * Set the AppId for the AboutData
     *
     * AppId Should be a 128-bit UUID as specified in by RFC 4122.
     *
     * Passing in non-128-bit byte arrays will still Set the AppId but the
     * setAppId member function will always throw a BusException indicating
     * the AppId is not 128-bits (16-bytes).
     *
     * AppId IS required
     * AppId IS part of the Announce signal
     * AppId CAN NOT be localized for other languages
     *
     * @param appId the a globally unique array of bytes used as an ID for the
     *              application
     *
     * @throws BusException if the AppId is not a 128-bit field
     */
    CARAPI SetAppId(
        /* [in] */ ArrayOf<Byte>* appId);

    /**
     * Get the AppId from the AboutData
     *
     * AppId IS required
     * AppId IS part of the Announce signal
     * AppId CAN NOT be localized for other languages
     *
     * @return bytes representing a unique AppId
     *
     * @throws BusException indicating failure to find the AppId
     */
    CARAPI GetAppId(
        /* [out, callee] */ ArrayOf<Byte>** appId);

    /**
     * Set the AppId for the AboutData using a hex encoded String.
     *
     * AppId IS required
     * AppId IS part of the Announce signal
     * AppId CAN NOT be localized for other languages
     *
     * @param appId Hex encoded String representing a globally unique array of bytes used
     *              as an ID for the application.
     *
     * @throws BusException indicating failure to set the AppId
     */
    CARAPI SetAppId(
        /* [in] */ const String& appId);

    CARAPI GetAppIdAsHexString(
        /* [out] */ String* appId);

    /**
     * Set the AppId for the AboutData using a UUID.
     *
     * Unlike setAppId(byte[]) this member function will only set the AppId if
     * a UUID is specified.
     *
     * AppId IS required
     * AppId IS part of the Announce signal
     * AppId CAN NOT be localized for other languages
     *
     * @param appId UUID representing a globally unique array of bytes used
     *              as an ID for the application.
     *
     * @throws BusException indicating failure to set the AppId
     */
    CARAPI SetAppId(
        /* [in] */ IUUID* appId);

    /**
     * Get the AppId UUID from the AboutData
     *
     * AppId IS required
     * AppId IS part of the Announce signal
     * AppId CAN NOT be localized for other languages
     *
     * @return the UUID for the AppId
     *
     * @throws BusException indicating failure to find the AppId
     */
    CARAPI GetAppIdAsUUID(
        /* [out] */ IUUID** appId);

    /**
     * Set the default language.
     *
     * This Locale language tag is automatically added to the SupportedLanguage
     * list. The Locale language tag should be an IETF language tag specified by
     * RFC 5646.
     *
     * DefaultLanguage is Required
     * DefaultLanguage is part of the Announce signal
     *
     * @param defaultLanguage Locale specifying the IETF language tag for the
     *                        default language
     *
     * @throws BusException indicating error when setting the default language
     */
    CARAPI SetDefaultLanguage(
        /* [in] */ const String& defaultLanguage);

    /**
     * Get the DefaultLanguage from the AboutData
     *
     * @return Locale indicating the default language.
     * @throws BusException indicating failure to get the default language
     */
    CARAPI GetDefaultLanguage(
        /* [out] */ String* language);

    /**
     * Set the DeviceName to the AboutData
     *
     * DeviceName is not required
     * DeviceName is part of the Announce signal
     * DeviceName can be localized for other languages
     *
     * @param deviceName the deviceName (UTF8 encoded string)
     * @param language Locale language tag specified by RFC 5646 if language is
     *                 NULL the DeviceName will be set for the default language.
     *
     * @throws BusException indicating failure to set the device name
     */
    CARAPI SetDeviceName(
        /* [in] */ const String& deviceName,
        /* [in] */ const String& language);

    /**
     * Set the DeviceName to the AboutData for the default language
     *
     * Default language must be set.
     *
     * DeviceName is not required
     * DeviceName is part of the Announce signal
     * DeviceName can be localized for other languages
     *
     * @param deviceName deviceName the deviceName (UTF8 encoded string)
     *
     * @throws BusException indicating failure to set the device name
     */
    CARAPI SetDeviceName(
        /* [in] */ const String& deviceName);

    /**
     * Get the DeviceName from the About data
     *
     * DeviceName is not required
     * DeviceName is part of the Announce signal
     * DeviceName can be localized for other languages
     *
     * @param language Locale for the IETF language tag specified by RFC 5646
     *                 if language is NULL the DeviceName for the default
     *                 language will be returned.
     *
     * @return the deviceName found in the AboutData
     * @throws BusException error indicating failure to obtain the device name
     */
    CARAPI GetDeviceName(
        /* [in] */ const String& language,
        /* [out] */ String* deviceName);

    CARAPI GetDeviceName(
        /* [out] */ String* deviceName);

    /**
     * Set the DeviceId from the AboutData
     *
     * DeviceId IS required
     * DeviceId IS part of the announce signal
     * DeviceId CAN NOT be localized for other languages
     *
     * @param deviceId is a string with a value generated using platform specific
     *                 means
     *
     * @throws BusException indicating failure to obtain the device id.
     */
    CARAPI SetDeviceId(
        /* [in] */ const String& deviceId);

    /**
     * Get the DeviceId from the AboutData
     *
     * DeviceId IS required
     * DeviceId IS part of the announce signal
     * DeviceId CAN NOT be localized for other languages
     *
     * @return the DeviceId string
     * @throws BusException indicating failure to find device id.
     */
    CARAPI GetDeviceId(
        /* [out] */ String* deviceId);

    /**
     * Set the AppName to the AboutData
     *
     * AppName is required
     * AppName is part of the announce signal
     * AppName can be localized for other languages
     *
     * @param appName the AppName (UTF8 encoded string)
     * @param language Locale for the IETF language tag specified by RFC 5646
     *                 if language is NULL the AppName will be set for the
     *                 default language.
     *
     * @throws BusException indicating failure to set the AppName.
     */
    CARAPI SetAppName(
        /* [in] */ const String& appName,
        /* [in] */ const String& language);

    /**
     * Set the AppName to the AboutData for the default language.
     *
     * AppName is required
     * AppName is part of the announce signal
     * AppName can be localized for other languages
     *
     * @param appName the AppName (UTF8 encoded string)
     *
     * @throws BusException indicating failure to set the AppName.
     */
    CARAPI SetAppName(
        /* [in] */ const String& appName);

    /**
     * Get the AppName from the About data
     *
     * AppName is required
     * AppName is part of the announce signal
     * AppName can be localized for other languages
     *
     * @param language Locale for the IETF language tag specified by RFC 5646
     *                 if language is NULL the AppName for the default language
     *                 will be returned.
     *
     * @return The application name
     * @throws BusException indicates failure to obtain the application name.
     */
    CARAPI GetAppName(
        /* [in] */ const String& language,
        /* [out] */ String* appName);

    /**
     * Get the AppName from the About data for the default language
     *
     * AppName is required
     * AppName is part of the announce signal
     * AppName can be localized for other languages
     *
     * @return The application name
     * @throws BusException indicates failure to obtain the application name.
     */
    CARAPI GetAppName(
        /* [out] */ String* appName);

    /**
     * Set the Manufacture for the AboutData
     *
     * Manufacture is required
     * Manufacture is part of the announce signal
     * Manufacture can be localized for other languages
     *
     * @param manufacturer the Manufacturer (UTF8 encoded string)
     * @param language Locale containing the IETF language tag specified by
     *                 RFC 5646 if language is NULL the Manufacture will be set
     *                 for the default language.
     *
     * @throws BusException unable to set the Manufacture
     */
    CARAPI SetManufacturer(
        /* [in] */ const String& manufacturer,
        /* [in] */ const String& language);

    /**
     * Set the Manufacture for the AboutData for the default language.
     *
     * Manufacture is required
     * Manufacture is part of the announce signal
     * Manufacture can be localized for other languages
     *
     * @param manufacturer the Manufacturer (UTF8 encoded string)
     *
     * @throws BusException unable to set the Manufacture
     */
    CARAPI SetManufacturer(
        /* [in] */ const String& manufacturer);

    /**
     * Get the Manufacturer from the About data.
     *
     * Manufacture is required
     * Manufacture is part of the announce signal
     * Manufacture can be localized for other languages
     *
     * @param language Locale containing the IETF language tag specified by
     *                 RFC 5646 if language is NULL the Manufacturer for the
     *                 default language will be returned.
     *
     * @return The Manufacture for the specified language
     * @throws BusException indicating failure to obtain the manufacturer name
     */
    CARAPI GetManufacturer(
        /* [in] */ const String& language,
        /* [out] */ String* manufacturer);

    /**
     * Get the Manufacturer from the About data for the default language.
     *
     * Manufacture is required
     * Manufacture is part of the announce signal
     * Manufacture can be localized for other languages
     *
     * @return The Manufacture for the default language.
     * @throws BusException indicating failure to obtain the manufacturer name
     */
    CARAPI GetManufacturer(
        /* [out] */ String* manufacturer);

    /**
     * Set the ModelNumber to the AboutData
     *
     * ModelNumber is required
     * ModelNumber is part of the announce signal
     * ModelNumber can not be localized for other languages
     *
     * @param modelNumber the application model number
     *
     * @throws BusException indicating failure to set the model number
     */
    CARAPI SetModelNumber(
        /* [in] */ const String& modelNumber);

    /**
     * Get the ModelNumber from the AboutData
     *
     * ModelNumber IS required
     * ModelNumber IS part of the announce signal
     * ModelNumber CAN NOT be localized for other languages
     *
     * @return the model number
     * @throws BusException indicating failure to obtain the model number
     */
    CARAPI GetModelNumber(
        /* [out] */ String* modelNumber);

    /**
     * Set a supported language.
     *
     * This is a Locale representing the a single language. The language is
     * specified in a Locale using IETF language tags specified by the RFC 5646.
     *
     * If the language tag has already been added no error will be thrown. The
     * method will returned with no changes being made.
     *
     * @param language Locale containing the IETF language tag
     *
     * @throws BusException indicating failure to set the language tag.
     */
    CARAPI SetSupportedLanguage(
        /* [in] */ const String& language);

    /**
     * Set supported languages.
     *
     * This is an array of Locals representing the supported languages. The
     * languages are specified in a Locals using IETF language tags specified
     * by the RFC 5646.
     *
     * If a language tag has already been added no error will be thrown. The
     * method will continue to add other languages in the array.
     *
     * @param languages An array of Locals containing the IETF language tag
     *
     * @throws BusException indicating failure to set the language tags.
     */
    CARAPI SetSupportedLanguages(
        /* [in] */ ArrayOf<String>* languages);

    /**
     * Get and array of supported languages
     *
     * @return An array of locals containing languageTags.
     * @throws BusException indicating failure obtaining language tags
     */
    CARAPI GetSupportedLanguages(
        /* [out, callee] */ ArrayOf<String>** languages);

    /**
     * Set the Description to the AboutData
     *
     * Description IS required
     * Description IS NOT part of the announce signal
     * Description CAN BE localized for other languages
     *
     * @param description the Description (UTF8 encoded string)
     * @param language Locale containing the IETF language tag specified by
     *                 RFC 5646 if language is NULL the Description will be set
     *                 for the default language.
     *
     * @throws BusException indicating failure to set the description.
     */
    CARAPI SetDescription(
        /* [in] */ const String& description,
        /* [in] */ const String& language);

    /**
     * Set the Description to the AboutData for the default language.
     *
     * Description IS required
     * Description IS NOT part of the announce signal
     * Description CAN BE localized for other languages
     *
     * @param description the Description (UTF8 encoded string)
     *
     * @throws BusException indicating failure to set the description.
     */
    CARAPI SetDescription(
        /* [in] */ const String& description);

    /**
     * Get the Description from the About data
     *
     * Description IS required
     * Description IS NOT part of the announce signal
     * Description CAN BE localized for other languages
     *
     * @param language the IETF language tag specified by RFC 5646
     *        if language is NULL the Description for the default language will be returned.
     *
     * @return The description.
     * @throws BusException indicating failure to get the description
     */
    CARAPI GetDescription(
        /* [in] */ const String& language,
        /* [out] */ String* description);

    /**
     * Get the Description from the About data for the default language.
     *
     * Description IS required
     * Description IS NOT part of the announce signal
     * Description CAN BE localized for other languages
     *
     * @return The description for the default language.
     * @throws BusException indicating failure to get the description
     */
    CARAPI GetDescription(
        /* [out] */ String* description);

    /**
     * Set the DateOfManufacture to the AboutData
     *
     * The date of manufacture using the format YYYY-MM-DD.  Known as XML
     * DateTime format.
     *
     * ModelNumber IS NOT required
     * ModelNumber IS NOT part of the announce signal
     * ModelNumber CAN NOT be localized for other languages
     *
     * @param dateOfManufacture the date of manufacture using YYYY-MM-DD format
     *
     * @throws BusException indicating failure to set the date of manufacture
     */
    CARAPI SetDateOfManufacture(
        /* [in] */ const String& dateOfManufacture);

    /**
     * Get the DatOfManufacture from the AboutData
     *
     * The date of manufacture using the format YYYY-MM-DD.  Known as XML
     * DateTime format.
     *
     * ModelNumber IS NOT required
     * ModelNumber IS NOT part of the announce signal
     * ModelNumber CAN NOT be localized for other languages
     *
     * @return the date of manufacture
     * @throws BusException indicating failure to get the date of manufacture.
     */
    CARAPI GetDateOfManufacture(
        /* [out] */ String* dateOfManufacture);

    /**
     * Set the SoftwareVersion to the AboutData
     *
     * SoftwareVersion IS required
     * SoftwareVersion IS NOT part of the announce signal
     * SoftwareVersion CAN NOT be localized for other languages
     *
     * @param softwareVersion the software version for the OEM software
     *
     * @throws BusException indicating failure to set the software version.
     */
    CARAPI SetSoftwareVersion(
        /* [in] */ const String& softwareVersion);

    /**
     * Get the SoftwareVersion from the AboutData
     *
     * SoftwareVersion IS required
     * SoftwareVersion IS NOT part of the announce signal
     * SoftwareVersion CAN NOT be localized for other languages
     *
     * @return The software version
     * @throws BusException indicating failure to get the software version.
     */
    CARAPI GetSoftwareVersion(
        /* [out] */ String* softwareVersion);

    /**
     * Get the AJSoftwareVersion from the AboutData
     *
     * The AJSoftwareVersion is automatically set when the AboutData is created
     * or when it is read from remote device.
     *
     * ModelNumber IS required
     * ModelNumber IS NOT part of the announce signal
     * ModelNumber CAN NOT be localized for other languages
     *
     * @return the AllJoyn software version
     * @throws BusException indicating failure to get the AllJoyn software version
     */
    CARAPI GetAJSoftwareVersion(
        /* [out] */ String* softwareVersion);

    /**
     * Set the HardwareVersion to the AboutData
     *
     * HardwareVersion IS NOT required
     * HardwareVersion IS NOT part of the announce signal
     * HardwareVersion CAN NOT be localized for other languages
     *
     * @param hardwareVersion the device hardware version
     *
     * @throws BusException indicating failure to set the hardware version
     */
    CARAPI SetHardwareVersion(
        /* [in] */ const String& hardwareVersion);

    /**
     * Get the HardwareVersion from the AboutData
     *
     * HardwareVersion IS NOT required
     * HardwareVersion IS NOT part of the announce signal
     * HardwareVersion CAN NOT be localized for other languages
     *
     * @return The hardware version
     * @throws BusException indicating failure to read the hardware version.
     */
    CARAPI GetHardwareVersion(
        /* [out] */ String* hardwareVersion);

    /**
     * Set the SupportUrl to the AboutData
     *
     * SupportUrl IS NOT required
     * SupportUrl IS NOT part of the announce signal
     * SupportUrl CAN NOT be localized for other languages
     *
     * @param supportUrl the support URL to be populated by OEM
     *
     * @throws BusException indicating failure to set the support URL
     */
    CARAPI SetSupportUrl(
        /* [in] */ const String& supportUrl);

    /**
     * Get the SupportUrl from the AboutData
     *
     * SupportUrl IS NOT required
     * SupportUrl IS NOT part of the announce signal
     * SupportUrl CAN NOT be localized for other languages
     *
     * @return The support URL
     * @throws BusException indicating failure to get the support URL
     */
    CARAPI GetSupportUrl(
        /* [out] */ String* supportUrl);

    /**
     * generic way to Set new field.  Every field could be set this way.
     *
     * Unless the generic field is one of the pre-defined fields when they are
     * set they will have the following specifications
     *   NOT required
     *   NOT part of the announce signal
     *   CAN be localized if it is a string NOT localized otherwise
     *
     * Since every field can be localized even if the field is not localized it
     * must be set for every language.
     *
     * @param name     the name of the field to set
     * @param value    a MsgArg that contains the value that is set for the field
     * @param language The IETF language tag specified by RFC 5646 if language
     *                 is NULL the default language will be used.  Only used for
     *                 fields that are marked as localizable.
     *
     * @throws BusException indicating failure to set the field
     */
    CARAPI SetField(
        /* [in] */ const String& name,
        /* [in] */ IVariant* value,
        /* [in] */ const String& language);

    CARAPI SetField(
        /* [in] */ const String& name,
        /* [in] */ IVariant* value);

    /**
     * Is the given field name required to make an About announcement
     *
     * @param fieldName the name of the field
     *
     * @return
     * <ul>
     *   <li> <code>true</code> if the field is required to make an About announcement</li>
     *   <li> <code>false</code> otherwise.  If the fieldName is unknown false will be returned</li>
     * </ul>
     */
    CARAPI IsFieldRequired(
        /* [in] */ const String& fieldName,
        /* [out] */ Boolean* res);

    /**
     * Is the given field part of the announce signal
     *
     * @param fieldName the name of the field
     *
     * @return
     * <ul>
     *   <li><code>true</code> if the field is part of the announce signal</li>
     *   <li><code>false</code> otherwise.  If the fieldName is unknown false will be returned</li>
     * </ul>
     */
    CARAPI IsFieldAnnounced(
        /* [in] */ const String& fieldName,
        /* [out] */ Boolean* res);

    /**
     * Is the given field a localized field.
     *
     * Localized fields should be provided for every supported language.
     *
     * @param fieldName the name of the field
     *
     * @return
     * <ul>
     *   <li><code>true</code> if the field is a localizable value</li>
     *   <li><code>false</code> otherwise.  If the fieldName is unknown false will be returned.</li>
     * </ul>
     */
    CARAPI IsFieldLocalized(
        /* [in] */ const String& fieldName,
        /* [out] */ Boolean* res);

    /**
     * @param language IETF language tags specified by RFC 5646 if the string
     *                     is NULL or an empty string the MsgArg for the default
     *                     language will be returned
     *
     * @return Map containing AboutData key/value pairs.
     * @throws ErrorReplyBusException
     * <ul>
     *   <li>ErrorReplyBusException LANGUAGE_NOT_SUPPORTED if language is not supported
     *   <li>ErrorReplyBusException ABOUT_ABOUTDATA_MISSING_REQUIRED_FIELD if a required field is missing
     * </ul>
     */
    // @Override
    CARAPI GetAboutData(
        /* [in] */ const String& language,
        /* [out] */ IMap** aboutData);

    /**
     * Return a MsgArg pointer containing dictionary containing the AboutData that
     * is announced with the org.alljoyn.About.announce signal.
     * This will always be the default language and will only contain the fields
     * that are announced.
     *
     * The fields that will be part of the announced MsgArg are:
     *  - AppId
     *  - DefaultLanguage
     *  - DeviceName
     *  - DeviceId
     *  - AppName
     *  - Manufacture
     *  - ModelNumber
     *
     * If you require other fields or need the localized AboutData
     *   The org.alljoyn.About.GetAboutData method can be used.
     * @return Map containing the announced AboutData key/value pairs.
     * @throws ErrorReplyBusException
     * <ul>
     *   <li>ErrorReplyBusException LANGUAGE_NOT_SUPPORTED if language is not supported
     *   <li>ErrorReplyBusException ABOUT_ABOUTDATA_MISSING_REQUIRED_FIELD if a required field is missing
     * </ul>
     */
    // @Override
    CARAPI GetAnnouncedAboutData(
        /* [out] */ IMap** aboutData);

public:
    /**
     * use xml definition of AboutData to set the about data.
       @code
       "<AboutData>"
       "  <AppId>000102030405060708090A0B0C0D0E0C</AppId>"
       "  <DefaultLanguage>en</DefaultLanguage>"
       "  <DeviceName>My Device Name</DeviceName>"
       "  <DeviceName lang = 'es'>Nombre de mi dispositivo</DeviceName>"
       "  <DeviceId>93c06771-c725-48c2-b1ff-6a2a59d445b8</DeviceId>"
       "  <AppName>My Application Name</AppName>"
       "  <AppName lang = 'es'>Mi Nombre de la aplicacion</AppName>"
       "  <Manufacturer>Company</Manufacturer>"
       "  <Manufacturer lang = 'sp'>Empresa</Manufacturer>"
       "  <ModelNumber>Wxfy388i</ModelNumber>"
       "  <Description>A detailed description provided by the application.</Description>"
       "  <Description lang = 'es'>Una descripcion detallada proporcionada por la aplicacion.</Description>"
       "  <DateOfManufacture>2014-01-08</DateOfManufacture>"
       "  <SoftwareVersion>1.0.0</SoftwareVersion>"
       "  <HardwareVersion>1.0.0</HardwareVersion>"
       "  <SupportUrl>www.example.com</SupportUrl>"
       "</AboutData>"
       @endcode
     *
     * The createFromXml method will attempt to process the entire xml passed
     * in. If a parsing error is encountered the last error found will be thrown
     * as a BusException.
     *
     * Note: AJSoftwareVersion is automatically set to the version of Alljoyn that
     * is being used. The SupportedLanguages tag is automatically implied from
     * the DefaultLanguage tag and the lang annotation from tags that are
     * localizable.
     *
     * @param aboutDataXml a string that contains an XML representation of
     *                     the AboutData fields.
     * @return ER_OK on success
     * @throws BusException Indicating failure to find one or more AboutData tags
     * @throws ParserConfigurationException If a DocumentBuilder cannot be created.
     *                                       This is required to parse the xml.
     * @throws IOException If any IO errors occur.
     * @throws SAXException If any parse errors occur.
     */
    CARAPI CreateFromXml(
        /* [in] */ const String& aboutDataXml);

    /**
     * Generic way to get field.
     *
     * @param name     the name of the field to get
     * @param language Locale containing the IETF language tag specified by
     *                 RFC 5646 if language is NULL the field for the default
     *                 language will be returned.
     *
     * @return return a Variant that holds the requested field value.
     * @throws BusException indicating failure to get the requested field.
     */
    CARAPI GetField(
        /* [in] */ const String& name,
        /* [in] */ const String& language,
        /* [out] */ IVariant** field);

    CARAPI GetField(
        /* [in] */ const String& name,
        /* [out] */ IVariant** field);

    /**
     * Get a Set listing the fields contained in this AboutData class.  This may be
     * required if a the AboutData comes from a remote source. User defined
     * fields are permitted. Use the getFields method to get a list of all fields
     * currently known by the AboutData.
     *
     * @return
     *  Set containing a list of all known fields in the AboutData class.
     */
    CARAPI GetFields(
        /* [out] */ ISet** fields);

    /**
     * Get the signature for the given field.
     *
     * @param fieldName the name of the field
     *
     * @return
     * <ul>
     *   <li>the signature of the field</li>
     *   <li><code>NULL</code> means field is unknown</li>
     * </ul>
     */
    CARAPI_(String) GetFieldSignature(
        /* [in] */ const String& fieldName);

protected:
    CARAPI_(void) SetNewFieldDetails(
        /* [in] */ const String& fieldName,
        /* [in] */ Int32 aboutFieldMask,
        /* [in] */ const String& signature);

private:
    static CARAPI HexStringToByteArray(
        /* [in] */ const String& hexString,
        /* [out] */ ArrayOf<Byte>** array);

    static CARAPI_(String) ByteArrayToHexString(
        /* [in] */ ArrayOf<Byte>* byteArray);

    CARAPI_(void) InitializeFieldDetails();

protected:
    static AutoPtr<ArrayOf<Char32> > sHexCharArray;

private:
    AutoPtr<ISet> mSupportedLanguages; // Set<String>
    AutoPtr<IMap> mPropertyStore; // Map<String, Variant>
    AutoPtr<IMap> mLocalizedPropertyStore; // Map<String, Map<String, Variant>>
    AutoPtr<IMap> mAboutFields; // Map<String, FieldDetails>
    String mErrorMessage;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_ABOUTDATA_H__
