#include "CMonkeySourceNetworkVars.h"
#include <elastos/core/StringUtils.h>
#include "elastos/droid/os/Build.h"
#include "CMonkeyCommandReturn.h"
#include <elastos/core/StringBuffer.h>
#include "MonkeySourceNetwork.h"
#include "Monkey.h"
#include "elastos/droid/os/SystemClock.h"

using Elastos::Core::StringUtils;
using Elastos::Core::StringBuffer;
using Elastos::Core::ISystem;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::Hardware::Display::IDisplayManagerGlobal;
using Elastos::Droid::Hardware::Display::CDisplayManagerGlobalHelper;
using Elastos::Droid::Hardware::Display::IDisplayManagerGlobalHelper;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Utility::CDisplayMetrics;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::SystemClock;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

AutoPtr<HashMap<String, AutoPtr<CMonkeySourceNetworkVars::VarGetter> > > CMonkeySourceNetworkVars::VAR_MAP;

AutoPtr<HashMap<String, AutoPtr<CMonkeySourceNetworkVars::VarGetter> > > CMonkeySourceNetworkVars::InitStatics()
{
    AutoPtr<HashMap<String, AutoPtr<CMonkeySourceNetworkVars::VarGetter> > > map =
        new HashMap<String, AutoPtr<CMonkeySourceNetworkVars::VarGetter> >;
    (*map)[String("build.board")] = new StaticVarGetter(Build::BOARD);
    (*map)[String("build.brand")] = new StaticVarGetter(Build::BRAND);
    (*map)[String("build.device")] = new StaticVarGetter(Build::DEVICE);
    (*map)[String("build.display")] = new StaticVarGetter(Build::DISPLAY);
    (*map)[String("build.fingerprint")] = new StaticVarGetter(Build::FINGERPRINT);
    (*map)[String("build.host")] = new StaticVarGetter(Build::HOST);
    (*map)[String("build.id")] = new StaticVarGetter(Build::ID);
    (*map)[String("build.model")] = new StaticVarGetter(Build::MODEL);
    (*map)[String("build.product")] = new StaticVarGetter(Build::PRODUCT);
    (*map)[String("build.tags")] = new StaticVarGetter(Build::TAGS);
    (*map)[String("build.brand")] = new StaticVarGetter(StringUtils::Int64ToString(Build::TIME));
    (*map)[String("build.type")] = new StaticVarGetter(Build::TYPE);
    (*map)[String("build.user")] = new StaticVarGetter(Build::USER);
    (*map)[String("build.cpu_abi")] = new StaticVarGetter(Build::CPU_ABI);
    (*map)[String("build.manufacturer")] = new StaticVarGetter(Build::MANUFACTURER);
    (*map)[String("build.version.incremental")] = new StaticVarGetter(Build::VERSION::INCREMENTAL);
    (*map)[String("build.version.release")] = new StaticVarGetter(Build::VERSION::RELEASE);
    (*map)[String("build.version.sdk")] = new StaticVarGetter(StringUtils::Int32ToString(Build::VERSION::SDK_INT));
    (*map)[String("build.version.codename")] = new StaticVarGetter(Build::VERSION::CODENAME);

    AutoPtr<IDisplay> display;
    AutoPtr<IDisplayManagerGlobal> global;
    AutoPtr<IDisplayManagerGlobalHelper> helper;
    CDisplayManagerGlobalHelper::AcquireSingleton((IDisplayManagerGlobalHelper**)&helper);
    helper->GetInstance((IDisplayManagerGlobal**)&global);
    global->GetRealDisplay(IDisplay::DEFAULT_DISPLAY, (IDisplay**)&display);

    Int32 width = 0;
    Int32 height = 0;

    display->GetWidth(&width);
    display->GetHeight(&height);
    (*map)[String("display.width")] = new StaticVarGetter(StringUtils::Int32ToString(width));
    (*map)[String("display.height")] = new StaticVarGetter(StringUtils::Int32ToString(height));

    AutoPtr<IDisplayMetrics> dm = NULL;
    CDisplayMetrics::New((IDisplayMetrics**)&dm);
    display->GetMetrics(dm);
    Float density = 0;
    dm->GetDensity(&density);
    (*map)[String("display.density")] = new StaticVarGetter(StringUtils::FloatToString(density));

    typedef class local1 : public VarGetter
    {
    public:
        String Get()
        {
            return Monkey::currentPackage;
        }
    } local1;
    (*map)[String("am.current.package")] = new local1();

    typedef class local2 : public VarGetter
    {
    public:
        String Get()
        {
            String action;
            if(Monkey::currentIntent)
                Monkey::currentIntent->GetAction(&action);
            return action;
        }
    } local2;
    (*map)[String("am.current.action")] = new local2();

    typedef class local3 : public VarGetter
    {
    public:
        String Get()
        {
            String className;
            if(Monkey::currentIntent)
            {
                AutoPtr<IComponentName> componentName;
                Monkey::currentIntent->GetComponent((IComponentName**)&componentName);
                if(componentName)
                    componentName->GetClassName(&className);
            }
            return className;
        }
    } local3;
    (*map)[String("am.current.comp.class")] = new local3();

    typedef class local4 : public VarGetter
    {
    public:
        String Get()
        {
            String packageName;
            if(Monkey::currentIntent)
            {
                AutoPtr<IComponentName> componentName;
                Monkey::currentIntent->GetComponent((IComponentName**)&componentName);
                if(componentName)
                    componentName->GetPackageName(&packageName);
            }
            return packageName;
        }
    } local4;
    (*map)[String("am.current.comp.package")] = new local4();

    typedef class local5 : public VarGetter
    {
    public:
        String Get()
        {
            String data;
            if(Monkey::currentIntent)
            {
                Monkey::currentIntent->GetDataString(&data);
            }
            return data;
        }
    } local5;
    (*map)[String("am.current.data")] = new local5();

    typedef class local6 : public VarGetter
    {
    public:
        String Get()
        {
            String categoriesString;
            StringBuffer sb;
            if(Monkey::currentIntent)
            {
                AutoPtr<ArrayOf<String> > categories;
                Monkey::currentIntent->GetCategories((ArrayOf<String>**)&categories);
                Int32 length = categories->GetLength();
                for(Int32 i = 0; i < length; ++i)
                {
                    sb.Append((*categories)[i]);
                    sb.Append(String(" "));
                }
                sb.ToString(&categoriesString);
            }
            return categoriesString;
        }
    } local6;
    (*map)[String("am.current.categories")] = new local6();

    // clock
    typedef class local7 : public VarGetter
    {
    public:
        String Get()
        {
            return StringUtils::Int64ToString(SystemClock::GetElapsedRealtime());
        }
    } local7;
    (*map)[String("clock.realtime")] = new local7();

    typedef class local8 : public VarGetter
    {
    public:
        String Get()
        {
            return StringUtils::Int64ToString(SystemClock::GetUptimeMillis());
        }
    } local8;
    (*map)[String("clock.uptime")] = new local8();

    typedef class local9 : public VarGetter
    {
    public:
        String Get()
        {
            AutoPtr<ISystem> system;
            Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
            Int64 now;
            system->GetCurrentTimeMillis(&now);
            return StringUtils::Int64ToString(now);
        }
    } local9;
    (*map)[String("clock.millis")] = new local9();

    typedef class local10 : public VarGetter
    {
    public:
        String Get()
        {
            return StringUtils::Int32ToString(IMonkeySourceNetwork::MONKEY_NETWORK_VERSION);
        }
    } local10;
    (*map)[String("monkey.version")] = new local10();

    return map;
}

CAR_INTERFACE_IMPL(CMonkeySourceNetworkVars::ListVarCommand, IMonkeyCommand)

ECode CMonkeySourceNetworkVars::ListVarCommand::TranslateCommand(
    /* [in] */ ArrayOf<String> *command,
    /* [in] */ ICommandQueue *queue,
    /* [out] */ IMonkeyCommandReturn **ret)
{
    VALIDATE_NOT_NULL(ret);

    if (VAR_MAP == NULL) {
        VAR_MAP = CMonkeySourceNetworkVars::InitStatics();
    }

    StringBuffer sb;
    HashMap<String, AutoPtr<VarGetter> >::Iterator it = VAR_MAP->Begin();
    for(; it != VAR_MAP->End(); ++it)
    {
        sb.Append(it->mFirst);
        sb.Append(String(" "));
    }
    String word;
    sb.Substring(0, &word);
    CMonkeyCommandReturn::New(TRUE, word, ret);
    return NOERROR;
}

CAR_INTERFACE_IMPL(CMonkeySourceNetworkVars::GetVarCommand, IMonkeyCommand)

ECode CMonkeySourceNetworkVars::GetVarCommand::TranslateCommand(
            /* [in] */ ArrayOf<String> *command,
            /* [in] */ ICommandQueue *queue,
            /* [out] */ IMonkeyCommandReturn **ret)
{
    if(2 == command->GetLength())
    {
        AutoPtr<VarGetter> getter = (*VAR_MAP)[(*command)[1]];
        if(!getter.Get())
        {
            CMonkeyCommandReturn::New(FALSE, String("unknown var"), ret);
            return NOERROR;
        }
        CMonkeyCommandReturn::New(TRUE, getter->Get(), ret);
        return NOERROR;
    }
    *ret = MonkeySourceNetwork::EARG;
    REFCOUNT_ADD(*ret)
    return NOERROR;
}

CMonkeySourceNetworkVars::StaticVarGetter::StaticVarGetter(
                /* [in] */ const String& value)
    : mValue(value)
{
}

String CMonkeySourceNetworkVars::StaticVarGetter::Get()
{
    return mValue;
}

CAR_OBJECT_IMPL(CMonkeySourceNetworkVars)

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos
