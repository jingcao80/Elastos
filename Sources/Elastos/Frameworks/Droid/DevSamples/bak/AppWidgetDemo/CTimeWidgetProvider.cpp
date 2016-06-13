
#include "CTimeWidgetProvider.h"

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IComponentName;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace AppWidgetDemo {

CTimeWidgetProvider::CTimeWidgetProvider()
{
printf("File=[%s], Func=[%s], line=[%d] ======================== . \n", __FILE__, __FUNCTION__, __LINE__);
}

ECode CTimeWidgetProvider::OnUpdate(
    /* [in] */ IContext* context,
    /* [in] */ IAppWidgetManager* appWidgetManager,
    /* [in] */ ArrayOf<Int32>* appWidgetIds)
{
    printf("File=[%s], Func=[%s], line=[%d] ======================== . \n", __FILE__, __FUNCTION__, __LINE__);
    return NOERROR;
}

ECode CTimeWidgetProvider::OnDeleted(
    /* [in] */ IContext* context,
    /* [in] */ ArrayOf<Int32>* appWidgetIds)
{
    printf("File=[%s], Func=[%s], line=[%d] ======================== . \n", __FILE__, __FUNCTION__, __LINE__);
    return NOERROR;
}

ECode CTimeWidgetProvider::OnEnabled(
    /* [in] */ IContext* context)
{
    printf("File=[%s], Func=[%s], line=[%d] ======================== . \n", __FILE__, __FUNCTION__, __LINE__);
    assert(context != NULL);
    AutoPtr<IIntent> intent;

    AutoPtr<IModuleInfo> moduleInfo;
    AutoPtr<IClassInfo> classInfo;
    String path = String("/data/data/com.elastos.runtime/elastos/AppWidgetDemo/AppWidgetDemo.eco");
    CReflector::AcquireModuleInfo(path, (IModuleInfo**)&moduleInfo);
    assert(moduleInfo != NULL);
    moduleInfo->GetClassInfo(String("CTimeService"), (IClassInfo**)&classInfo);
    assert(classInfo != NULL);
    CIntent::New(context, classInfo, (IIntent**)&intent);
    AutoPtr<IComponentName> cName;
    return context->StartService(intent, (IComponentName**)&cName);
}

ECode CTimeWidgetProvider::OnDisabled(
    /* [in] */ IContext* context)
{
    printf("File=[%s], Func=[%s], line=[%d] ======================== . \n", __FILE__, __FUNCTION__, __LINE__);
    assert(context != NULL);
    AutoPtr<IIntent> intent;
    AutoPtr<IModuleInfo> moduleInfo;
    AutoPtr<IClassInfo> classInfo;
    String path = String("/data/data/com.elastos.runtime/elastos/AppWidgetDemo/AppWidgetDemo.eco");
printf("File=[%s], Func=[%s], line=[%d],path=[%s] . \n", __FILE__, __FUNCTION__, __LINE__, path.string());
    ECode  ec = CReflector::AcquireModuleInfo(path, (IModuleInfo**)&moduleInfo);
    printf("File=[%s], Func=[%s], line=[%d], ec=[0x%08x] . \n", __FILE__, __FUNCTION__, __LINE__, ec);
    assert(moduleInfo != NULL);
    ec = moduleInfo->GetClassInfo(String("CTimeService"), (IClassInfo**)&classInfo);
printf("File=[%s], Func=[%s], line=[%d], ec=[0x%08x] . \n", __FILE__, __FUNCTION__, __LINE__, ec);
    ec = CIntent::New(context, classInfo, (IIntent**)&intent);
printf("File=[%s], Func=[%s], line=[%d], ec=[0x%08x] . \n", __FILE__, __FUNCTION__, __LINE__, ec);
printf("File=[%s], Func=[%s], line=[%d], intent=[0x%08x] . \n", __FILE__, __FUNCTION__, __LINE__, intent.Get());
    Boolean succeed = FALSE;
    return context->StopService(intent, &succeed);
}


} // namespace AppWidgetDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
