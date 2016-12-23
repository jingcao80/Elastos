#include "MyAboutData.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/CoreUtils.h>
#include <org/alljoyn/bus/Version.h>

using Org::Alljoyn::Bus::IVariant;
using Org::Alljoyn::Bus::CVariant;
using Org::Alljoyn::Bus::Version;
using Org::Alljoyn::Bus::EIID_IAboutDataListener;
using Elastos::Core::CoreUtils;
using Elastos::Core::ICharSequence;
using Elastos::Core::EIID_ICharSequence;
using Elastos::Core::IByte;
using Elastos::Core::EIID_IByte;
using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Utility::CHashMap;

namespace Elastos {
namespace DevSamples {
namespace DFObserver {

CAR_INTERFACE_IMPL(MyAboutData, Object, IAboutDataListener)

static AutoPtr<IVariant> CreateStringVariant(
    /* [in] */ const char* str)
{
    AutoPtr<ICharSequence> csq = CoreUtils::Convert(str);
    AutoPtr<IVariant> result;
    CVariant::New(csq, String("s"), (IVariant**)&result);
    return result;
}

static AutoPtr<IVariant> CreateAppIdVariant()
{
    AutoPtr<IArrayOf> array;
    CArrayOf::New(EIID_IByte, 16, (IArrayOf**)&array);
    for (Int32 i = 0; i < 16; ++i) {
        array->Set(i, CoreUtils::ConvertByte(i + 1));
    }
    AutoPtr<IVariant> result;
    CVariant::New(array, String("ay"),(IVariant**)&result);
    return result;
}

static AutoPtr<IVariant> CreateSupportedLanguagesVariant()
{
    AutoPtr<IArrayOf> array;
    CArrayOf::New(EIID_ICharSequence, 1, (IArrayOf**)&array);
    array->Set(0, CoreUtils::Convert("en"));

    AutoPtr<IVariant> result;
    CVariant::New(array, String("as"), (IVariant**)&result);
    return result;
}

ECode MyAboutData::GetAboutData(
    /* [in] */ const String& language,
    /* [out] */ IMap** data)
{
    VALIDATE_NOT_NULL(data)

    AutoPtr<IMap> aboutData;
    CHashMap::New((IMap**)&aboutData);  //HashMap<String, Variant>();

    // nonlocalized values
    aboutData->Put(CoreUtils::Convert("AppId"), CreateAppIdVariant());
    aboutData->Put(CoreUtils::Convert("DefaultLanguage"), CreateStringVariant("en"));
    aboutData->Put(CoreUtils::Convert("DeviceId"), CreateStringVariant("93c06771-c725-48c2-b1ff-6a2a59d445b8"));
    aboutData->Put(CoreUtils::Convert("ModelNumber"), CreateStringVariant("A1B2C3"));
    aboutData->Put(CoreUtils::Convert("SupportedLanguages"), CreateSupportedLanguagesVariant());
    aboutData->Put(CoreUtils::Convert("DateOfManufacture"), CreateStringVariant("2014-09-23"));
    aboutData->Put(CoreUtils::Convert("SoftwareVersion"), CreateStringVariant("1.0"));
    aboutData->Put(CoreUtils::Convert("AJSoftwareVersion"), CreateStringVariant(Version::Get().string()));
    aboutData->Put(CoreUtils::Convert("HardwareVersion"),CreateStringVariant("0.1alpha"));
    aboutData->Put(CoreUtils::Convert("SupportUrl"), CreateStringVariant("http://www.example.com/support"));
    aboutData->Put(CoreUtils::Convert("DeviceName"), CreateStringVariant("A device name"));
    aboutData->Put(CoreUtils::Convert("AppName"), CreateStringVariant("An application name"));
    aboutData->Put(CoreUtils::Convert("Manufacturer"), CreateStringVariant("A mighty manufacturing company"));
    aboutData->Put(CoreUtils::Convert("Description"),
        CreateStringVariant("Sample showing the about feature in a service application"));

    *data = aboutData;
    REFCOUNT_ADD(*data)
    return NOERROR;
}

ECode MyAboutData::GetAnnouncedAboutData(
    /* [out] */ IMap** data)
{
    VALIDATE_NOT_NULL(data)

    AutoPtr<IMap> aboutData;
    CHashMap::New((IMap**)&aboutData);  //HashMap<String, Variant>();

    aboutData->Put(CoreUtils::Convert("AppId"), CreateAppIdVariant());
    aboutData->Put(CoreUtils::Convert("DefaultLanguage"), CreateStringVariant("en"));
    aboutData->Put(CoreUtils::Convert("DeviceName"), CreateStringVariant("A device name"));
    aboutData->Put(CoreUtils::Convert("DeviceId"), CreateStringVariant("93c06771-c725-48c2-b1ff-6a2a59d445b8"));
    aboutData->Put(CoreUtils::Convert("AppName"), CreateStringVariant("An application name"));
    aboutData->Put(CoreUtils::Convert("Manufacturer"), CreateStringVariant("A mighty manufacturing company"));
    aboutData->Put(CoreUtils::Convert("ModelNumber"), CreateStringVariant("A1B2C3"));

    *data = aboutData;
    REFCOUNT_ADD(*data)
    return NOERROR;
}



} // namespace DFObserver
} // namespace DevSamples
} // namespace Elastos