
#include <Elastos.CoreLibrary.External.h>
#include "Elastos.Droid.Content.h"
#include "elastos/droid/internal/os/CPowerProfile.h"
#include "elastos/droid/internal/utility/XmlUtils.h"
#include "elastos/droid/R.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Core::CArrayOf;
using Elastos::Core::IArrayOf;
using Elastos::Core::CDouble;
using Elastos::Core::IDouble;
using Elastos::Core::EIID_IDouble;
using Elastos::Core::StringUtils;
using Elastos::Utility::Etl::List;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

HashMap<String, AutoPtr<IInterface> > CPowerProfile::sPowerMap;
const String CPowerProfile::TAG_DEVICE("device");
const String CPowerProfile::TAG_ITEM("item");
const String CPowerProfile::TAG_ARRAY("array");
const String CPowerProfile::TAG_ARRAYITEM("value");
const String CPowerProfile::ATTR_NAME("name");

CAR_INTERFACE_IMPL(CPowerProfile, Object, IPowerProfile)

CAR_OBJECT_IMPL(CPowerProfile)

ECode CPowerProfile::constructor(
    /* [in] */ IContext* context)
{
    // Read the XML file for the given profile (normally only one per
    // device)
    if (sPowerMap.GetSize() == 0) {
        return ReadPowerValuesFromXml(context);
    }
    return NOERROR;
}

ECode CPowerProfile::ReadPowerValuesFromXml(
    /* [in] */ IContext* context)
{
    Int32 id = R::xml::power_profile;
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr<IXmlResourceParser> _parser;
    resources->GetXml(id, (IXmlResourceParser**)&_parser);
    AutoPtr<IXmlPullParser> parser = IXmlPullParser::Probe(_parser);
    Boolean parsingArray = FALSE;
    List<AutoPtr<IDouble> > array;
    String arrayName;

    ECode ec = NOERROR;
    do {
        ec = XmlUtils::BeginDocument(parser, TAG_DEVICE);
        if (FAILED(ec))
            break;

        while (TRUE) {
            ec = XmlUtils::NextElement(parser);
            if (FAILED(ec))
                break;

            String element;
            ec = parser->GetName(&element);
            if (FAILED(ec))
                break;
            if (element == NULL)
                break;

            if (parsingArray && !element.Equals(TAG_ARRAYITEM)) {
                // Finish array
                AutoPtr<IArrayOf> arrayof;
                CArrayOf::New(EIID_IDouble, array.GetSize(), (IArrayOf**)&arrayof);
                List<AutoPtr<IDouble> >::Iterator iter = array.Begin();
                for (Int32 i = 0; iter != array.End(); ++iter, i++)
                    arrayof->Set(i, TO_IINTERFACE(*iter));

                sPowerMap[arrayName] = TO_IINTERFACE(arrayof);
                parsingArray = FALSE;
            }

            if (element.Equals(TAG_ARRAY)) {
                parsingArray = TRUE;
                array.Clear();
                ec = parser->GetAttributeValue(String(NULL), ATTR_NAME, &arrayName);
                if (FAILED(ec))
                    break;
            }
            else if (element.Equals(TAG_ITEM) || element.Equals(TAG_ARRAYITEM)) {
                String name;
                if (!parsingArray) {
                    ec = parser->GetAttributeValue(String(NULL), ATTR_NAME, &name);
                    if (FAILED(ec))
                        break;
                }

                Int32 next;
                ec = parser->Next(&next);
                if (FAILED(ec))
                    break;
                if (next == IXmlPullParser::TEXT) {
                    String power;
                    ec = parser->GetText(&power);
                    if (FAILED(ec))
                        break;
                    Double value = StringUtils::ParseDouble(power);
                    AutoPtr<IDouble> ivalue;
                    CDouble::New(value, (IDouble**)&ivalue);
                    if (element.Equals(TAG_ITEM)) {
                        sPowerMap[name] = TO_IINTERFACE(ivalue);
                    }
                    else if (parsingArray) {
                        array.PushBack(ivalue);
                    }
                }
            }
        }
        if (FAILED(ec))
            break;

        if (parsingArray) {
            AutoPtr<IArrayOf> arrayof;
            CArrayOf::New(EIID_IDouble, array.GetSize(), (IArrayOf**)&arrayof);
            List<AutoPtr<IDouble> >::Iterator iter = array.Begin();
            for (Int32 i = 0; iter != array.End(); ++iter, i++)
                arrayof->Set(i, TO_IINTERFACE(*iter));

            sPowerMap[arrayName] = TO_IINTERFACE(arrayof);
        }
    } while (0);

    _parser->Close();

    if (ec == (ECode)E_XML_PULL_PARSER_EXCEPTION ||
        ec == (ECode)E_IO_EXCEPTION) {
        return E_RUNTIME_EXCEPTION;
    }

    return ec;
}

ECode CPowerProfile::GetAveragePower(
    /* [in] */ const String& type,
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    HashMap<String, AutoPtr<IInterface> >::Iterator find = sPowerMap.Find(type);
    if (find != sPowerMap.End()) {
        AutoPtr<IInterface> data = find->mSecond;
        AutoPtr<IInterface> value;
        if (IArrayOf::Probe(data)) {
            IArrayOf::Probe(data)->Get(0, (IInterface**)&value);
        }
        else {
            value = data;
        }
        return IDouble::Probe(value)->GetValue(result);
    }
    else {
        *result = 0;
    }

    return NOERROR;
}

ECode CPowerProfile::GetAveragePower(
    /* [in] */ const String& type,
    /* [in] */ Int32 level,
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    HashMap<String, AutoPtr<IInterface> >::Iterator find = sPowerMap.Find(type);
    if (find != sPowerMap.End()) {
        AutoPtr<IInterface> data = find->mSecond;
        AutoPtr<IInterface> value;
        if (IArrayOf::Probe(data)) {
            AutoPtr<IArrayOf> values = IArrayOf::Probe(data);
            Int32 length;
            values->GetLength(&length);
            if (length > level && level >= 0) {
                AutoPtr<IInterface> value;
                values->Get(level, (IInterface**)&value);
                return IDouble::Probe(value)->GetValue(result);
            }
            else if (level < 0) {
                *result = 0;
            }
            else {
                AutoPtr<IInterface> value;
                values->Get(length - 1, (IInterface**)&value);
                return IDouble::Probe(value)->GetValue(result);
            }
        }
        else {
            return IDouble::Probe(data)->GetValue(result);
        }
    }
    else {
        *result = 0;
    }

    return NOERROR;
}

ECode CPowerProfile::GetBatteryCapacity(
    /* [out] */ Double* result)
{
    return GetAveragePower(POWER_BATTERY_CAPACITY, result);
}

ECode CPowerProfile::GetNumSpeedSteps(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    HashMap<String, AutoPtr<IInterface> >::Iterator find = sPowerMap.Find(POWER_CPU_SPEEDS);
    if (find != sPowerMap.End()) {
        AutoPtr<IInterface> value = find->mSecond;
        if (IArrayOf::Probe(value))
            return IArrayOf::Probe(value)->GetLength(result);
    }
    *result = 1; // Only one speed
    return NOERROR;
}

} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos
