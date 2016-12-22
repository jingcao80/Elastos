
#include "org/alljoyn/bus/AboutData.h"
#include "org/alljoyn/bus/MsgArg.h"
#include "org/alljoyn/bus/CVariant.h"
#include "org/alljoyn/bus/Version.h"
#include <elastos/core/Character.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Extensions.h"
#include "Elastos.CoreLibrary.External.h"

using Elastos::IO::IReader;
using Elastos::IO::CStringReader;
using Elastos::IO::CByteBufferHelper;
using Elastos::IO::IByteBuffer;
using Elastos::IO::IByteBufferHelper;
using Elastos::Core::Character;
using Elastos::Core::CoreUtils;
using Elastos::Core::EIID_ICharSequence;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CTreeMap;
using Elastos::Utility::CTreeSet;
using Elastos::Utility::CUUID;
using Elastos::Utility::CUUIDHelper;
using Elastos::Utility::IUUIDHelper;
using Elastos::Utility::Logging::Logger;

using Elastosx::Xml::Parsers::CDocumentBuilderFactoryHelper;
using Elastosx::Xml::Parsers::IDocumentBuilderFactoryHelper;
using Elastosx::Xml::Parsers::IDocumentBuilder;
using Elastosx::Xml::Parsers::IDocumentBuilderFactory;

using Org::W3c::Dom::IDocument;
using Org::W3c::Dom::INamedNodeMap;
using Org::W3c::Dom::INode;
using Org::W3c::Dom::INodeList;
using Org::Xml::Sax::CInputSource;
using Org::Xml::Sax::IInputSource;

