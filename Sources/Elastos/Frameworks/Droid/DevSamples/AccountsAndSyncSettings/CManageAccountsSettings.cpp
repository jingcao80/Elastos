
#include "CManageAccountsSettings.h"
#include "R.h"
//#include "CAccountManagerCallback.h"
#include <elastos/utility/logging/Slogger.h>
#include <unistd.h>

using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::Accounts::IAccountManager;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::Accounts::CAccountManager;
using Elastos::Droid::Accounts::IAccountManagerFuture;
using Elastos::Droid::Accounts::IAccountManagerHelper;
using Elastos::Droid::Accounts::CAccountManagerHelper;
using Elastos::Droid::Accounts::IAccount;
using Elastos::Droid::Accounts::CAccount;
using Elastos::Utility::Logging::Slogger;

namespace AccountsAndSyncSettings {

// static pthread_key_t thread_log_key;

//=======================================================================
//              CManageAccountsSettings::MyListener
//=======================================================================
CAR_INTERFACE_IMPL(CManageAccountsSettings::MyListener, IViewOnClickListener)

CManageAccountsSettings::MyListener::MyListener(
    /* [in] */ CManageAccountsSettings* host)
    : mActivity(host)
{
}

ECode CManageAccountsSettings::MyListener::OnClick(
    /* [in] */ IView* v)
{
    Int32 viewId = 0;
    if (FAILED(v->GetId(&viewId))) {
        Slogger::E("CManageAccountsSettings", "GetId failed!");
    }

    Slogger::E("CManageAccountsSettings", "GetId viewId:%d", viewId);

    switch(viewId) {
        case R::id::add_account_button: {
            //Slogger::D("CManageAccountsSettings", "OnClick---add_account_button");
            //mActivity->GetPassword();
            Slogger::D("CManageAccountsSettings", "OnClick---add_account_button to test start");
            mActivity->AddAccount(String("com.google.exchange"));
            Slogger::D("CManageAccountsSettings", "OnClick---add_account_button to test end");
            break;
        }

        case R::id::add_getaccount_button: {
            Slogger::D("CManageAccountsSettings", "OnClic k---RemoveAccount");
            Slogger::D("CManageAccountsSettings", "GetAccounts----1");
            mActivity->GetAccounts();
            Slogger::D("CManageAccountsSettings", "GetAccounts----2");
            Slogger::D("CManageAccountsSettings", "GetPassword----1");
            mActivity->GetPassword();
            Slogger::D("CManageAccountsSettings", "GetPassword----2");
            Slogger::D("CManageAccountsSettings", "UpdateAccount----1");
            mActivity->UpdateAccount();
            Slogger::D("CManageAccountsSettings", "UpdateAccount----2");
            Slogger::D("CManageAccountsSettings", "RemoveAccount----1");
            mActivity->RemoveAccount();
            Slogger::D("CManageAccountsSettings", "RemoveAccount----2");
            Slogger::D("CManageAccountsSettings", "OnClick---RemoveAccount");
            break;
        }

        case R::id::update_account_button: {
            Slogger::D("CManageAccountsSettings", "OnClick---update_account_button");
            mActivity->UpdateAccount();
            Slogger::D("CManageAccountsSettings", "OnClick---update_account_button end");
            break;
        }
        default:
            break;
    }


    return NOERROR;
}

//=======================================================================
//              CManageAccountsSettings
//=======================================================================
ECode CManageAccountsSettings::OnCreate(
/* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::manage_accounts_screen);

    AutoPtr<MyListener> l = new MyListener(this);

    AutoPtr<IView> addaccountbtn = FindViewById(R::id::add_account_button);
    assert(addaccountbtn != NULL);
    addaccountbtn->SetOnClickListener(l);

    AutoPtr<IView> getaccountbtn = FindViewById(R::id::add_getaccount_button);
    assert(getaccountbtn != NULL);
    getaccountbtn->SetOnClickListener(l);

    // debug pthread key related




    return NOERROR;
}

ECode CManageAccountsSettings::AddAccount(
  const String& accountType)
{
    Slogger::I("accountrelated", "xxxxxxaccountType:%s", accountType.string());
    String server, username("zhangyina0317@gmail.com"), password("abcd1234"), type;
    AutoPtr<IAccount> account, account2, account3, account4;
    CAccount::New(username, accountType, (IAccount**)&account);
    CAccount::New(String("minato@gmail.com"), String("baidu.com"), (IAccount**)&account2);
    CAccount::New(String("shanghai@apple.com"), String("apple.com"), (IAccount**)&account3);
    CAccount::New(String("kuxina@qq.com"), String("qq.com"), (IAccount**)&account4);


    AutoPtr<IBundle> userdata;
    CBundle::New((IBundle**)&userdata);

    userdata->PutString(String("server"), String("server"));

    //AccountManager am = AccountManager.get(self);
    AutoPtr<IAccountManager> accountMgr;
    AutoPtr<IAccountManagerHelper> helper;
    ASSERT_SUCCEEDED(CAccountManagerHelper::AcquireSingleton((IAccountManagerHelper**)&helper));
    helper->Get(this, (IAccountManager**)&accountMgr);

    // 向帐户管理器中添加一个帐户
    Boolean result;
    ECode ec = accountMgr->AddAccountExplicitly(account2, password, userdata, &result);
    Slogger::I("accountrelated", "the ec is %d result:%d", ec, result);
    ec = accountMgr->AddAccountExplicitly(account3, password, userdata, &result);
    Slogger::I("accountrelated", "the ec is %d result:%d", ec, result);
    ec = accountMgr->AddAccountExplicitly(account4, password, userdata, &result);
    Slogger::I("accountrelated", "the ec is %d result:%d", ec, result);
    /*ec = accountMgr->AddAccountExplicitly(account3, password, userdata, &result);
    Slogger::I("accountrelated", "the ec is %d result:%d", ec, result);*/
    return NOERROR;
}

ECode CManageAccountsSettings::GetAccounts()
{
  AutoPtr< ArrayOf<IAccount*> > accounts;
  AutoPtr<IAccountManager> accountMgr;
  AutoPtr<IAccountManagerHelper> helper;
  ASSERT_SUCCEEDED(CAccountManagerHelper::AcquireSingleton((IAccountManagerHelper**)&helper));
  helper->Get(this, (IAccountManager**)&accountMgr);
  Boolean result;
  ECode ec = accountMgr->GetAccounts((ArrayOf<IAccount*>**)&accounts);

  if (accounts != NULL) {
    Slogger::I("oooooGetAccountsoooo", "Account length:%d", accounts->GetLength());
    String tmpName, tmpType;
    for (int i = 0; i < accounts->GetLength(); ++i) {
        AutoPtr<IAccount> account = (*accounts)[i];
        account->GetName(&tmpName);
        account->GetType(&tmpType);
        Slogger::I("oooooGetAccountsoooo", "Account with name:%s and type:%s", tmpName.string(), tmpType.string());
    }
  }

  return NOERROR;
}

ECode CManageAccountsSettings::UpdateAccount() {
    String server, username("zhangyina0317@gmail.com"), password("abcd1234"), accountType("com.android.exchange");
    AutoPtr<IAccount> account, account2, account3, account4;
    CAccount::New(username, accountType, (IAccount**)&account);
    CAccount::New(String("minato@gmail.com"), String("baidu.com"), (IAccount**)&account2);
    CAccount::New(String("shanghai@apple.com"), String("apple.com"), (IAccount**)&account3);
    CAccount::New(String("kuxina@qq.com"), String("qq.com"), (IAccount**)&account4);


    AutoPtr<IBundle> userdata;
    CBundle::New((IBundle**)&userdata);

    userdata->PutString(String("server"), String("server"));

    //AccountManager am = AccountManager.get(self);
    AutoPtr<IAccountManager> accountMgr;
    AutoPtr<IAccountManagerHelper> helper;
    ASSERT_SUCCEEDED(CAccountManagerHelper::AcquireSingleton((IAccountManagerHelper**)&helper));
    helper->Get(this, (IAccountManager**)&accountMgr);

    Boolean result;
    ECode ec = accountMgr->SetPassword(account2, String("youarethefirstone"));
    Slogger::I("accountrelated", "the ec is %d result:%d", ec, result);
    ec = accountMgr->SetPassword(account3, String("youarethefirsttwo"));
    Slogger::I("accountrelated", "the ec is %d result:%d", ec, result);
    ec = accountMgr->SetPassword(account4, String("youarethefirstthree"));
    Slogger::I("accountrelated", "the ec is %d result:%d", ec, result);
    return NOERROR;
}

ECode CManageAccountsSettings::RemoveAccount(){
  String server, username("zhangyina0317@gmail.com"), password("abcd1234"), accountType("com.android.exchange");
  AutoPtr<IAccount> account2;
  CAccount::New(String("minato@gmail.com"), String("baidu.com"), (IAccount**)&account2);

  AutoPtr<IBundle> userdata;
  CBundle::New((IBundle**)&userdata);

  userdata->PutString(String("server"), String("server"));

  //AccountManager am = AccountManager.get(self);
  AutoPtr<IAccountManager> accountMgr;
  AutoPtr<IAccountManagerHelper> helper;
  ASSERT_SUCCEEDED(CAccountManagerHelper::AcquireSingleton((IAccountManagerHelper**)&helper));
  helper->Get(this, (IAccountManager**)&accountMgr);

  Boolean result;
  AutoPtr<IAccountManagerFuture> amf;
  ECode ec = accountMgr->RemoveAccount(account2, NULL, NULL, (IAccountManagerFuture**)&amf);
  Slogger::I("accountrelated", "the ec is %d result:%d", ec, result);
}

ECode CManageAccountsSettings::GetPassword(){

  String server, username("zhangyina0317@gmail.com"), password("abcd1234"), accountType("com.android.exchange");
  AutoPtr<IAccount> account, account2, account3, account4;
  CAccount::New(username, accountType, (IAccount**)&account);
  CAccount::New(String("minato@gmail.com"), String("baidu.com"), (IAccount**)&account2);
  CAccount::New(String("shanghai@apple.com"), String("apple.com"), (IAccount**)&account3);
  CAccount::New(String("kuxina@qq.com"), String("qq.com"), (IAccount**)&account4);


  AutoPtr<IBundle> userdata;
  CBundle::New((IBundle**)&userdata);

  userdata->PutString(String("server"), String("server"));

  //AccountManager am = AccountManager.get(self);
  AutoPtr<IAccountManager> accountMgr;
  AutoPtr<IAccountManagerHelper> helper;
  ASSERT_SUCCEEDED(CAccountManagerHelper::AcquireSingleton((IAccountManagerHelper**)&helper));
  helper->Get(this, (IAccountManager**)&accountMgr);

  Boolean result;

  String passwd, userdata1, passwd3, userdata2;
  ECode ec = accountMgr->GetPassword(account2, &passwd);
  Slogger::I("accountrelated", "the ec is %d password:%s", ec, passwd.string());
  ec = accountMgr->GetUserData(account3, String("server"), &userdata1);
  Slogger::I("accountrelated", "the ec is %d userdata1:%d", ec, userdata1.string());
  ec = accountMgr->SetUserData(account4, String("server2222"), String("server111111111111"));
  ec = accountMgr->GetUserData(account4, String("server"), &userdata2);
  Slogger::I("accountrelated", "the ec 22222222222222 userdata2 is %d userdata:%d", ec, userdata2.string());
  return NOERROR;
}

pthread_key_t key;
void echomsg(void* p)
{
  int t=*(int*)p;
  Slogger::I("threadtest", "destructor excuted in thread %d, param=%d\n ",pthread_self(),t);
}

void* child1(void* arg)
{
  int*ptid= new int;
  *ptid=pthread_self();
  Slogger::I("threadtest", "thread %d enter\n ",*ptid);
  pthread_setspecific(key,(void*)ptid);
  sleep(2);
  Slogger::I("threadtest", "thread %d returns %d\n ",*ptid,*((int*)pthread_getspecific(key)));
  sleep(5);
  pthread_exit(NULL);
  return NULL;
}

void* child2(void* arg)
{
  int*ptid= new int;
  *ptid=pthread_self();
  Slogger::I("threadtest", "thread %d enter\n ",*ptid);
  pthread_setspecific(key,(void*)ptid);
  sleep(1);
  Slogger::I("threadtest", "thread %d returns %d\n ",*ptid,*((int*)pthread_getspecific(key)));
  sleep(5);
  pthread_exit(NULL);
  return NULL;
}


ECode CManageAccountsSettings::TestpThreadKey() {
  Slogger::I("threadrelated", "TestpThreadKey---1");
  pthread_t tid1,tid2;
  Slogger::I("threadtest", "hello\n ");
  pthread_key_create(&key,echomsg);
  pthread_create(&tid1,NULL,child1,NULL);
  pthread_create(&tid2,NULL,child2,NULL);
  pthread_join(tid1,NULL);
  pthread_join(tid2,NULL);
  sleep(3);
  pthread_key_delete(key);
  Slogger::I("threadtest", "main thread %d exit\n ",pthread_self());
  Slogger::I("threadrelated", "TestpThreadKey---2");
  Slogger::I("threadrelated", "TestpThreadKey---3");

  return 0;
}

} // namespace AccountsAndSyncSettings


