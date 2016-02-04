
#include "CAm.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include "elastos/droid/app/ActivityManagerNative.h"
#include "CIntentReceiver.h"
#include <unistd.h>
#include <elastos/utility/logging/Logger.h>
#include <CInstrumentationWatcher.h>
#include <CMyActivityController.h>
#include <elastos/utility/etl/List.h>
#include <elastos/StringToIntegral.h>

#define FAILED_AND_RETURN(ec) \
do { \
    if (FAILED(ec)) { \
        return ec; \
    } \
} while(0);

using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Etl::List;
using Elastos::Core::StringToIntegral;
using Elastos::Droid::App::CActivityManagerWaitResult;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::IActivityManagerWaitResult;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IInstrumentationWatcher;
using Elastos::Droid::App::IIActivityController;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::EIID_IIntentReceiver;
using Elastos::Droid::Content::IComponentNameHelper;
using Elastos::Droid::Content::CComponentNameHelper;
using Elastos::Droid::Content::IIntentHelper;
using Elastos::Droid::Content::CIntentHelper;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::View::IIWindowManager;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Os::IParcelFileDescriptorHelper;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::CParcelFileDescriptorHelper;
using Elastos::IO::CFile;
using Elastos::IO::IFile;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Am {

// These are magic strings understood by the Eclipse plugin.
static const String FATAL_ERROR_CODE("Error type 1");
static const String NO_SYSTEM_ERROR_CODE("Error type 2");
static const String NO_CLASS_ERROR_CODE("Error type 3");

static const String TAG("CommandAm");

CAm::CAm()
    : mAm()
    , mArgs()
    , mNextArg(0)
    , mCurArgData()
    , mStartFlags(0)
    , mWaitOption(FALSE)
    , mStopOption(FALSE)
    , mRepeat(0)
    , mUserId(0)
    , mProfileFile()
{
}

ECode CAm::Main(
    /* [in] */ const ArrayOf<String>& args)
{
    ECode ec = this->Run(&const_cast<ArrayOf<String>&>(args));
    if (FAILED(ec)) {
        ShowUsage();
    }
    return ec;
}

ECode CAm::Run(
    /* [in] */ ArrayOf<String>* args)
{
    mAm = ActivityManagerNative::GetDefault();
    if (mAm == NULL) {
        PFL_EX("%s", NO_SYSTEM_ERROR_CODE.string());
        return E_REMOTE_SERVICE_EXCEPTION;
    }

    mArgs = args;
    String op = (*args)[0];
    mNextArg = 1;

    if (op.Equals("start")) {
        RunStart();
    } else if (op.Equals("startservice")) {
        RunStartService();
    } else if (op.Equals("force-stop")) {
        RunForceStop();
    } else if (op.Equals("kill")) {
        RunKill();
    } else if (op.Equals("kill-all")) {
        RunKillAll();
    } else if (op.Equals("instrument")) {
        RunInstrument();
    } else if (op.Equals("broadcast")) {
        SendBroadcast();
    } else if (op.Equals("profile")) {
        RunProfile();
    } else if (op.Equals("dumpheap")) {
        RunDumpHeap();
    } else if (op.Equals("set-debug-app")) {
        RunSetDebugApp();
    } else if (op.Equals("clear-debug-app")) {
        RunClearDebugApp();
    } else if (op.Equals("bug-report")) {
        RunBugReport();
    } else if (op.Equals("monitor")) {
        RunMonitor();
    } else if (op.Equals("screen-compat")) {
        RunScreenCompat();
    } else if (op.Equals("display-size")) {
        RunDisplaySize();
    } else if (op.Equals("display-density")) {
        RunDisplayDensity();
    } else if (op.Equals("to-uri")) {
        RunToUri(FALSE);
    } else if (op.Equals("to-intent-uri")) {
        RunToUri(TRUE);
    } else if (op.Equals("switch-user")) {
        RunSwitchUser();
    } else if (op.Equals("stop-user")) {
        RunStopUser();
    } else if (op.Equals("help")) {
        ShowUsage();
    } else {
        PFL_EX("Unknown command: %s", op.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

Int32 CAm::ParseUserArg(
        /* [in] */ const String& arg)
{
    Int32 userId;
    if (arg.Equals("all")) {
        userId = IUserHandle::USER_ALL;
    } else if (arg.Equals("current") || arg.Equals("cur")) {
        userId = IUserHandle::USER_CURRENT;
    } else {
        StringUtils::ParseInt32(arg, &userId);
    }
    return userId;
}

ECode CAm::MakeIntent(
    /* [in] */ Int32 defUser,
    /* [out] */ IIntent** returnedIntent)
{
    VALIDATE_NOT_NULL(returnedIntent)
    AutoPtr<IIntent> intent, baseIntent;
    CIntent::New((IIntent**)&intent);
    baseIntent = intent;
    Boolean hasIntentInfo = FALSE;

    AutoPtr<IUriHelper> urlHelper;
    CUriHelper::AcquireSingleton((IUriHelper**)&urlHelper);
    AutoPtr<IComponentNameHelper> helper;
    CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&helper);

    mStartFlags = 0;
    mWaitOption = FALSE;
    mStopOption = FALSE;
    mRepeat = 0;
    mProfileFile = String();
    mUserId = defUser;
    AutoPtr<IUri> data;
    String type = String();
    String opt;
    while (!(NextOption(&opt), opt).IsNull()) {
        if (opt.Equals("-a")) {
            String nextArgRequired;
            NextArgRequired(&nextArgRequired);
            intent->SetAction(nextArgRequired);
            if (intent == baseIntent) {
                hasIntentInfo = TRUE;
            }
        }
        else if (opt.Equals("-d")) {
            String nextArgRequired;
            NextArgRequired(&nextArgRequired);
            data = NULL;
            urlHelper->Parse(nextArgRequired, (IUri**)&data);
            if (intent == baseIntent) {
                hasIntentInfo = TRUE;
            }
        }
        else if (opt.Equals("-t")) {
            NextArgRequired(&type);
            if (intent == baseIntent) {
                hasIntentInfo = TRUE;
            }
        }
        else if (opt.Equals("-c")) {
            String nextArgRequired;
            NextArgRequired(&nextArgRequired);
            intent->AddCategory(nextArgRequired);
            if (intent == baseIntent) {
                hasIntentInfo = TRUE;
            }
        }
        else if (opt.Equals("-e") || opt.Equals("--es")) {
            String key, value;
            NextArgRequired(&key);
            NextArgRequired(&value);
            intent->PutExtra(key, value);
        }
        else if (opt.Equals("--esn")) {
            String key;
            NextArgRequired(&key);
            intent->PutExtra(key, String());
        }
        else if (opt.Equals("--ei")) {
            String key, value;
            NextArgRequired(&key);
            NextArgRequired(&value);
            intent->PutExtra(key, StringUtils::ParseInt32(value));
        }
        else if (opt.Equals("--eu")) {
            String key, value;
            NextArgRequired(&key);
            NextArgRequired(&value);

            AutoPtr<IUri> uri;
            urlHelper->Parse(value, (IUri**)&uri);
            intent->PutExtra(key, IParcelable::Probe(uri));
        }
        else if (opt.Equals("--ecn")) {
            String key, value;
            NextArgRequired(&key);
            NextArgRequired(&value);
            AutoPtr<IComponentName> cn;
            helper->UnflattenFromString(value, (IComponentName**)&cn);
            if (cn == NULL) {
                Logger::E(String("COMMAND_AM"), "Bad component name: %s", value.string());
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            intent->PutExtra(key, IParcelable::Probe(cn));
        }
        else if (opt.Equals("--eia")) {
            String key, value;
            NextArgRequired(&key);
            NextArgRequired(&value);
            AutoPtr<StringTokenizer> tok = new StringTokenizer(value.string(), ",");
            List<String> strings;
            while (tok->HasMoreTokens()) {
                strings.PushBack(String((const char*)tok->NextToken()));
            }
            Int32 count = strings.GetSize();
            AutoPtr<ArrayOf<Int32> > list = ArrayOf<Int32>::Alloc(count);
            for (Int32 i = 0; i < count; ++i) {
                (*list)[i] = StringUtils::ParseInt32(strings[i]);
            }
            intent->PutExtra(key, list);
        }
        else if (opt.Equals("--el")) {
            String key, value;
            NextArgRequired(&key);
            NextArgRequired(&value);
            intent->PutExtra(key, StringUtils::ParseInt64(value));
        }
        else if (opt.Equals("--ela")) {
            String key, value;
            NextArgRequired(&key);
            NextArgRequired(&value);
            AutoPtr<StringTokenizer> tok = new StringTokenizer(value.string(), ",");
            List<String> strings;
            while (tok->HasMoreTokens()) {
                strings.PushBack(String((const char*)tok->NextToken()));
            }
            Int32 count = strings.GetSize();
            AutoPtr<ArrayOf<Int64> > list = ArrayOf<Int64>::Alloc(count);
            for (Int32 i = 0; i < count; ++i) {
                (*list)[i] = StringUtils::ParseInt64(strings[i]);
            }
            intent->PutExtra(key, list);
            hasIntentInfo = TRUE;
        }
        else if (opt.Equals("--ef")) {
            String key, value;
            NextArgRequired(&key);
            NextArgRequired(&value);
            intent->PutExtra(key, StringUtils::ParseFloat(value));
            hasIntentInfo = TRUE;
        }
        else if (opt.Equals("--efa")) {
            String key, value;
            NextArgRequired(&key);
            NextArgRequired(&value);
            AutoPtr<StringTokenizer> tok = new StringTokenizer(value.string(), ",");
            List<String> strings;
            while (tok->HasMoreTokens()) {
                strings.PushBack(String((const char*)tok->NextToken()));
            }
            Int32 count = strings.GetSize();
            AutoPtr<ArrayOf<Float> > list = ArrayOf<Float>::Alloc(count);
            for (Int32 i = 0; i < count; ++i) {
                (*list)[i] = StringUtils::ParseFloat(strings[i]);
            }
            intent->PutExtra(key, list);
            hasIntentInfo = TRUE;
        }
        else if (opt.Equals("--ez")) {
            String key, value;
            NextArgRequired(&key);
            NextArgRequired(&value);
            intent->PutBooleanExtra(key, value.EqualsIgnoreCase("true"));
        }
        else if (opt.Equals("-n")) {
            String str;
            NextArgRequired(&str);
            AutoPtr<IComponentName> cn;
            helper->UnflattenFromString(str, (IComponentName**)&cn);
            if (cn == NULL) {
                Logger::E(String("COMMAND_AM"), "Bad component name: %s", str.string());
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            intent->SetComponent(cn);
            if (intent == baseIntent) {
                hasIntentInfo = TRUE;
            }
        }
        else if (opt.Equals("-f")) {
            String str;
            NextArgRequired(&str);
            Int32 flag;
            intent->SetFlags((StringToIntegral::Decode(str, &flag), flag));
        }
        else if (opt.Equals("--grant-read-uri-permission")) {
            intent->AddFlags(IIntent::FLAG_GRANT_READ_URI_PERMISSION);
        }
        else if (opt.Equals("--grant-write-uri-permission")) {
            intent->AddFlags(IIntent::FLAG_GRANT_WRITE_URI_PERMISSION);
        }
        else if (opt.Equals("--exclude-stopped-packages")) {
            intent->AddFlags(IIntent::FLAG_EXCLUDE_STOPPED_PACKAGES);
        }
        else if (opt.Equals("--include-stopped-packages")) {
            intent->AddFlags(IIntent::FLAG_INCLUDE_STOPPED_PACKAGES);
        }
        else if (opt.Equals("--debug-log-resolution")) {
            intent->AddFlags(IIntent::FLAG_DEBUG_LOG_RESOLUTION);
        }
        else if (opt.Equals("--activity-brought-to-front")) {
            intent->AddFlags(IIntent::FLAG_ACTIVITY_BROUGHT_TO_FRONT);
        }
        else if (opt.Equals("--activity-clear-top")) {
            intent->AddFlags(IIntent::FLAG_ACTIVITY_CLEAR_TOP);
        }
        else if (opt.Equals("--activity-clear-when-task-reset")) {
            intent->AddFlags(IIntent::FLAG_ACTIVITY_CLEAR_WHEN_TASK_RESET);
        }
        else if (opt.Equals("--activity-exclude-from-recents")) {
            intent->AddFlags(IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);
        }
        else if (opt.Equals("--activity-launched-from-history")) {
            intent->AddFlags(IIntent::FLAG_ACTIVITY_LAUNCHED_FROM_HISTORY);
        }
        else if (opt.Equals("--activity-multiple-task")) {
            intent->AddFlags(IIntent::FLAG_ACTIVITY_MULTIPLE_TASK);
        }
        else if (opt.Equals("--activity-no-animation")) {
            intent->AddFlags(IIntent::FLAG_ACTIVITY_NO_ANIMATION);
        }
        else if (opt.Equals("--activity-no-history")) {
            intent->AddFlags(IIntent::FLAG_ACTIVITY_NO_HISTORY);
        }
        else if (opt.Equals("--activity-no-user-action")) {
            intent->AddFlags(IIntent::FLAG_ACTIVITY_NO_USER_ACTION);
        }
        else if (opt.Equals("--activity-previous-is-top")) {
            intent->AddFlags(IIntent::FLAG_ACTIVITY_PREVIOUS_IS_TOP);
        }
        else if (opt.Equals("--activity-reorder-to-front")) {
            intent->AddFlags(IIntent::FLAG_ACTIVITY_REORDER_TO_FRONT);
        }
        else if (opt.Equals("--activity-reset-task-if-needed")) {
            intent->AddFlags(IIntent::FLAG_ACTIVITY_RESET_TASK_IF_NEEDED);
        }
        else if (opt.Equals("--activity-single-top")) {
            intent->AddFlags(IIntent::FLAG_ACTIVITY_SINGLE_TOP);
        }
        else if (opt.Equals("--activity-clear-task")) {
            intent->AddFlags(IIntent::FLAG_ACTIVITY_CLEAR_TASK);
        }
        else if (opt.Equals("--activity-task-on-home")) {
            intent->AddFlags(IIntent::FLAG_ACTIVITY_TASK_ON_HOME);
        }
        else if (opt.Equals("--receiver-registered-only")) {
            intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY);
        }
        else if (opt.Equals("--receiver-replace-pending")) {
            intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING);
        }
        else if (opt.Equals("--selector")) {
            intent->SetDataAndType(data, type);
            intent = NULL;
            CIntent::New((IIntent**)&intent);
        }
        else if (opt.Equals("-D")) {
            mStartFlags |= IActivityManager::START_FLAG_DEBUG;
        }
        else if (opt.Equals("-W")) {
            mWaitOption = TRUE;
        }
        else if (opt.Equals("-P")) {
            NextArgRequired(&mProfileFile);
            mStartFlags |= IActivityManager::START_FLAG_AUTO_STOP_PROFILER;
        }
        else if (opt.Equals("--start-profiler")) {
            NextArgRequired(&mProfileFile);
            mStartFlags &= ~IActivityManager::START_FLAG_AUTO_STOP_PROFILER;
        }
        else if (opt.Equals("-R")) {
            String nextArgRequired;
            NextArgRequired(&nextArgRequired);
            NextArgRequired(&nextArgRequired);
            mRepeat = StringUtils::ParseInt32(nextArgRequired);
        }
        else if (opt.Equals("-S")) {
            mStopOption = TRUE;
        }
        else if (opt.Equals("--opengl-trace")) {
            mStartFlags |= IActivityManager::START_FLAG_OPENGL_TRACES;
        }
        else if (opt.Equals("--user")) {
            String nextArgRequired;
            NextArgRequired(&nextArgRequired);
            mUserId = ParseUserArg(nextArgRequired);
        }
        else {
            PFL_EX("Error: Unknown option: %s", opt.string());
            *returnedIntent = NULL;
            return NOERROR;
        }
    }
    intent->SetDataAndType(data, type);

    const Boolean hasSelector = intent != baseIntent;
    if (hasSelector) {
        // A selector was specified; fix up.
        baseIntent->SetSelector(intent);
        intent = baseIntent;
    }

    String arg = NextArg();
    baseIntent = NULL;
    if (arg.IsNull()) {
        if (hasSelector) {
            // If a selector has been specified, and no arguments
            // have been supplied for the main Intent, then we can
            // assume it is ACTION_MAIN CATEGORY_LAUNCHER; we don't
            // need to have a component name specified yet, the
            // selector will take care of that.
            CIntent::New(IIntent::ACTION_MAIN, (IIntent**)&baseIntent);
            baseIntent->AddCategory(IIntent::CATEGORY_LAUNCHER);
        }
    }
    else if (arg.IndexOf(':') >= 0) {
        // The argument is a URI.  Fully parse it, and use that result
        // to fill in any data not specified so far.
        AutoPtr<IIntentHelper> helper;
        CIntentHelper::AcquireSingleton((IIntentHelper**)&helper);
        helper->ParseUri(arg, IIntent::URI_INTENT_SCHEME, (IIntent**)&baseIntent);
    }
    else if (arg.IndexOf('/') >= 0) {
        // The argument is a component name.  Build an Intent to launch
        // it.
        CIntent::New(IIntent::ACTION_MAIN, (IIntent**)&baseIntent);
        baseIntent->AddCategory(IIntent::CATEGORY_LAUNCHER);
        AutoPtr<IComponentName> cn;
        helper->UnflattenFromString(arg, (IComponentName**)&cn);
        baseIntent->SetComponent(cn);
    }
    else {
        // Assume the argument is a package name.
        CIntent::New(IIntent::ACTION_MAIN, (IIntent**)&baseIntent);
        baseIntent->AddCategory(IIntent::CATEGORY_LAUNCHER);
        baseIntent->SetPackage(arg);
    }

    if (baseIntent != NULL) {
        AutoPtr<IBundle> extras;
        intent->GetExtras((IBundle**)&extras);
        intent->ReplaceExtras((IBundle*)NULL);
        AutoPtr<IBundle> uriExtras;
        baseIntent->GetExtras((IBundle**)&uriExtras);
        baseIntent->ReplaceExtras((IBundle*)NULL);
        String action;
        AutoPtr<ArrayOf<String> > categories;
        if (!(intent->GetAction(&action), action).IsNull() &&
            (baseIntent->GetCategories((ArrayOf<String>**)&categories), categories) != NULL) {
            Int32 length = categories->GetLength();
            for (Int32 i = 0; i < length; ++i) {
                baseIntent->RemoveCategory((*categories)[i]);
            }
        }
        Int32 result;
        intent->FillIn(baseIntent, IIntent::FILL_IN_COMPONENT | IIntent::FILL_IN_SELECTOR,
            &result);
        if (extras == NULL) {
            extras = uriExtras;
        }
        else if (uriExtras != NULL) {
            uriExtras->PutAll(extras);
            extras = uriExtras;
        }
        intent->ReplaceExtras(extras);
        hasIntentInfo = TRUE;
    }

    if (!hasIntentInfo) {
        Logger::E(String("COMMAND_AM"), "No intent supplied");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    *returnedIntent = intent;
    REFCOUNT_ADD(*returnedIntent);
    return NOERROR;
}

void CAm::RunStartService()
{
    AutoPtr<IIntent> intent;
    MakeIntent(IUserHandle::USER_CURRENT, (IIntent**)&intent);
    if (mUserId == IUserHandle::USER_ALL) {
        PFL_EX("Error: Can't start activity with user 'all'");
        return;
    }

    String intentToString;
    intent->ToString(&intentToString);
    PFL_EX("Starting service: %s", intentToString.string());

    String type;
    intent->GetType(&type);
    AutoPtr<IComponentName> cn;
    mAm->StartService(NULL, intent, type, mUserId, (IComponentName**)&cn);
    if (cn == NULL) {
        PFL_EX("Error: Not found; no service started.");
    }
    else {
        String packageName;
        String className;
        cn->GetPackageName(&packageName);
        cn->GetClassName(&className);
        if(packageName.Equals("!")) {
            PFL_EX("Error: Requires permission %s", className.string());
        }
        else if(packageName.Equals("!!")) {
            PFL_EX("Error: %s", className.string());
        }
    }
}

ECode CAm::RunStart()
{
    AutoPtr<IIntent> intent;
    MakeIntent(IUserHandle::USER_CURRENT, (IIntent**)&intent);

    if (mUserId == IUserHandle::USER_ALL) {
        PFL_EX("Error: Can't start service with user 'all'");
        return NOERROR;
    }

    String mimeType;
    intent->GetType(&mimeType);
    if(mimeType.IsNull()) {
        AutoPtr<IUri> data;
        intent->GetData((IUri**)&data);
        if (data != NULL) {
            String scheme;
            data->GetScheme(&scheme);
            if (scheme.Equals("content")) {
                mAm->GetProviderMimeType(data, mUserId, &mimeType);
            }
        }
    }

    do {
        String intentByString;
        intent->ToString(&intentByString);
        if (mStopOption) {
            String packageName;
            AutoPtr<IComponentName> component;
            intent->GetComponent((IComponentName**)&component);
            if (component != NULL) {
                component->GetPackageName(&packageName);
            }
            else {
                AutoPtr<IIPackageManager> pm;
                AutoPtr<IInterface> base;
                AutoPtr<IServiceManager> sm;
                ASSERT_SUCCEEDED(CServiceManager::AcquireSingleton((IServiceManager**)&sm));
                sm->GetService(String("package"), (IInterface**)&base);
                pm = (IIPackageManager::Probe(base));
                if (pm == NULL) {
                    PFL_EX("Error: Package manager not running; aborting");
                    return NOERROR;
                }

                AutoPtr<IObjectContainer> activities;
                pm->QueryIntentActivities(intent, mimeType, 0,
                    mUserId, (IObjectContainer**)&activities);
                Int32 count(0);
                if (activities == NULL || (activities->GetObjectCount(&count), count) <= 0) {
                    PFL_EX("Error: Intent does not match any activities: %s", intentByString.string());
                    return NOERROR;
                }
                else if (count > 1) {
                    PFL_EX("Error: Intent matches multiple activities; can't stop: ", intentByString.string());
                    return NOERROR;
                }


                AutoPtr<IObjectEnumerator> enumerator;
                activities->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
                Boolean hasNext = FALSE;
                enumerator->MoveNext(&hasNext);
                if (hasNext) {
                    AutoPtr<IInterface> element;
                    enumerator->Current((IInterface**)&element);
                    AutoPtr<IResolveInfo> r = IResolveInfo::Probe(element);
                    AutoPtr<IActivityInfo> act;
                    r->GetActivityInfo((IActivityInfo**)&act);
                    act->GetPackageName(&packageName);
                }
            }
            PFL_EX("Stopping: %s", packageName.string());
            mAm->ForceStopPackage(packageName, mUserId);
            usleep(250 * 1000);
        }
        PFL_EX("Starting: %s", intentByString.string());
        intent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);

        AutoPtr<IParcelFileDescriptor> fd;

        if (!mProfileFile.IsNull()) {
            AutoPtr<IParcelFileDescriptorHelper> parcelHelper;
            FAIL_RETURN(CParcelFileDescriptorHelper::AcquireSingleton((IParcelFileDescriptorHelper**)&parcelHelper));
            AutoPtr<IFile> file;
            CFile::New(mProfileFile, (IFile**)&file);
            ECode ec = parcelHelper->Open(file,
                IParcelFileDescriptor::MODE_CREATE |
                IParcelFileDescriptor::MODE_TRUNCATE |
                IParcelFileDescriptor::MODE_READ_WRITE,
                (IParcelFileDescriptor**)&fd);
            if (ec == (Int32)E_NOT_FOUND_EXCEPTION) {
                PFL_EX("Error: Unable to open file: %s", mProfileFile.string());
                return NOERROR;
            }
        }

        AutoPtr<IActivityManagerWaitResult> result;
        Int32 res;
        if (mWaitOption) {
            mAm->StartActivityAndWait(NULL ,intent, mimeType, NULL,
                String(), 0, mStartFlags, mProfileFile, fd, NULL, mUserId,
                (IActivityManagerWaitResult**)&result);
            result->GetResult(&res);
        }
        else {
            mAm->StartActivityAsUser(NULL ,intent, mimeType,
                NULL, String(), 0, mStartFlags, mProfileFile, fd, NULL, mUserId,
                &res);
        }

        Boolean launched(FALSE);
        switch (res) {
            case IActivityManager::START_SUCCESS: {
                    launched = TRUE;
                    break;
                }
            case IActivityManager::START_SWITCHES_CANCELED: {
                    launched = TRUE;
                    PFL_EX("Warning: Activity not started because the "
                        " current activity is being kept for the user.");
                    break;
                }
            case IActivityManager::START_DELIVERED_TO_TOP: {
                    launched = TRUE;
                    PFL_EX("Warning: Activity not started, intent has "
                        "been delivered to currently running "
                        "top-most instance.");
                    break;
                }
            case IActivityManager::START_RETURN_INTENT_TO_CALLER: {
                    launched = TRUE;
                    PFL_EX("Warning: Activity not started because intent "
                        "should be handled by the caller");
                    break;
                }
            case IActivityManager::START_TASK_TO_FRONT: {
                    launched = TRUE;
                    PFL_EX("Warning: Activity not started, its current "
                        "task has been brought to the front");
                    break;
                }
            case IActivityManager::START_INTENT_NOT_RESOLVED:
                {
                    PFL_EX("Error: Activity not started, unable to "
                        "resolve %s", intentByString.string());
                    break;
                }
            case IActivityManager::START_CLASS_NOT_FOUND: {
                    PFL_EX("%s", NO_CLASS_ERROR_CODE.string());
                    AutoPtr<IComponentName> component;
                    intent->GetComponent((IComponentName**)&component);
                    String name;
                    component->ToShortString(&name);
                    PFL_EX("Error: Activity class %s does not exist.", name.string());
                    break;
                }
            case IActivityManager::START_FORWARD_AND_REQUEST_CONFLICT: {
                    PFL_EX("Error: Activity not started, you requested to "
                        "both forward and receive its result");
                    break;
                }
            case IActivityManager::START_PERMISSION_DENIED: {
                    PFL_EX("Error: Activity not started, you do not "
                        "have permission to access it.");
                    break;
                }
            default:
                PFL_EX("Error: Activity not started, unknown error code %d", res);
                break;
        }

        if (mWaitOption && launched) {
            AutoPtr<IComponentName> who;
            if (result == NULL) {
                CActivityManagerWaitResult::New((IActivityManagerWaitResult**)&result);
                intent->GetComponent((IComponentName**)&who);
                result->SetWho(who);
            }

            Boolean timeout;
            PFL_EX("Status: %s", (result->GetTimeout(&timeout), timeout) ? "timeout" : "ok");
            who = NULL;
            result->GetWho((IComponentName**)&who);
            if (who != NULL) {
                String shortString;
                PFL_EX("Activity: %s", (who->FlattenToShortString(&shortString),shortString).string());
            }

            Int64 thisTime;
            if ((result->GetThisTime(&thisTime), thisTime) >= 0) {
                PFL_EX("ThisTime: %lld", thisTime);
            }

            Int64 totalTime;
            if ((result->GetTotalTime(&totalTime), totalTime) >= 0) {
                PFL_EX("TotalTime: %lld", totalTime);
            }
            PFL_EX("Complete");
        }

        --mRepeat;
        if (mRepeat > 1) {
            mAm->UnhandledBack();
        }
    }
    while (mRepeat > 1);

    return NOERROR;
}

ECode CAm::RunForceStop()
{
    Int32 userId = IUserHandle::USER_ALL;
    String opt;
    ECode ec = NextOption(&opt);
    FAILED_AND_RETURN(ec);
    while (!opt.IsNull()) {
        if (opt.Equals("--user")) {
            String nextArgRequired;
            ec = NextArgRequired(&nextArgRequired);
            FAILED_AND_RETURN(ec);
            userId = ParseUserArg(nextArgRequired);
        }
        else {
            PFL_EX("Error: Unknown option: %s", opt.string());
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        ec = NextOption(&opt);
        FAILED_AND_RETURN(ec);
    }

    String nextArgRequired;
    ec = NextArgRequired(&nextArgRequired);
    FAILED_AND_RETURN(ec);
    return mAm->ForceStopPackage(nextArgRequired, userId);
}

ECode CAm::RunKill()
{
    Int32 userId = IUserHandle::USER_ALL;
    String opt;
    ECode ec = NextOption(&opt);
    FAILED_AND_RETURN(ec);
    while (!opt.IsNull()) {
        if (opt.Equals("--user")) {
            String nextArgRequired;
            ec = NextArgRequired(&nextArgRequired);
            FAILED_AND_RETURN(ec);
            userId = ParseUserArg(nextArgRequired);
        }
        else {
            PFL_EX("Error: Unknown option: %s", opt.string());
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        ec = NextOption(&opt);
        FAILED_AND_RETURN(ec);
    }

    String nextArgRequired;
    ec = NextArgRequired(&nextArgRequired);
    FAILED_AND_RETURN(ec);
    return mAm->KillBackgroundProcesses(nextArgRequired, userId);
}

ECode CAm::RunKillAll()
{
    return mAm->KillAllBackgroundProcesses();
}

ECode CAm::SendBroadcast()
{
    AutoPtr<IIntent> intent;
    MakeIntent(IUserHandle::USER_ALL, (IIntent**)&intent);
    AutoPtr<IIntentReceiver> receiver;
    CIntentReceiver::New((IIntentReceiver**)&receiver);
    String intentToString;
    intent->ToString(&intentToString);
    PFL_EX("Broadcasting: %s", intentToString.string());
    Int32 result;
    String nullStr;
    mAm->BroadcastIntent(NULL, intent, nullStr, receiver,
        0, nullStr, NULL, nullStr, TRUE, FALSE, mUserId, &result);
    ((CIntentReceiver*)receiver.Get())->WaitForFinish();
    return NOERROR;
}

ECode CAm::RunInstrument()
{
    String profileFile;
    Boolean wait(FALSE);
    Boolean rawMode(FALSE);
    Boolean no_window_animation(FALSE);
    Int32 userId(IUserHandle::USER_CURRENT);
    AutoPtr<IBundle> args;
    CBundle::New((IBundle**)&args);
    String argKey, argValue;
    AutoPtr<IServiceManager> sm;
    AutoPtr<IInterface> base;
    ASSERT_SUCCEEDED(CServiceManager::AcquireSingleton((IServiceManager**)&sm));
    sm->GetService(String("window"), (IInterface**)&base);
    AutoPtr<IIWindowManager> wm = IIWindowManager::Probe(base);

    String opt;
    ECode ec = NextOption(&opt);
    FAILED_AND_RETURN(ec);
    while (!opt.IsNull()) {
        if (opt.Equals("-p")) {
            String nextArgRequired;
            ec = NextArgRequired(&nextArgRequired);
            FAILED_AND_RETURN(ec);
            profileFile = nextArgRequired;
        }
        else if (opt.Equals("-w")) {
            wait = TRUE;
        }
        else if (opt.Equals("-r")) {
            rawMode = TRUE;
        }
        else if (opt.Equals("-e")) {
            ec = NextArgRequired(&argKey);
            FAILED_AND_RETURN(ec);
            ec = NextArgRequired(&argValue);
            FAILED_AND_RETURN(ec);
            args->PutString(argKey, argValue);
        }
        else if (opt.Equals("--no_window_animation")
            || opt.Equals("--no-window-animation")) {
            no_window_animation = TRUE;
        }
        else if (opt.Equals("--user")) {
            String nextArgRequired;
            ec = NextArgRequired(&nextArgRequired);
            FAILED_AND_RETURN(ec);
            userId = ParseUserArg(nextArgRequired);
        }
        else {
            PFL_EX("Error: Unknown option: %s", opt.string());
            return NOERROR;
        }

        ec = NextOption(&opt);
        FAILED_AND_RETURN(ec);
    }

    if (userId == IUserHandle::USER_ALL) {
        PFL_EX("Error: Can't start instrumentation with user 'all'");
        return NOERROR;
    }

    String cnArg;
    ec = NextArgRequired(&cnArg);
    FAILED_AND_RETURN(ec);
    AutoPtr<IComponentName> cn;
    AutoPtr<IComponentNameHelper> helper;
    CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&helper);
    helper->UnflattenFromString(cnArg, (IComponentName**)&cn);
    if (cn == NULL) {
        Logger::E(TAG, "Bad component name: %s", cnArg.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IInstrumentationWatcher> watcher;
    if (wait) {
        CInstrumentationWatcher::New((Handle32)this, (IInstrumentationWatcher**)&watcher);
        ((CInstrumentationWatcher *)watcher.Get())->SetRawOutput(rawMode);
    }

    AutoPtr<ArrayOf<Float> > oldAnims;
    if (no_window_animation) {
        wm->GetAnimationScales((ArrayOf<Float>**)&oldAnims);
        wm->SetAnimationScale(0, 0.0f);
        wm->SetAnimationScale(1, 0.0f);
    }

    Boolean result;
    ec = mAm->StartInstrumentation(cn, profileFile, 0, args, watcher, userId, &result);
    if (FAILED(ec) || !result) {
        String cnInString;
        cn->FlattenToString(&cnInString);
        Logger::E(TAG, "INSTRUMENTATION_FAILED: %s", cnInString.string());
        return ec;
    }

    if (watcher != NULL) {
        ec = ((CInstrumentationWatcher *)watcher.Get())->WaitForFinish(&result);
        if (FAILED(ec))
            return ec;

        if (!result) {
            PFL_EX("INSTRUMENTATION_ABORTED: System has crashed.");
        }
    }

    if (oldAnims != NULL) {
        wm->SetAnimationScales(oldAnims);
    }

    return NOERROR;
}

ECode CAm::RemoveWallOption()
{
    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    String props;
    sysProp->Get(String("dalvik.vm.extra-opts"), &props);
    if (!props.IsNull() && props.Contains("-Xprofile:wallclock")) {
        StringBuilder sb;
        String replacee("-Xprofile:wallclock");
        Int32 startIndex = 0, endIndex = props.IndexOf(replacee);
        while(endIndex != -1) {
            sb.Append(props.Substring(startIndex, endIndex));
            startIndex = endIndex + replacee.GetLength();
            endIndex = props.IndexOf(replacee, startIndex);
        }

        sb.Append(props.Substring(startIndex, props.GetLength()));
        sb.ToString(&props);
        props = props.Trim();
        sysProp->Set(String("dalvik.vm.extra-opts"), props);
    }
    return NOERROR;
}

ECode CAm::RunProfile()
{
    String profileFile;
    Boolean start(FALSE);
    Boolean wall(FALSE);
    Int32 userId = IUserHandle::USER_CURRENT;
    Int32 profileType(0);

    String process;

    String cmd;
    ECode ec = NextArgRequired(&cmd);
    FAILED_AND_RETURN(ec);

    if (cmd.Equals("start")) {
        start = TRUE;
        String opt;
        ec = NextOption(&opt);
        FAILED_AND_RETURN(ec);
        while (!opt.IsNull()) {
            if (opt.Equals("--user")) {
                String userArg;
                ec = NextArgRequired(&userArg);
                FAILED_AND_RETURN(ec);
                userId = ParseUserArg(userArg);
            }
            else if (opt.Equals("--wall")) {
                wall = TRUE;
            }
            else {
                PFL_EX("Error: Unknown option: %s", opt.string());
                return NOERROR;
            }

            ec = NextOption(&opt);
            FAILED_AND_RETURN(ec);
        }
        ec = NextArgRequired(&process);
        FAILED_AND_RETURN(ec);
    }
    else if (cmd.Equals("stop")) {
        String opt;
        ec = NextOption(&opt);
        FAILED_AND_RETURN(ec);
        while (!opt.IsNull()) {
            if (opt.Equals("--user")) {
                String userArg;
                ec = NextArgRequired(&userArg);
                FAILED_AND_RETURN(ec);
                userId = ParseUserArg(userArg);
            }
            else {
                PFL_EX("Error: Unknown option: %s", opt.string());
                return NOERROR;
            }

            ec = NextOption(&opt);
            FAILED_AND_RETURN(ec);
        }
        process = NextArg();
    }
    else {
        // Compatibility with old syntax: process is specified first.
        process = cmd;
        ec = NextArgRequired(&cmd);
        FAILED_AND_RETURN(ec);
        if (cmd.Equals("start")) {
            start = TRUE;
        }
        else if (!cmd.Equals("stop")) {
            Logger::E(TAG, "Profile command %s not valid", process.string());
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    if (userId == IUserHandle::USER_ALL) {
        PFL_EX("Error: Can't profile with user 'all'");
        return NOERROR;
    }

    AutoPtr<IParcelFileDescriptor> fd;
    if (start) {
        ec = NextArgRequired(&profileFile);
        FAILED_AND_RETURN(ec);
        AutoPtr<IParcelFileDescriptorHelper> parcelHelper;
        CParcelFileDescriptorHelper::AcquireSingleton((IParcelFileDescriptorHelper**)&parcelHelper);
        AutoPtr<IFile> file;
        CFile::New(profileFile, (IFile**)&file);
        ec = parcelHelper->Open(file,
            IParcelFileDescriptor::MODE_CREATE |
            IParcelFileDescriptor::MODE_TRUNCATE |
            IParcelFileDescriptor::MODE_READ_WRITE,
            (IParcelFileDescriptor**)&fd);
        if (ec == (Int32)E_FILE_NOT_FOUND_EXCEPTION) {
            PFL_EX("Error: Unable to open file: %s", profileFile.string());
            return E_FILE_NOT_FOUND_EXCEPTION;
        }
    }

    if (wall) {
        // XXX doesn't work -- this needs to be set before booting.
        AutoPtr<ISystemProperties> sysProp;
        CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
        String props;
        sysProp->Get(String("dalvik.vm.extra-opts"), &props);
        if (props.IsNull() || !props.Contains("-Xprofile:wallclock")) {
            props = props + " -Xprofile:wallclock";
            //sysProp->Set("dalvik.vm.extra-opts", props);
        }
    }
    else if (start) {
        //removeWallOption();
    }

    Boolean result;
    mAm->ProfileControl(process, userId, start, profileFile, fd, profileType, &result);
    if (!result) {
        wall = FALSE;
        Logger::E("PROFILE FAILED on process %s", process.string());
        return E_IO_EXCEPTION;
    }

    return NOERROR;
}

ECode CAm::RunDumpHeap()
{
    Boolean managed(TRUE);
    Int32 userId = IUserHandle::USER_CURRENT;
    String opt;
    ECode ec = NextOption(&opt);
    FAILED_AND_RETURN(ec);
    while (!opt.IsNull()) {
        if (opt.Equals("--user")) {
            String userArg;
            ec = NextArgRequired(&userArg);
            FAILED_AND_RETURN(ec);
            userId = ParseUserArg(userArg);
            if (userId == IUserHandle::USER_ALL) {
                PFL_EX("Error: Can't dump heap with user 'all'");
                return NOERROR;
            }
        }
        else if (opt.Equals("-n")) {
            managed = FALSE;
        }
        else {
            PFL_EX("Error: Unknown option: %s", opt.string());
            return NOERROR;
        }
        ec = NextOption(&opt);
        FAILED_AND_RETURN(ec);
    }

    String process, heapFile;
    ec = NextArgRequired(&process);
    FAILED_AND_RETURN(ec);
    ec = NextArgRequired(&heapFile);
    FAILED_AND_RETURN(ec);
    AutoPtr<IParcelFileDescriptor> fd;
    AutoPtr<IFile> file;
    Boolean isDelete;
    CFile::New(heapFile, (IFile**)&file);
    file->Delete(&isDelete);

    AutoPtr<IParcelFileDescriptorHelper> parcelHelper;
    CParcelFileDescriptorHelper::AcquireSingleton((IParcelFileDescriptorHelper**)&parcelHelper);
    ec = parcelHelper->Open(file,
        IParcelFileDescriptor::MODE_CREATE |
        IParcelFileDescriptor::MODE_TRUNCATE |
        IParcelFileDescriptor::MODE_READ_WRITE,
        (IParcelFileDescriptor**)&fd);
    if (ec == (Int32)E_FILE_NOT_FOUND_EXCEPTION) {
        PFL_EX("Error: Unable to open file: %s", heapFile.string());
        return NOERROR;
    }

    Boolean result;
    mAm->DumpHeap(process, userId, managed, heapFile, fd, &result);
    if (!result) {
        Logger::E("HEAP DUMP FAILED on process %s", process.string());
        return E_IO_EXCEPTION;
    }

    return NOERROR;
}

ECode CAm::RunSetDebugApp()
{
    Boolean wait = FALSE;
    Boolean persistent = FALSE;
    String opt;
    ECode ec = NextOption(&opt);
    FAILED_AND_RETURN(ec);
    while(!opt.IsNull()) {
        if (opt.Equals("-w")) {
            wait = TRUE;
        }
        else if (opt.Equals("--persistent")) {
            persistent = TRUE;
        }
        else {
            PFL_EX("Error: Unknown option: %s", opt.string());
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        ec = NextOption(&opt);
        FAILED_AND_RETURN(ec);
    }

    String pkg;
    ec = NextArgRequired(&pkg);
    FAILED_AND_RETURN(ec);
    return mAm->SetDebugApp(pkg, wait, persistent);
}

ECode CAm::RunClearDebugApp()
{
    return mAm->SetDebugApp(String(), FALSE, TRUE);
}

ECode CAm::RunBugReport()
{
    ECode ec = mAm->RequestBugReport();
    PFL_EX("Your lovely bug report is being created; please be patient.");
    return ec;
}

ECode CAm::RunSwitchUser()
{
    String user;
    ECode ec = NextArgRequired(&user);
     FAILED_AND_RETURN(ec);
    Int32 userId;
    Boolean result;
    StringUtils::ParseInt32(user, &userId);
    return mAm->SwitchUser(userId, &result);
}

ECode CAm::RunStopUser()
{
    String user;
    ECode ec = NextArgRequired(&user);
    FAILED_AND_RETURN(ec);
    Int32 res;
    mAm->StopUser(StringUtils::ParseInt32(user), NULL, &res);
    if (res != IActivityManager::USER_OP_SUCCESS) {
        String txt("");
        switch (res) {
            case IActivityManager::USER_OP_IS_CURRENT:
                txt = " (Can't stop current user)";
                break;
            case IActivityManager::USER_OP_UNKNOWN_USER:
                txt = String(" (Unknown user ") + user + ")";
                break;
        }
        PFL_EX("Switch failed: %d%s", res, txt.string());
    }
    return NOERROR;
}

ECode CAm::RunMonitor()
{
    String opt;
    String gdbPort;
    ECode ec = NextOption(&opt);
    FAILED_AND_RETURN(ec);
    while(!opt.IsNull()) {
        if (opt.Equals("--gdb")) {
            ec = NextArgRequired(&gdbPort);
            FAILED_AND_RETURN(ec);
        }
        else {
            PFL_EX("Error: Unknown option: %s", opt.string());
            return NOERROR;
        }
        ec = NextOption(&opt);
        FAILED_AND_RETURN(ec);
    }

    AutoPtr<IIActivityController> controller;
    CMyActivityController::New((Handle32)this, gdbPort, (IIActivityController**)&controller);
    MyActivityController* mc = reinterpret_cast<MyActivityController*>(controller->Probe(EIID_MyActivityController));
    return mc->Run();
}

ECode CAm::RunScreenCompat()
{
    String mode;
    ECode ec = NextArgRequired(&mode);
    FAILED_AND_RETURN(ec);
    Boolean enabled;
    if (mode.Equals("on")) {
        enabled = TRUE;
    }
    else if (mode.Equals("off")) {
        enabled = FALSE;
    }
    else {
        PFL_EX("Error: enabled mode must be 'on' or 'off' at %s", mode.string());
        return NOERROR;
    }

    String packageName;
    ec = NextArgRequired(&packageName);
    FAILED_AND_RETURN(ec);
    do {
        mAm->SetPackageScreenCompatMode(packageName, enabled
            ? IActivityManager::COMPAT_MODE_ENABLED
            : IActivityManager::COMPAT_MODE_DISABLED);
        packageName = NextArg();
    }
    while (!packageName.IsNull());
    return NOERROR;
}

ECode CAm::RunDisplaySize()
{
    String size;
    ECode ec = NextArgRequired(&size);
    FAILED_AND_RETURN(ec);
    Int32 w, h;
    if (size.Equals("reset")) {
        w = h = -1;
    }
    else {
        Int32 div = size.IndexOf('x');
        if (div <= 0 || div >= (size.GetLength()-1)) {
            PFL_EX("Error: bad size %s", size.string());
            return NOERROR;
        }

        String wstr = size.Substring(0, div);
        String hstr = size.Substring(div+1);
        ECode ec = StringUtils::ParseInt32(wstr, &w);
        if (FAILED(ec)) {
            Logger::E(TAG, "Error: bad number %d", ec);
            return NOERROR;
        }

        ec = StringUtils::ParseInt32(hstr, &h);
        if (FAILED(ec)) {
            Logger::E(TAG, "Error: bad number %d", ec);
            return NOERROR;
        }
    }

    AutoPtr<IServiceManager> sm;
    AutoPtr<IInterface> base;
    CServiceManager::AcquireSingleton((IServiceManager**)&sm);
    sm->GetService(IContext::WINDOW_SERVICE, (IInterface**)&base);
    AutoPtr<IIWindowManager> wm = IIWindowManager::Probe(base);

    if (wm == NULL) {
        PFL_EX(NO_SYSTEM_ERROR_CODE.string());
        Logger::E(TAG, "Can't connect to window manager; is the system running?");
        return E_REMOTE_SERVICE_EXCEPTION;
    }

    if (w >= 0 && h >= 0) {
        // TODO(multidisplay): For now Configuration only applies to main screen.
        wm->SetForcedDisplaySize(IDisplay::DEFAULT_DISPLAY, w, h);
    }
    else {
        wm->ClearForcedDisplaySize(IDisplay::DEFAULT_DISPLAY);
    }

    return NOERROR;
}

ECode CAm::RunDisplayDensity()
{
    String densityStr;
    ECode ec = NextArgRequired(&densityStr);
    FAILED_AND_RETURN(ec);

    Int32 density;
    if (densityStr.Equals("reset")) {
        density = -1;
    }
    else {
        ECode ec = StringUtils::ParseInt32(densityStr, &density);
        if (FAILED(ec)) {
            PFL_EX("Error: bad number %d", ec);
            return NOERROR;
        }

        if (density < 72) {
            PFL_EX("Error: density must be >= 72");
            return NOERROR;
        }
    }

    AutoPtr<IServiceManager> sm;
    AutoPtr<IInterface> base;
    CServiceManager::AcquireSingleton((IServiceManager**)&sm);
    sm->GetService(IContext::WINDOW_SERVICE, (IInterface**)&base);
    AutoPtr<IIWindowManager> wm = IIWindowManager::Probe(base);

    if (wm == NULL) {
        PFL_EX(NO_SYSTEM_ERROR_CODE.string());
        Logger::E(TAG, "Can't connect to window manager; is the system running?");
        return E_REMOTE_SERVICE_EXCEPTION;
    }

    if (density > 0) {
        // TODO(multidisplay): For now Configuration only applies to main screen.
        wm->SetForcedDisplayDensity(IDisplay::DEFAULT_DISPLAY, density);
    }
    else {
        wm->ClearForcedDisplayDensity(IDisplay::DEFAULT_DISPLAY);
    }

    return NOERROR;
}

ECode CAm::RunToUri(
        /* [in] */ Boolean intentScheme)
{
    AutoPtr<IIntent> intent;
    MakeIntent(IUserHandle::USER_CURRENT, (IIntent**)&intent);
    String scheme;
    intent->ToUri(intentScheme ? IIntent::URI_INTENT_SCHEME : 0, &scheme);
    PFL_EX(scheme.string());
    return NOERROR;
}

ECode CAm::NextOption(
    /* [out] */ String* nextOpt)
{
    VALIDATE_NOT_NULL(nextOpt);
    if (!mCurArgData.IsNull()) {
        String prev = (*mArgs)[mNextArg - 1];
        Logger::E(TAG, "No argument expected after \"%s\"", prev.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mNextArg >= mArgs->GetLength()) {
        *nextOpt = String();
        return NOERROR;
    }

    String arg = (*mArgs)[mNextArg];
    if (!arg.StartWith("-")) {
        *nextOpt = String();
        return NOERROR;
    }

    mNextArg++;
    if (arg.Equals("--")) {
        *nextOpt = String();
        return NOERROR;
    }

    if (arg.GetLength() > 1 && arg.GetChar(1) != '-') {
        if (arg.GetLength() > 2) {
            mCurArgData = arg.Substring(2);
            *nextOpt = arg.Substring(0, 2);
            return NOERROR;
        }
        else {
            mCurArgData = String();
            *nextOpt = arg;
            return NOERROR;
        }
    }

    mCurArgData = String();
    *nextOpt = arg;
    return NOERROR;
}

String CAm::NextArg()
{
    if (!mCurArgData.IsNull()) {
        String arg = mCurArgData;
        mCurArgData = String();
        return arg;
    }
    else if (mNextArg < mArgs->GetLength()) {
        return (*mArgs)[mNextArg++];
    }
    else {
        return String();
    }
}

ECode CAm::NextArgRequired(
    /* [out] */ String* nextArgRequired)
{
    VALIDATE_NOT_NULL(nextArgRequired);
    String arg = NextArg();
    if (arg.IsNull()) {
        String prev = (*mArgs)[mNextArg - 1];
        Logger::E(TAG, "Argument expected after \"%s\"", prev.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    *nextArgRequired = arg;
    return NOERROR;
}

ECode CAm::ShowUsage()
{
    String usage;
    usage = usage +
        "usage: am [subcommand] [options] \n" +
        "usage: am start [-D] [-W] [-P <FILE>] [--start-profiler <FILE>]\n" +
        "               [--R COUNT] [-S] [--opengl-trace]\n" +
        "               [--user <USER_ID> | current] <INTENT>\n" +
        "       am startservice [--user <USER_ID> | current] <INTENT>\n" +
        "       am force-stop [--user <USER_ID> | all | current] <PACKAGE>\n" +
        "       am kill [--user <USER_ID> | all | current] <PACKAGE>\n" +
        "       am kill-all\n" +
        "       am broadcast [--user <USER_ID> | all | current] <INTENT>\n" +
        "       am instrument [-r] [-e <NAME> <VALUE>] [-p <FILE>] [-w]\n" +
        "               [--user <USER_ID> | current]\n" +
        "               [--no-window-animation] <COMPONENT>\n" +
        "       am profile start [--user <USER_ID> current] <PROCESS> <FILE>\n" +
        "       am profile stop [--user <USER_ID> current] [<PROCESS>]\n" +
        "       am dumpheap [--user <USER_ID> current] [-n] <PROCESS> <FILE>\n" +
        "       am set-debug-app [-w] [--persistent] <PACKAGE>\n" +
        "       am clear-debug-app\n" +
        "       am monitor [--gdb <port>]\n" +
        "       am screen-compat [on|off] <PACKAGE>\n" +
        "       am display-size [reset|WxH]\n" +
        "       am display-density [reset|DENSITY]\n" +
        "       am to-uri [INTENT]\n" +
        "       am to-intent-uri [INTENT]\n" +
        "       am switch-user <USER_ID>\n" +
        "       am stop-user <USER_ID>\n" +
        "\n" +
        "am start: start an Activity.  Options are:\n" +
        "    -D: enable debugging\n" +
        "    -W: wait for launch to complete\n" +
        "    --start-profiler <FILE>: start profiler and send results to <FILE>\n" +
        "    -P <FILE>: like above, but profiling stops when app goes idle\n" +
        "    -R: repeat the activity launch <COUNT> times.  Prior to each repeat,\n" +
        "        the top activity will be finished.\n" +
        "    -S: force stop the target app before starting the activity\n" +
        "    --opengl-trace: enable tracing of OpenGL functions\n" +
        "    --user <USER_ID> | current: Specify which user to run as; if not\n" +
        "        specified then run as the current user.\n" +
        "\n" +
        "am startservice: start a Service.  Options are:\n" +
        "    --user <USER_ID> | current: Specify which user to run as; if not\n" +
        "        specified then run as the current user.\n" +
        "\n" +
        "am force-stop: force stop everything associated with <PACKAGE>.\n" +
        "    --user <USER_ID> | all | current: Specify user to force stop;\n" +
        "        all users if not specified.\n" +
        "\n" +
        "am kill: Kill all processes associated with <PACKAGE>.  Only kills.\n" +
        "  processes that are safe to kill -- that is, will not impact the user\n" +
        "  experience.\n" +
        "    --user <USER_ID> | all | current: Specify user whose processes to kill;\n" +
        "        all users if not specified.\n" +
        "\n" +
        "am kill-all: Kill all background processes.\n" +
        "\n" +
        "am broadcast: send a broadcast Intent.  Options are:\n" +
        "    --user <USER_ID> | all | current: Specify which user to send to; if not\n" +
        "        specified then send to all users.\n" +
        "\n" +
        "am instrument: start an Instrumentation.  Typically this target <COMPONENT>\n" +
        "  is the form <TEST_PACKAGE>/<RUNNER_CLASS>.  Options are:\n" +
        "    -r: print raw results (otherwise decode REPORT_KEY_STREAMRESULT).  Use with\n" +
        "        [-e perf true] to generate raw output for performance measurements.\n" +
        "    -e <NAME> <VALUE>: set argument <NAME> to <VALUE>.  For test runners a\n" +
        "        common form is [-e <testrunner_flag> <value>[,<value>...]].\n" +
        "    -p <FILE>: write profiling data to <FILE>\n" +
        "    -w: wait for instrumentation to finish before returning.  Required for\n" +
        "        test runners.\n" +
        "    --user <USER_ID> | current: Specify user instrumentation runs in;\n" +
        "        current user if not specified.\n" +
        "    --no-window-animation: turn off window animations will running.\n" +
        "\n" +
        "am profile: start and stop profiler on a process.  The given <PROCESS> argument\n" +
        "  may be either a process name or pid.  Options are:\n" +
        "    --user <USER_ID> | current: When supplying a process name,\n" +
        "        specify user of process to profile; uses current user if not specified.\n" +
        "\n" +
        "am dumpheap: dump the heap of a process.  The given <PROCESS> argument may\n" +
        "  be either a process name or pid.  Options are:\n" +
        "    -n: dump native heap instead of managed heap\n" +
        "    --user <USER_ID> | current: When supplying a process name,\n" +
        "        specify user of process to dump; uses current user if not specified.\n" +
        "\n" +
        "am set-debug-app: set application <PACKAGE> to debug.  Options are:\n" +
        "    -w: wait for debugger when application starts\n" +
        "    --persistent: retain this value\n" +
        "\n" +
        "am clear-debug-app: clear the previously set-debug-app.\n" +
        "\n" +
        "am bug-report: request bug report generation; will launch UI\n" +
        "    when done to select where it should be delivered." +
        "\n" +
        "am monitor: start monitoring for crashes or ANRs.\n" +
        "    --gdb: start gdbserv on the given port at crash/ANR\n" +
        "\n" +
        "am screen-compat: control screen compatibility mode of <PACKAGE>.\n" +
        "\n" +
        "am display-size: override display size.\n" +
        "\n" +
        "am display-density: override display density.\n" +
        "\n" +
        "am to-uri: print the given Intent specification as a URI.\n" +
        "\n" +
        "am to-intent-uri: print the given Intent specification as an intent: URI.\n" +
        "\n" +
        "am switch-user: switch to put USER_ID in the foreground, starting\n" +
        "  execution of that user if it is currently stopped.\n" +
        "\n" +
        "am stop-user: stop execution of USER_ID, not allowing it to run any\n" +
        "  code until a later explicit switch to it.\n" +
        "\n" +
        "<INTENT> specifications include these flags and arguments:\n" +
        "    [-a <ACTION>] [-d <DATA_URI>] [-t <MIME_TYPE>]\n" +
        "    [-c <CATEGORY> [-c <CATEGORY>] ...]\n" +
        "    [-e|--es <EXTRA_KEY> <EXTRA_STRING_VALUE> ...]\n" +
        "    [--esn <EXTRA_KEY> ...]\n" +
        "    [--ez <EXTRA_KEY> <EXTRA_BOOLEAN_VALUE> ...]\n" +
        "    [--ei <EXTRA_KEY> <EXTRA_INT_VALUE> ...]\n" +
        "    [--el <EXTRA_KEY> <EXTRA_LONG_VALUE> ...]\n" +
        "    [--ef <EXTRA_KEY> <EXTRA_FLOAT_VALUE> ...]\n" +
        "    [--eu <EXTRA_KEY> <EXTRA_URI_VALUE> ...]\n" +
        "    [--ecn <EXTRA_KEY> <EXTRA_COMPONENT_NAME_VALUE>]\n" +
        "    [--eia <EXTRA_KEY> <EXTRA_INT_VALUE>[,<EXTRA_INT_VALUE...]]\n" +
        "    [--ela <EXTRA_KEY> <EXTRA_LONG_VALUE>[,<EXTRA_LONG_VALUE...]]\n" +
        "    [--efa <EXTRA_KEY> <EXTRA_FLOAT_VALUE>[,<EXTRA_FLOAT_VALUE...]]\n" +
        "    [-n <COMPONENT>] [-f <FLAGS>]\n" +
        "    [--grant-read-uri-permission] [--grant-write-uri-permission]\n" +
        "    [--debug-log-resolution] [--exclude-stopped-packages]\n" +
        "    [--include-stopped-packages]\n" +
        "    [--activity-brought-to-front] [--activity-clear-top]\n" +
        "    [--activity-clear-when-task-reset] [--activity-exclude-from-recents]\n" +
        "    [--activity-launched-from-history] [--activity-multiple-task]\n" +
        "    [--activity-no-animation] [--activity-no-history]\n" +
        "    [--activity-no-user-action] [--activity-previous-is-top]\n" +
        "    [--activity-reorder-to-front] [--activity-reset-task-if-needed]\n" +
        "    [--activity-single-top] [--activity-clear-task]\n" +
        "    [--activity-task-on-home]\n" +
        "    [--receiver-registered-only] [--receiver-replace-pending]\n" +
        "    [--selector]\n" +
        "    [<URI> | <PACKAGE> | <COMPONENT>]\n";
    PFL_EX("%s", usage.string());
    return NOERROR;
}

AutoPtr<IIActivityManager> CAm::GetAm()
{
    return mAm;
}

}
}
}
}