namespace Org {
namespace Alljoyn {
namespace Bus {

const Int32 AboutData::FieldDetails::EMPTY_MASK;
const Int32 AboutData::FieldDetails::REQUIRED;
const Int32 AboutData::FieldDetails::ANNOUNCED;
const Int32 AboutData::FieldDetails::LOCALIZED;

AutoPtr<ArrayOf<Char32> > AboutData::sHexCharArray = String("0123456789ABCDEF").GetChars();

CAR_INTERFACE_IMPL_3(AboutData, Object, IAboutData, IAboutDataListener, IAboutKeys)

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
ECode AboutData::constructor()
{
    InitializeFieldDetails();
    CHashMap::New((IMap**)&mPropertyStore);
    CHashMap::New((IMap**)&mLocalizedPropertyStore);
    CTreeSet::New((ISet**)&mSupportedLanguages);
    AutoPtr<IVariant> variant;
    CVariant::New(CoreUtils::Convert(Version::Get()), (IVariant**)&variant);
    if (FAILED(SetField(ABOUT_AJ_SOFTWARE_VERSION, variant))) {
        assert(0 && "Failed to set the AllJoyn software version.");
    }

    return NOERROR;
}

/**
 * constructor
 *
 * @param defaultLanguage a Locale containing a IETF language tag specified
 *                        by RFC 5646 specifying the default language for the
 *                        AboutData fields
 * @throws BusException
 */
ECode AboutData::constructor(
    /* [in] */ const String& defaultLanguage)
{
    InitializeFieldDetails();
    CHashMap::New((IMap**)&mPropertyStore);
    CHashMap::New((IMap**)&mLocalizedPropertyStore);
    CTreeSet::New((ISet**)&mSupportedLanguages);
    AutoPtr<IVariant> variant;
    CVariant::New(CoreUtils::Convert(Version::Get()), (IVariant**)&variant);
    if (FAILED(SetField(ABOUT_AJ_SOFTWARE_VERSION, variant))) {
        assert(0 && "Failed to set the AllJoyn software version.");
    }
    variant = NULL;
    CVariant::New(CoreUtils::Convert(Version::Get()), (IVariant**)&variant);
    if (FAILED(SetField(ABOUT_DEFAULT_LANGUAGE, variant))) {
        assert(0 && "Failed to set the default language.");
    }

    return NOERROR;
}

/**
 * constructor
 *
 * All Localized tags will be set to the default language.
 *
 * @param aboutData Map containing the AboutData dictionary
 *
 * @throws BusException
 */
ECode AboutData::constructor(
    /* [in] */ IMap* aboutData)
{
    InitializeFieldDetails();
    CHashMap::New((IMap**)&mPropertyStore);
    CHashMap::New((IMap**)&mLocalizedPropertyStore);
    CTreeSet::New((ISet**)&mSupportedLanguages);
    return CreateFromAnnoncedAboutData(aboutData, String(NULL));
}

/**
 * constructor
 *
 * @param aboutData Map containing the AboutData dictionary
 * @param language a Locale containing a IETF language tag specified by RFC 5646
 *
 * @throws BusException
 */

ECode AboutData::constructor(
    /* [in] */ IMap* aboutData,
    /* [in] */ const String& language)
{
    InitializeFieldDetails();
    CHashMap::New((IMap**)&mPropertyStore);
    CHashMap::New((IMap**)&mLocalizedPropertyStore);
    CTreeSet::New((ISet**)&mSupportedLanguages);
    return CreateFromAnnoncedAboutData(aboutData, language);
}

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
ECode AboutData::CreateFromXml(
    /* [in] */ const String& aboutDataXml)
{
    AutoPtr<IDocumentBuilderFactoryHelper> dbfHelper;
    CDocumentBuilderFactoryHelper::AcquireSingleton((IDocumentBuilderFactoryHelper**)&dbfHelper);
    AutoPtr<IDocumentBuilderFactory> factory;
    dbfHelper->NewInstance((IDocumentBuilderFactory**)&factory);
    AutoPtr<IDocumentBuilder> builder;
    Boolean IsDefaultLangaugeMissing = FALSE;
    factory->NewDocumentBuilder((IDocumentBuilder**)&builder);

    AutoPtr<IInputSource> inputSource;
    CInputSource::New((IInputSource**)&inputSource);
    AutoPtr<IReader> reader;
    CStringReader::New(aboutDataXml, (IReader**)&reader);
    inputSource->SetCharacterStream(reader);
    AutoPtr<IDocument> doc;
    builder->Parse(inputSource, (IDocument**)&doc);
    // First process all fields that are not localized so we can get the
    // default language
    AutoPtr<IUUIDHelper> uuidHelper;
    CUUIDHelper::AcquireSingleton((IUUIDHelper**)&uuidHelper);
    AutoPtr<ISet> keySet;
    mAboutFields->GetKeySet((ISet**)&keySet);
    AutoPtr<ArrayOf<IInterface*> > keyArray;
    keySet->ToArray((ArrayOf<IInterface*>**)&keyArray);
    for (Int32 i = 0; i < keyArray->GetLength(); ++i) {
        String field;
        ICharSequence::Probe((*keyArray)[i])->ToString(&field);
        // Supported languages are implicitly added no need to look for a
        // SupportedLanguages languages tag.
        if (field.Equals(ABOUT_SUPPORTED_LANGUAGES)) {
            continue;
        }
        // The Alljoyn software version is implicitly added so we don't need to
        // look for this tag
        if (field.Equals(ABOUT_AJ_SOFTWARE_VERSION)) {
            continue;
        }
        // We only expect to see one tag for non-localized values.
        // So we automatically take the first field seen by the parser.
        Boolean res;
        IsFieldLocalized(field, &res);
        if (!res) {
            AutoPtr<INodeList> nl;
            doc->GetElementsByTagName(field, (INodeList**)&nl);
            if (nl == NULL) {
                continue;
            }
            // if the NodeList length is not greater than 0 then the
            // element was not found. We continue to look for the
            // next expected element.
            Int32 length;
            nl->GetLength(&length);
            if (length <= 0) {
                continue;
            }
            AutoPtr<INode> node;
            nl->Item(0, (INode**)&node);
            String text;
            node->GetTextContent(&text);
            if (field.Equals(ABOUT_APP_ID)) {
                // try to construct the AppId using a UUID if that fails
                // due to an IllegalArgumentException or a BusException
                // try to construct the AppId using a String.
                AutoPtr<IUUID> id;
                if (FAILED(uuidHelper->FromString(text, (IUUID**)&id)) || FAILED(SetAppId(id))) {
                    FAIL_RETURN(SetAppId(text))
                }
            }
            else {
                AutoPtr<IVariant> variant;
                CVariant::New(CoreUtils::Convert(text), (IVariant**)&variant);
                FAIL_RETURN(SetField(field, variant))
            }
        }
    }
    // At this point we should have the default language if its not set
    // we will throw a BusException if the 'lang' attribute is not set.
    AutoPtr<INodeList> nl;
    doc->GetElementsByTagName(String("*"), (INodeList**)&nl);
    Int32 length;
    nl->GetLength(&length);
    for (Int32 i = 0; i < length; ++i) {
        // If the Tag is unknown is will be added using the default rules
        // if the Tag is already known we check to see if its a localized
        // field.  If the field is localized we process the tag. If its
        // not localized we ignore the tag since it was processed above
        AutoPtr<INode> node;
        nl->Item(i, (INode**)&node);
        String name;
        node->GetNodeName(&name);
        Boolean containsKey, res;
        mAboutFields->ContainsKey(CoreUtils::Convert(name), &containsKey);
        if (!containsKey || (IsFieldLocalized(name, &res), res)) {
            // if the 'lang' attribute is found us the language
            // specified.  Otherwise use the default language
            AutoPtr<INamedNodeMap> attrs;
            node->GetAttributes((INamedNodeMap**)&attrs);
            AutoPtr<INode> langNode;
            attrs->GetNamedItem(String("lang"), (INode**)&langNode);
            String text;
            node->GetTextContent(&text);
            AutoPtr<IVariant> variant;
            CVariant::New(CoreUtils::Convert(text), (IVariant**)&variant);
            if (langNode != NULL) {
                String value;
                langNode->GetNodeValue(&value);
                FAIL_RETURN(SetField(name, variant, value))
            }
            else {
                ECode ec = SetField(name, variant);
                if (FAILED(ec)) {
                    if (mErrorMessage.Equals("Specified language tag not found.")) {
                        IsDefaultLangaugeMissing = TRUE;
                        continue;
                    }
                    else {
                        return ec;
                    }
                }
            }
        }
    }
    if (IsDefaultLangaugeMissing) {
        // throw new BusException("DefaultLanguage language tag not found.");
        Logger::E("AboutData", "DefaultLanguage language tag not found.");
        return E_BUS_EXCEPTION;
    }
    return NOERROR;
}

/**
 * The AboutData has all of the required fields
 *
 * If no language is given default language will be checked
 *
 * @param language a Locale containing a IETF language tag specified by RFC 5646
 *
 * @return true if all required field are listed for the given language
 */
ECode AboutData::IsValid(
    /* [in] */ const String& _language,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;
    AutoPtr<ICharSequence> language;
    if (_language.IsNullOrEmpty()) {
        AutoPtr<IInterface> value;
        mPropertyStore->Get(CoreUtils::Convert(ABOUT_DEFAULT_LANGUAGE), (IInterface**)&value);
        AutoPtr<IInterface> obj;
        if (FAILED(IVariant::Probe(value)->GetObject((IInterface**)&obj)))
            return NOERROR;
        language = ICharSequence::Probe(obj);
    }
    else
        language = CoreUtils::Convert(_language);

    Boolean contains;
    mSupportedLanguages->Contains(language, &contains);
    if (!contains) {
        return NOERROR;
    }

    AutoPtr<ISet> keySet;
    mAboutFields->GetKeySet((ISet**)&keySet);
    AutoPtr<ArrayOf<IInterface*> > keyArray;
    keySet->ToArray((ArrayOf<IInterface*>**)&keyArray);
    for (Int32 i = 0; i < keyArray->GetLength(); ++i) {
        ICharSequence* field = ICharSequence::Probe((*keyArray)[i]);
        String s;
        field->ToString(&s);
        Boolean result = FALSE;
        if (IsFieldRequired(s, &result), result) {
            if (IsFieldLocalized(s, &result), result) {
                AutoPtr<IInterface> value;
                if (!(mLocalizedPropertyStore->ContainsKey(field, &result), result)
                    || !(mLocalizedPropertyStore->Get(field, (IInterface**)&value),
                        IMap::Probe(value)->ContainsKey(language, &result), result)) {
                    return NOERROR;
                }
            }
            else {
                mPropertyStore->ContainsKey(field, &result);
                if (!result) {
                    return NOERROR;
                }
            }
        }
    }
    *res = TRUE;
    return NOERROR;
}

/**
 * The AboutData has all of the required fields.
 *
 * Only fields for the default language will be checked.
 *
 * @return true if all required field are listed for the given language
 */
ECode AboutData::IsValid(
    /* [out] */ Boolean* res)
{
    return IsValid(String(NULL), res);
}

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
ECode AboutData::CreateFromAnnoncedAboutData(
    /* [in] */ IMap* aboutData,
    /* [in] */ const String& _language)
{
    if (aboutData == NULL) {
        return E_ERROR_REPLY_BUS_EXCEPTION;
        // throw new ErrorReplyBusException(Status.ABOUT_ABOUTDATA_MISSING_REQUIRED_FIELD);
    }
    String language = _language;
    if (language.IsNullOrEmpty()) {
        AutoPtr<IInterface> defaultLangArg;
        aboutData->Get(CoreUtils::Convert(ABOUT_DEFAULT_LANGUAGE), (IInterface**)&defaultLangArg);
        if (defaultLangArg == NULL) {
            return E_ERROR_REPLY_BUS_EXCEPTION;
            // throw new ErrorReplyBusException(Status.ABOUT_ABOUTDATA_MISSING_REQUIRED_FIELD);
        }
        else {
            AutoPtr<IInterface> value;
            IVariant::Probe(defaultLangArg)->GetObject((IInterface**)&value);
            ICharSequence::Probe(value)->ToString(&language);;
        }
    }

    if (language.IsNullOrEmpty()) {
        return E_ERROR_REPLY_BUS_EXCEPTION;
        // throw new ErrorReplyBusException(Status.ABOUT_ABOUTDATA_MISSING_REQUIRED_FIELD);
    }

    AutoPtr<ISet> keySet;
    aboutData->GetKeySet((ISet**)&keySet);
    AutoPtr<ArrayOf<IInterface*> > keyArray;
    keySet->ToArray((ArrayOf<IInterface*>**)&keyArray);
    for (Int32 i = 0; i < keyArray->GetLength(); ++i) {
        ICharSequence* field = ICharSequence::Probe((*keyArray)[i]);
        String s;
        field->ToString(&s);
        AutoPtr<IInterface> value;
        aboutData->Get(field, (IInterface**)&value);
        FAIL_RETURN(SetField(s, IVariant::Probe(value), language))
    }
    return NOERROR;
}

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
ECode AboutData::CreateFromAnnouncedAboutData(
    /* [in] */ IMap* aboutData)
{
    return CreateFromAnnoncedAboutData(aboutData, String(NULL));
}

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
ECode AboutData::SetAppId(
    /* [in] */ ArrayOf<Byte>* appId)
{
    AutoPtr<IVariant> variant;
    CVariant::New(CoreUtils::Convert(appId), (IVariant**)&variant);
    FAIL_RETURN(SetField(ABOUT_APP_ID, variant))
    if (appId->GetLength() != 16) {
        Logger::E("AboutData", "AppId is not 128-bits. AppId passed in is still used.");
        return E_BUS_EXCEPTION;
    }
    return NOERROR;
}

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
ECode AboutData::GetAppId(
    /* [out, callee] */ ArrayOf<Byte>** appId)
{
    VALIDATE_NOT_NULL(appId)
    AutoPtr<IVariant> variant;
    GetField(ABOUT_APP_ID, (IVariant**)&variant);
    AutoPtr<IArrayOf> value;
    variant->GetArrayObject((IArrayOf**)&value);
    AutoPtr<MsgArg::CarValue> carValue = MsgArg::CarValue::Convert(value);
    *appId = *(ArrayOf<Byte>**)carValue->ToValuePtr();
    REFCOUNT_ADD(*appId);
    return NOERROR;
}

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
ECode AboutData::SetAppId(
    /* [in] */ const String& appId)
{
    AutoPtr<ArrayOf<Byte> > array;
    FAIL_RETURN(HexStringToByteArray(appId, (ArrayOf<Byte>**)&array))
    return SetAppId(array);
}

ECode AboutData::HexStringToByteArray(
    /* [in] */ const String& hexString,
    /* [out] */ ArrayOf<Byte>** array)
{
    Int32 length = hexString.GetLength();
    if ((length % 2) != 0) {
        Logger::E("AboutData", "Failed to parse AppId.");
        return E_BUS_EXCEPTION;
    }
    AutoPtr<ArrayOf<Byte> > byteArray = ArrayOf<Byte>::Alloc(length/2);
    for (Int32 i = 0; i < length; i += 2) {
        (*byteArray)[i/2] = (Byte)((Character::ToDigit(hexString.GetChar(i), 16) << 4) +
                            Character::ToDigit(hexString.GetChar(i + 1), 16));
    }
    *array = byteArray;
    REFCOUNT_ADD(*array)
    return NOERROR;
}

ECode AboutData::GetAppIdAsHexString(
    /* [out] */ String* appId)
{
    VALIDATE_NOT_NULL(appId)
    AutoPtr<IVariant> variant;
    GetField(ABOUT_APP_ID, (IVariant**)&variant);
    AutoPtr<IArrayOf> value;
    variant->GetArrayObject((IArrayOf**)&value);
    AutoPtr<MsgArg::CarValue> carValue = MsgArg::CarValue::Convert(value);
    AutoPtr<ArrayOf<Byte> > array = *(ArrayOf<Byte>**)carValue->ToValuePtr();
    *appId = ByteArrayToHexString(array);
    return NOERROR;
}

String AboutData::ByteArrayToHexString(
    /* [in] */ ArrayOf<Byte>* byteArray)
{
    Int32 length = byteArray->GetLength();
    AutoPtr<ArrayOf<Char32> > hexChars = ArrayOf<Char32>::Alloc(length * 2);
    for (Int32 i = 0; i < length; ++i) {
        Int32 x = (*byteArray)[i] & 0xff;
        (*hexChars)[i*2] = (*sHexCharArray)[x >> 4];
        (*hexChars)[i*2 + 1] = (*sHexCharArray)[x & 0x0f];
    }
    return String(*hexChars);
}

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
ECode AboutData::SetAppId(
    /* [in] */ IUUID* appId)
{
    AutoPtr<ArrayOf<Byte> > appIdBytes = ArrayOf<Byte>::Alloc(16);
    AutoPtr<IByteBufferHelper> bbHelper;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&bbHelper);
    AutoPtr<IByteBuffer> byteBuffer;
    bbHelper->Wrap(appIdBytes, (IByteBuffer**)&byteBuffer);
    Int64 bits;
    appId->GetMostSignificantBits(&bits);
    byteBuffer->PutInt64(bits);
    appId->GetLeastSignificantBits(&bits);
    byteBuffer->PutInt64(bits);
    return SetAppId(appIdBytes);
}

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
ECode AboutData::GetAppIdAsUUID(
    /* [out] */ IUUID** appId)
{
    VALIDATE_NOT_NULL(appId)
    AutoPtr<IVariant> variant;
    GetField(ABOUT_APP_ID, (IVariant**)&variant);
    AutoPtr<IArrayOf> value;
    variant->GetArrayObject((IArrayOf**)&value);
    AutoPtr<MsgArg::CarValue> carValue = MsgArg::CarValue::Convert(value);
    AutoPtr<ArrayOf<Byte> > appIdBytes = *(ArrayOf<Byte>**)carValue->ToValuePtr();

    AutoPtr<IByteBufferHelper> bbHelper;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&bbHelper);
    AutoPtr<IByteBuffer> byteBuffer;
    bbHelper->Wrap(appIdBytes, (IByteBuffer**)&byteBuffer);
    Int64 bits1, bits2;
    byteBuffer->GetInt64(&bits1);
    byteBuffer->GetInt64(&bits2);
    return CUUID::New(bits1, bits2, appId);
}

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
ECode AboutData::SetDefaultLanguage(
    /* [in] */ const String& defaultLanguage)
{
    AutoPtr<IVariant> variant;
    CVariant::New(CoreUtils::Convert(defaultLanguage), (IVariant**)&variant);
    return SetField(ABOUT_DEFAULT_LANGUAGE, variant);
}

/**
 * Get the DefaultLanguage from the AboutData
 *
 * @return Locale indicating the default language.
 * @throws BusException indicating failure to get the default language
 */
ECode AboutData::GetDefaultLanguage(
    /* [out] */ String* language)
{
    VALIDATE_NOT_NULL(language)
    AutoPtr<IVariant> variant;
    GetField(ABOUT_DEFAULT_LANGUAGE, (IVariant**)&variant);
    AutoPtr<IInterface> value;
    variant->GetObject((IInterface**)&value);
    return ICharSequence::Probe(value)->ToString(language);
}

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
ECode AboutData::SetDeviceName(
    /* [in] */ const String& deviceName,
    /* [in] */ const String& language)
{
    AutoPtr<IVariant> variant;
    CVariant::New(CoreUtils::Convert(deviceName), (IVariant**)&variant);
    return SetField(ABOUT_DEVICE_NAME, variant, language);
}

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
ECode AboutData::SetDeviceName(
    /* [in] */ const String& deviceName)
{
    return SetDeviceName(deviceName, String(NULL));
}

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
ECode AboutData::GetDeviceName(
    /* [in] */ const String& language,
    /* [out] */ String* deviceName)
{
    VALIDATE_NOT_NULL(deviceName)
    AutoPtr<IVariant> variant;
    GetField(ABOUT_DEVICE_NAME, (IVariant**)&variant);
    AutoPtr<IInterface> value;
    variant->GetObject((IInterface**)&value);
    return ICharSequence::Probe(value)->ToString(deviceName);
}

ECode AboutData::GetDeviceName(
    /* [out] */ String* deviceName)
{
    return GetDeviceName(String(NULL), deviceName);
}

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
ECode AboutData::SetDeviceId(
    /* [in] */ const String& deviceId)
{
    AutoPtr<IVariant> variant;
    CVariant::New(CoreUtils::Convert(deviceId), (IVariant**)&variant);
    return SetField(ABOUT_DEVICE_ID, variant);
}
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
ECode AboutData::GetDeviceId(
    /* [out] */ String* deviceId)
{
    VALIDATE_NOT_NULL(deviceId)
    AutoPtr<IVariant> variant;
    GetField(ABOUT_DEVICE_ID, (IVariant**)&variant);
    AutoPtr<IInterface> value;
    variant->GetObject((IInterface**)&value);
    return ICharSequence::Probe(value)->ToString(deviceId);
}

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
ECode AboutData::SetAppName(
    /* [in] */ const String& appName,
    /* [in] */ const String& language)
{
    AutoPtr<IVariant> variant;
    CVariant::New(CoreUtils::Convert(appName), (IVariant**)&variant);
    return SetField(ABOUT_APP_NAME, variant, language);
}

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
ECode AboutData::SetAppName(
    /* [in] */ const String& appName)
{
    return SetAppName(appName, String(NULL));
}

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
ECode AboutData::GetAppName(
    /* [in] */ const String& language,
    /* [out] */ String* appName)
{
    VALIDATE_NOT_NULL(appName)
    AutoPtr<IVariant> variant;
    GetField(ABOUT_APP_NAME, (IVariant**)&variant);
    AutoPtr<IInterface> value;
    variant->GetObject((IInterface**)&value);
    return ICharSequence::Probe(value)->ToString(appName);
}

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
ECode AboutData::GetAppName(
    /* [out] */ String* appName)
{
    return GetAppName(String(NULL), appName);
}

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
ECode AboutData::SetManufacturer(
    /* [in] */ const String& manufacturer,
    /* [in] */ const String& language)
{
    AutoPtr<IVariant> variant;
    CVariant::New(CoreUtils::Convert(manufacturer), (IVariant**)&variant);
    return SetField(ABOUT_MANUFACTURER, variant, language);
}

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
ECode AboutData::SetManufacturer(
    /* [in] */ const String& manufacturer)
{
    return SetManufacturer(manufacturer, String(NULL));
}

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
ECode AboutData::GetManufacturer(
    /* [in] */ const String& language,
    /* [out] */ String* manufacturer)
{
    VALIDATE_NOT_NULL(manufacturer)
    AutoPtr<IVariant> variant;
    GetField(ABOUT_MANUFACTURER, (IVariant**)&variant);
    AutoPtr<IInterface> value;
    variant->GetObject((IInterface**)&value);
    return ICharSequence::Probe(value)->ToString(manufacturer);
}

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
ECode AboutData::GetManufacturer(
    /* [out] */ String* manufacturer)
{
    return GetManufacturer(String(NULL), manufacturer);
}

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
ECode AboutData::SetModelNumber(
    /* [in] */ const String& modelNumber)
{
    AutoPtr<IVariant> variant;
    CVariant::New(CoreUtils::Convert(modelNumber), (IVariant**)&variant);
    return SetField(ABOUT_MODEL_NUMBER, variant);
}

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
ECode AboutData::GetModelNumber(
    /* [out] */ String* modelNumber)
{
    VALIDATE_NOT_NULL(modelNumber)
    AutoPtr<IVariant> variant;
    GetField(ABOUT_MODEL_NUMBER, (IVariant**)&variant);
    AutoPtr<IInterface> value;
    variant->GetObject((IInterface**)&value);
    return ICharSequence::Probe(value)->ToString(modelNumber);
}

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
ECode AboutData::SetSupportedLanguage(
    /* [in] */ const String& language)
{
    mSupportedLanguages->Add(CoreUtils::Convert(language));
    AutoPtr<ArrayOf<IInterface*> > array;
    mSupportedLanguages->ToArray((ArrayOf<IInterface*>**)&array);
    Int32 length = array->GetLength();
    AutoPtr<IArrayOf> iarray;
    CArrayOf::New(EIID_ICharSequence, length, (IArrayOf**)&iarray);
    for (Int32 i = 0; i < length; i++)
        iarray->Set(i, (*array)[i]);
    AutoPtr<IVariant> variant;
    CVariant::New(iarray, (IVariant**)&variant);
    return SetField(ABOUT_SUPPORTED_LANGUAGES, variant);
}

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
ECode AboutData::SetSupportedLanguages(
    /* [in] */ ArrayOf<String>* languages)
{
    Int32 length = languages ? languages->GetLength() : 0;
    for (Int32 i = 0; i < length; i++)
        mSupportedLanguages->Add(CoreUtils::Convert((*languages)[i]));

    AutoPtr<ArrayOf<IInterface*> > array;
    mSupportedLanguages->ToArray((ArrayOf<IInterface*>**)&array);
    length = array->GetLength();
    AutoPtr<IArrayOf> iarray;
    CArrayOf::New(EIID_ICharSequence, length, (IArrayOf**)&iarray);
    for (Int32 i = 0; i < length; i++)
        iarray->Set(i, (*array)[i]);
    AutoPtr<IVariant> variant;
    CVariant::New(iarray, (IVariant**)&variant);
    return SetField(ABOUT_SUPPORTED_LANGUAGES, variant);
}
/**
 * Get and array of supported languages
 *
 * @return An array of locals containing languageTags.
 * @throws BusException indicating failure obtaining language tags
 */
ECode AboutData::GetSupportedLanguages(
    /* [out, callee] */ ArrayOf<String>** languages)
{
    AutoPtr<IVariant> variant;
    GetField(ABOUT_DEFAULT_LANGUAGE, (IVariant**)&variant);
    AutoPtr<IArrayOf> value;
    variant->GetArrayObject((IArrayOf**)&value);
    AutoPtr<MsgArg::CarValue> carValue = MsgArg::CarValue::Convert(value);
    *languages = *(ArrayOf<String>**)carValue->ToValuePtr();
    REFCOUNT_ADD(*languages)
    return NOERROR;
}


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
ECode AboutData::SetDescription(
    /* [in] */ const String& description,
    /* [in] */ const String& language)
{
    AutoPtr<IVariant> variant;
    CVariant::New(CoreUtils::Convert(description), (IVariant**)&variant);
    return SetField(ABOUT_DESCRIPTION, variant, language);
}

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
ECode AboutData::SetDescription(
    /* [in] */ const String& description)
{
    return SetDescription(description, String(NULL));
}

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
ECode AboutData::GetDescription(
    /* [in] */ const String& language,
    /* [out] */ String* description)
{
    VALIDATE_NOT_NULL(description)
    AutoPtr<IVariant> variant;
    GetField(ABOUT_DESCRIPTION, (IVariant**)&variant);
    AutoPtr<IInterface> value;
    variant->GetObject((IInterface**)&value);
    return ICharSequence::Probe(value)->ToString(description);
}

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
ECode AboutData::GetDescription(
    /* [out] */ String* description)
{
    return GetDescription(String(NULL), description);
}

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
ECode AboutData::SetDateOfManufacture(
    /* [in] */ const String& dateOfManufacture)
{
    AutoPtr<IVariant> variant;
    CVariant::New(CoreUtils::Convert(dateOfManufacture), (IVariant**)&variant);
    return SetField(ABOUT_DATE_OF_MANUFACTURE, variant);
}

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
ECode AboutData::GetDateOfManufacture(
    /* [out] */ String* dateOfManufacture)
{
    VALIDATE_NOT_NULL(dateOfManufacture)
    AutoPtr<IVariant> variant;
    GetField(ABOUT_DATE_OF_MANUFACTURE, (IVariant**)&variant);
    AutoPtr<IInterface> value;
    variant->GetObject((IInterface**)&value);
    return ICharSequence::Probe(value)->ToString(dateOfManufacture);
}

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
ECode AboutData::SetSoftwareVersion(
    /* [in] */ const String& softwareVersion)
{
    AutoPtr<IVariant> variant;
    CVariant::New(CoreUtils::Convert(softwareVersion), (IVariant**)&variant);
    return SetField(ABOUT_SOFTWARE_VERSION, variant);
}

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
ECode AboutData::GetSoftwareVersion(
    /* [out] */ String* softwareVersion)
{
    VALIDATE_NOT_NULL(softwareVersion)
    AutoPtr<IVariant> variant;
    GetField(ABOUT_SOFTWARE_VERSION, (IVariant**)&variant);
    AutoPtr<IInterface> value;
    variant->GetObject((IInterface**)&value);
    return ICharSequence::Probe(value)->ToString(softwareVersion);
}

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
ECode AboutData::GetAJSoftwareVersion(
    /* [out] */ String* softwareVersion)
{
    VALIDATE_NOT_NULL(softwareVersion)
    AutoPtr<IVariant> variant;
    GetField(ABOUT_AJ_SOFTWARE_VERSION, (IVariant**)&variant);
    AutoPtr<IInterface> value;
    variant->GetObject((IInterface**)&value);
    return ICharSequence::Probe(value)->ToString(softwareVersion);
}

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
ECode AboutData::SetHardwareVersion(
    /* [in] */ const String& hardwareVersion)
{
    AutoPtr<IVariant> variant;
    CVariant::New(CoreUtils::Convert(hardwareVersion), (IVariant**)&variant);
    return SetField(ABOUT_HARDWARE_VERSION, variant);
}

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
ECode AboutData::GetHardwareVersion(
    /* [out] */ String* hardwareVersion)
{
    VALIDATE_NOT_NULL(hardwareVersion)
    AutoPtr<IVariant> variant;
    GetField(ABOUT_HARDWARE_VERSION, (IVariant**)&variant);
    AutoPtr<IInterface> value;
    variant->GetObject((IInterface**)&value);
    return ICharSequence::Probe(value)->ToString(hardwareVersion);
}

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
ECode AboutData::SetSupportUrl(
    /* [in] */ const String& supportUrl)
{
    AutoPtr<IVariant> variant;
    CVariant::New(CoreUtils::Convert(supportUrl), (IVariant**)&variant);
    return SetField(ABOUT_SUPPORT_URL, variant);
}

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
ECode AboutData::GetSupportUrl(
    /* [out] */ String* supportUrl)
{
    VALIDATE_NOT_NULL(supportUrl)
    AutoPtr<IVariant> variant;
    GetField(ABOUT_SUPPORT_URL, (IVariant**)&variant);
    AutoPtr<IInterface> value;
    variant->GetObject((IInterface**)&value);
    return ICharSequence::Probe(value)->ToString(supportUrl);
}

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
ECode AboutData::SetField(
    /* [in] */ const String& name,
    /* [in] */ IVariant* value,
    /* [in] */ const String& _language)
{
    // The user is adding an OEM specific field.
    // At this time OEM specific fields are added as
    //    not required
    //    not announced
    //    can be localized
    AutoPtr<ICharSequence> csName = CoreUtils::Convert(name);
    Boolean result = FALSE;
    mAboutFields->ContainsKey(csName, &result);
    if (!result) {
        String signature;
        value->GetSignature(&signature);
        if (signature == NULL) {
            mErrorMessage = "Unable to find Variant signature.";
            Logger::E("AboutData", mErrorMessage);
            return E_BUS_EXCEPTION;
        }
        else if (signature.Equals("s")) {
            AutoPtr<IObject> fd = new FieldDetails(FieldDetails::LOCALIZED, signature);
            mAboutFields->Put(csName, fd);
        }
        else {
            AutoPtr<IObject> fd = new FieldDetails(FieldDetails::EMPTY_MASK, signature);
            mAboutFields->Put(csName, fd);
        }
    }
    if (name.Equals(ABOUT_DEFAULT_LANGUAGE)) {
        AutoPtr<IInterface> obj;
        value->GetObject((IInterface**)&obj);
        String lang;
        ICharSequence::Probe(obj)->ToString(&lang);
        SetSupportedLanguage(lang);
    }
    IsFieldLocalized(name, &result);
    if (result) {
        String language = _language;
        if (language.IsNullOrEmpty()) {
            mPropertyStore->ContainsKey(CoreUtils::Convert(ABOUT_DEFAULT_LANGUAGE), &result);
            if (result) {
                AutoPtr<IInterface> val;
                mPropertyStore->Get(CoreUtils::Convert(ABOUT_DEFAULT_LANGUAGE), (IInterface**)&val);
                AutoPtr<IInterface> obj;
                IVariant::Probe(val)->GetObject((IInterface**)&obj);
                ICharSequence::Probe(obj)->ToString(&language);
            }
            else {
                mErrorMessage = "Specified language tag not found.";
                Logger::E("AboutData", mErrorMessage);
                return E_BUS_EXCEPTION;
            }
            mLocalizedPropertyStore->ContainsKey(csName, &result);
            if (!result) {
                AutoPtr<IMap> treeMap;
                CTreeMap::New(/*String.CASE_INSENSITIVE_ORDER*/(IMap**)&treeMap);
                mLocalizedPropertyStore->Put(csName, treeMap);
            }
            AutoPtr<IInterface> treeMap;
            mLocalizedPropertyStore->Get(csName, (IInterface**)&treeMap);
            IMap::Probe(treeMap)->Put(CoreUtils::Convert(language), value);
        }
        else {
            mLocalizedPropertyStore->ContainsKey(csName, &result);
            if (!result) {
                AutoPtr<IMap> treeMap;
                CTreeMap::New(/*String.CASE_INSENSITIVE_ORDER*/(IMap**)&treeMap);
                mLocalizedPropertyStore->Put(csName, treeMap);
            }
            AutoPtr<IInterface> treeMap;
            mLocalizedPropertyStore->Get(csName, (IInterface**)&treeMap);
            IMap::Probe(treeMap)->Put(CoreUtils::Convert(language), value);

            //implicitly add all language tags to the supported languages
            mSupportedLanguages->Contains(CoreUtils::Convert(language), &result);
            if (!result) {
                SetSupportedLanguage(language);
            }
        }
    }
    else {
        mPropertyStore->Put(csName, value);
    }
    return NOERROR;
}

ECode AboutData::SetField(
    /* [in] */ const String& name,
    /* [in] */ IVariant* value)
{
    return SetField(name, value, String(NULL));
}

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
ECode AboutData::GetField(
    /* [in] */ const String& name,
    /* [in] */ const String& language,
    /* [out] */ IVariant** field)
{
    VALIDATE_NOT_NULL(field)
    AutoPtr<ICharSequence> csName = CoreUtils::Convert(name);
    Boolean result = FALSE;
    if (!(mPropertyStore->ContainsKey(csName, &result), result)
        && !(mLocalizedPropertyStore->ContainsKey(csName, &result), result)) {
        Logger::E("AboutData", "About Field Not Found.");
        return E_BUS_EXCEPTION;
    }
    IsFieldLocalized(name, &result);
    AutoPtr<IInterface> ret;
    if (!result) {
        return mPropertyStore->Get(csName, (IInterface**)&ret);
    }
    else {
        AutoPtr<ICharSequence> csLanguage;
        if (language.IsNullOrEmpty()) {
            AutoPtr<IInterface> v;
            mPropertyStore->Get(CoreUtils::Convert(ABOUT_DEFAULT_LANGUAGE), (IInterface**)&v);
            if (v == NULL) {
                Logger::E("AboutData", "DefaultLanguage language tag not found.");
                return E_BUS_EXCEPTION;
            }
            AutoPtr<IInterface> obj;
            IVariant::Probe(v)->GetObject((IInterface**)&obj);
            csLanguage = ICharSequence::Probe(obj);
        }
        else
            csLanguage = CoreUtils::Convert(language);

        mSupportedLanguages->Contains(csLanguage, &result);
        if (!result) {
            Logger::E("AboutData", "Specified language tag not found.");
            return E_BUS_EXCEPTION;
        }
        AutoPtr<IInterface> treeMap;
        mLocalizedPropertyStore->Get(csName, (IInterface**)&treeMap);
        IMap::Probe(treeMap)->Get(csLanguage, (IInterface**)&ret);
    }
    *field = IVariant::Probe(ret);
    REFCOUNT_ADD(*field)
    return NOERROR;
}

ECode AboutData::GetField(
    /* [in] */ const String& name,
    /* [out] */ IVariant** field)
{
    return GetField(name, String(NULL), field);
}

/**
 * Get a Set listing the fields contained in this AboutData class.  This may be
 * required if a the AboutData comes from a remote source. User defined
 * fields are permitted. Use the getFields method to get a list of all fields
 * currently known by the AboutData.
 *
 * @return
 *  Set containing a list of all known fields in the AboutData class.
 */
ECode AboutData::GetFields(
    /* [out] */ ISet** fields)
{
    return mAboutFields->GetKeySet(fields);
}


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
ECode AboutData::IsFieldRequired(
    /* [in] */ const String& fieldName,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    AutoPtr<IInterface> value;
    mAboutFields->Get(CoreUtils::Convert(fieldName), (IInterface**)&value);
    FieldDetails* fieldDetails = (FieldDetails*)IObject::Probe(value);
    if (fieldDetails == NULL) {
        *res = FALSE;
    }
    else {
        *res = ((fieldDetails->mFieldMask & FieldDetails::REQUIRED) == FieldDetails::REQUIRED);
    }
    return NOERROR;
}

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
ECode AboutData::IsFieldAnnounced(
    /* [in] */ const String& fieldName,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    AutoPtr<IInterface> value;
    mAboutFields->Get(CoreUtils::Convert(fieldName), (IInterface**)&value);
    FieldDetails* fieldDetails = (FieldDetails*)IObject::Probe(value);
    if (fieldDetails == NULL) {
        *res = FALSE;
    }
    else {
        *res = ((fieldDetails->mFieldMask & FieldDetails::ANNOUNCED) == FieldDetails::ANNOUNCED);
    }
    return NOERROR;
}

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
ECode AboutData::IsFieldLocalized(
    /* [in] */ const String& fieldName,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    AutoPtr<IInterface> value;
    mAboutFields->Get(CoreUtils::Convert(fieldName), (IInterface**)&value);
    FieldDetails* fieldDetails = (FieldDetails*)IObject::Probe(value);
    if (fieldDetails == NULL) {
        *res = FALSE;
    }
    else {
        *res = ((fieldDetails->mFieldMask & FieldDetails::LOCALIZED) == FieldDetails::LOCALIZED);
    }
    return NOERROR;
}

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
String AboutData::GetFieldSignature(
    /* [in] */ const String& fieldName)
{
    AutoPtr<IInterface> value;
    mAboutFields->Get(CoreUtils::Convert(fieldName), (IInterface**)&value);
    FieldDetails* fieldDetails = (FieldDetails*)IObject::Probe(value);
    if (fieldDetails == NULL) {
        return String(NULL);
    }
    else {
        return fieldDetails->mSignature;
    }
}

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
ECode AboutData::GetAboutData(
    /* [in] */ const String& language,
    /* [out] */ IMap** aboutData)
{
    VALIDATE_NOT_NULL(aboutData)
    AutoPtr<ICharSequence> csLanguage;
    if (language.IsNullOrEmpty()) {
        AutoPtr<IInterface> v;
        mPropertyStore->Get(CoreUtils::Convert(ABOUT_DEFAULT_LANGUAGE), (IInterface**)&v);
        assert(v != NULL);
        AutoPtr<IInterface> obj;
        if (FAILED(IVariant::Probe(v)->GetObject((IInterface**)&obj))) {
            return E_ERROR_REPLY_BUS_EXCEPTION;
            // throw new ErrorReplyBusException(Status.LANGUAGE_NOT_SUPPORTED);
        }
        csLanguage = ICharSequence::Probe(obj);
    }
    else
        csLanguage = CoreUtils::Convert(language);

    Boolean result = FALSE;
    mSupportedLanguages->Contains(csLanguage, &result);
    if (!result) {
        return E_ERROR_REPLY_BUS_EXCEPTION;
        // throw new ErrorReplyBusException(Status.LANGUAGE_NOT_SUPPORTED);
    }

    AutoPtr<ISet> keySet;
    mAboutFields->GetKeySet((ISet**)&keySet);
    AutoPtr<ArrayOf<IInterface*> > keyArray;
    keySet->ToArray((ArrayOf<IInterface*>**)&keyArray);
    for (Int32 i = 0; i < keyArray->GetLength(); ++i) {
        IInterface* key = (*keyArray)[i];
        String s;
        ICharSequence::Probe(key)->ToString(&s);
        if (IsFieldRequired(s, &result), result) {
            if (IsFieldLocalized(s, &result), result) {
                AutoPtr<IInterface> value;
                if (!(mLocalizedPropertyStore->ContainsKey(key, &result), result)
                    || !(mLocalizedPropertyStore->Get(key, (IInterface**)&value),
                        IMap::Probe(value)->ContainsKey(csLanguage, &result), result)) {
                    return E_ERROR_REPLY_BUS_EXCEPTION;
                    // throw new ErrorReplyBusException(Status.ABOUT_ABOUTDATA_MISSING_REQUIRED_FIELD);
                }
            }
            else {
                mPropertyStore->ContainsKey(key, &result);
                if (!result) {
                    return E_ERROR_REPLY_BUS_EXCEPTION;
                    // throw new ErrorReplyBusException(Status.ABOUT_ABOUTDATA_MISSING_REQUIRED_FIELD);
                }
            }
        }
    }

    CHashMap::New(mPropertyStore, aboutData);
    keySet = NULL;
    mLocalizedPropertyStore->GetKeySet((ISet**)&keySet);
    keyArray = NULL;
    keySet->ToArray((ArrayOf<IInterface*>**)&keyArray);
    for (Int32 i = 0; i < keyArray->GetLength(); ++i) {
        IInterface* s = (*keyArray)[i];
        AutoPtr<IInterface> value;
        mLocalizedPropertyStore->Get((*keyArray)[i], (IInterface**)&value);
        IMap* treeMap = IMap::Probe(value);
        treeMap->ContainsKey(csLanguage, &result);
        if (result){
            AutoPtr<IInterface> val;
            treeMap->Get(csLanguage, (IInterface**)&val);
            (*aboutData)->Put(s, val);
        }
    }
    return NOERROR;
}

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
ECode AboutData::GetAnnouncedAboutData(
    /* [out] */ IMap** aboutData)
{
    VALIDATE_NOT_NULL(aboutData)
    AutoPtr<ICharSequence> csLanguage;
    AutoPtr<IInterface> v;
    mPropertyStore->Get(CoreUtils::Convert(ABOUT_DEFAULT_LANGUAGE), (IInterface**)&v);
    assert(v != NULL);
    AutoPtr<IInterface> obj;
    if (FAILED(IVariant::Probe(v)->GetObject((IInterface**)&obj))) {
        return E_ERROR_REPLY_BUS_EXCEPTION;
        // throw new ErrorReplyBusException(Status.LANGUAGE_NOT_SUPPORTED);
    }
    csLanguage = ICharSequence::Probe(obj);

    Boolean result = FALSE;
    mSupportedLanguages->Contains(csLanguage, &result);
    if (!result) {
        return E_ERROR_REPLY_BUS_EXCEPTION;
        // throw new ErrorReplyBusException(Status.LANGUAGE_NOT_SUPPORTED);
    }

    AutoPtr<ISet> keySet;
    mAboutFields->GetKeySet((ISet**)&keySet);
    AutoPtr<ArrayOf<IInterface*> > keyArray;
    keySet->ToArray((ArrayOf<IInterface*>**)&keyArray);
    for (Int32 i = 0; i < keyArray->GetLength(); ++i) {
        IInterface* key = (*keyArray)[i];
        String s;
        ICharSequence::Probe(key)->ToString(&s);
        if (IsFieldRequired(s, &result), result) {
            if (IsFieldLocalized(s, &result), result) {
                AutoPtr<IInterface> value;
                if (!(mLocalizedPropertyStore->ContainsKey(key, &result), result)
                    || !(mLocalizedPropertyStore->Get(key, (IInterface**)&value),
                        IMap::Probe(value)->ContainsKey(csLanguage, &result), result)) {
                    return E_ERROR_REPLY_BUS_EXCEPTION;
                    // throw new ErrorReplyBusException(Status.ABOUT_ABOUTDATA_MISSING_REQUIRED_FIELD);
                }
            }
            else {
                mPropertyStore->ContainsKey(key, &result);
                if (!result) {
                    return E_ERROR_REPLY_BUS_EXCEPTION;
                    // throw new ErrorReplyBusException(Status.ABOUT_ABOUTDATA_MISSING_REQUIRED_FIELD);
                }
            }
        }
    }

    CHashMap::New(aboutData);
    for (Int32 i = 0; i < keyArray->GetLength(); ++i) {
        IInterface* key = (*keyArray)[i];
        String s;
        ICharSequence::Probe(key)->ToString(&s);
        if (IsFieldAnnounced(s, &result), result) {
            if (IsFieldLocalized(s, &result), result) {
                AutoPtr<IInterface> value;
                if ((mLocalizedPropertyStore->ContainsKey(key, &result), result)
                    && (mLocalizedPropertyStore->Get(key, (IInterface**)&value),
                        IMap::Probe(value)->ContainsKey(csLanguage, &result), result)) {
                    AutoPtr<IInterface> val;
                    IMap::Probe(value)->Get(csLanguage, (IInterface**)&val);
                    (*aboutData)->Put(key, val);
                }
            }
            else {
                mPropertyStore->ContainsKey(key, &result);
                if (result) {
                    AutoPtr<IInterface> val;
                    mPropertyStore->Get(key, (IInterface**)&val);
                    (*aboutData)->Put(key, val);
                }
            }
        }
    }
    return NOERROR;
}

void AboutData::SetNewFieldDetails(
    /* [in] */ const String& fieldName,
    /* [in] */ Int32 aboutFieldMask,
    /* [in] */ const String& signature)
{
    AutoPtr<IObject> fd = new FieldDetails(aboutFieldMask, signature);
    mAboutFields->Put(CoreUtils::Convert(fieldName), fd);
}

void AboutData::InitializeFieldDetails()
{
    mAboutFields = NULL;
    CHashMap::New((IMap**)&mAboutFields);
    AutoPtr<IObject> fd = new FieldDetails(FieldDetails::REQUIRED | FieldDetails::ANNOUNCED, String("ay"));
    mAboutFields->Put(CoreUtils::Convert(ABOUT_APP_ID), fd);
    fd = new FieldDetails(FieldDetails::REQUIRED | FieldDetails::ANNOUNCED, String("s"));
    mAboutFields->Put(CoreUtils::Convert(ABOUT_DEFAULT_LANGUAGE), fd);
    fd = new FieldDetails(FieldDetails::ANNOUNCED | FieldDetails::LOCALIZED, String("s"));
    mAboutFields->Put(CoreUtils::Convert(ABOUT_DEVICE_NAME), fd);
    fd = new FieldDetails(FieldDetails::REQUIRED | FieldDetails::ANNOUNCED, String("s"));
    mAboutFields->Put(CoreUtils::Convert(ABOUT_DEVICE_ID), fd);
    fd = new FieldDetails(FieldDetails::REQUIRED | FieldDetails::ANNOUNCED | FieldDetails::LOCALIZED, String("s"));
    mAboutFields->Put(CoreUtils::Convert(ABOUT_APP_NAME), fd);
    fd = new FieldDetails(FieldDetails::REQUIRED | FieldDetails::ANNOUNCED | FieldDetails::LOCALIZED, String("s"));
    mAboutFields->Put(CoreUtils::Convert(ABOUT_MANUFACTURER), fd);
    fd = new FieldDetails(FieldDetails::REQUIRED | FieldDetails::ANNOUNCED, String("s"));
    mAboutFields->Put(CoreUtils::Convert(ABOUT_MODEL_NUMBER), fd);
    fd = new FieldDetails(FieldDetails::REQUIRED, String("as"));
    mAboutFields->Put(CoreUtils::Convert(ABOUT_SUPPORTED_LANGUAGES), fd);
    fd = new FieldDetails(FieldDetails::REQUIRED | FieldDetails::LOCALIZED, String("s"));
    mAboutFields->Put(CoreUtils::Convert(ABOUT_DESCRIPTION), fd);
    fd = new FieldDetails(FieldDetails::EMPTY_MASK, String("s"));
    mAboutFields->Put(CoreUtils::Convert(ABOUT_DATE_OF_MANUFACTURE), fd);
    fd = new FieldDetails(FieldDetails::REQUIRED, String("s"));
    mAboutFields->Put(CoreUtils::Convert(ABOUT_SOFTWARE_VERSION), fd);
    fd = new FieldDetails(FieldDetails::REQUIRED, String("s"));
    mAboutFields->Put(CoreUtils::Convert(ABOUT_AJ_SOFTWARE_VERSION), fd);
    fd = new FieldDetails(FieldDetails::EMPTY_MASK, String("s"));
    mAboutFields->Put(CoreUtils::Convert(ABOUT_HARDWARE_VERSION), fd);
    fd = new FieldDetails(FieldDetails::EMPTY_MASK, String("s"));
    mAboutFields->Put(CoreUtils::Convert(ABOUT_SUPPORT_URL), fd);
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
