
#include "CRestarter.h"
#include <elastos/core/Thread.h>
#include <stdio.h>
#include "GeckoAppShell.h"

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Snowfox {

ECode CRestarter::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    printf("Restarter trying to restart fennec\n");
    //try {
        Int32 countdown = 40;
        Boolean result;
        ECode ec = GeckoAppShell::CheckForGeckoProcs(&result);
        while (SUCCEEDED(ec) && result &&  --countdown > 0) {
            // Wait for the old process to die before we continue
            //try {
            //AutoPtr<IThread> pThread = Thread::GetCurrentThread();
            //assert(pThread);
            Thread::Sleep(100);
            //} catch (InterruptedException ie) {}
            ec = GeckoAppShell::CheckForGeckoProcs(&result);
        }

        if (countdown <= 0) {
            // if the countdown expired, something is hung
            GeckoAppShell::KillAnyZombies();
            countdown = 10;
            // wait for the kill to take effect
            ec = GeckoAppShell::CheckForGeckoProcs(&result);
            while (SUCCEEDED(ec) && result &&  --countdown > 0) {
                //try {
                //AutoPtr<IThread> pThread = Thread::GetCurrentThread();
                //assert(pThread);
                Thread::Sleep(100);
                //} catch (InterruptedException ie) {}
                ec = GeckoAppShell::CheckForGeckoProcs(&result);
            }
        }
    //} catch (Exception e) {
    //    Log.i("Restarter", e.toString());
    //}
    //try {
        String action("android.intent.action.MAIN");
        AutoPtr<IIntent> pNewIntent;
        AutoPtr<IIntent> pIIntent;
        GetIntent((IIntent**)&pIIntent);
        assert(pIIntent);

        AutoPtr<IBundle> pIBundle;
        ec = pIIntent->GetExtras((IBundle**)&pIBundle);
        if (FAILED(ec)) return ec;

        ec = CIntent::New((IIntent**)&pNewIntent);
        if (FAILED(ec)) return ec;

        pNewIntent->SetClassName(String("org.mozilla.fennec_unofficial"),
                            String("org.mozilla.fennec_unofficial.App"));
        if (pIBundle) {
            pNewIntent->PutExtras(pIBundle);
        }
        pNewIntent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
        //printf("GeckoAppJava %s\n", pNewIntent->ToString());
        StartActivity(pNewIntent);
    //} catch (Exception e) {
    //    Log.i("Restarter", e.toString());
    //}
    // Give the new process time to start before we die
    GeckoAppShell::WaitForAnotherGeckoProc();
    //System::Exit(0);
    return NOERROR;
}

} // namespace Snowfox
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
