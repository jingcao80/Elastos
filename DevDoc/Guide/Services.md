## Services ##
服务也是一个应用程序组件，可以在后台执行耗时的操作，不提供用户界面。另一个应用程序组件可以启动一个服务，它将继续在后台运行，即使用户切换到另一个应用程序。此外，一个组件可以绑定到一个服务与之通信，甚至执行进程间通信（IPC）。例如，一个服务可以处理网络事务，播放音乐，执行文件I/O，或者与内容提供者交互，而这一切都在后台处理。

**服务的Bound**

通过调用BindService()方法被绑定的服务称为“Bound“，一个被绑定的服务提供一个客户端服务器的接口，允许组件与服务交互，发送请求，获得结果，甚至也可以跨进程通信（IPC）。绑定服务只有当有其它应用组件绑定到服务上才能运行。多个组件可以同时绑定到服务，当组件全部解除绑定时服务停止并被销毁。

**创建一个Bound式的服务**

当服务只为你的应用服务时，通过IBinder的OnBind()方法提供Binder给客户端，让客户端可以访问Binder或Service提供的方法。

- 实现IBinder，并提供方法可以获取到服务。
头文件中声明方法，GetService()提供Service服务接口：
```c++
class HelloBinder
    : public IBinder
    , public ElRefBase
{
public :
    HelloBinder(
        /* [in] */ CBindHelloService* host);

    CAR_INTERFACE_DECL();

    CARAPI GetService(
        /*out*/ CBindHelloService* bindeHelloService);

    CARAPI ToString(
        /*[out]*/ String* str);

private:
    AutoPtr<CBindHelloService> mHost;
};
```
- 在service的OnBind()方法实现中对外提供IBinder
```c++
ECode CBindHelloService::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** binder)
{
    Logger::D(TAG, "Service OnBind");
    *binder = mBinder;
    INTERFACE_ADDREF(*binder);
    return NOERROR;
};
```
- 在Activity中获取服务
IServiceConnection负责监视Service，当调用BindService()时回调此接口：
```c++
class CServiceConnection
    : public IServiceConnection
    , public ElRefBase
{
public:
    CServiceConnection(
        /* [in] */ CMainActivity* host);

    CAR_INTERFACE_DECL();

    CARAPI OnServiceConnected (
        /*[in]*/ IComponentName* name,
        /*[in]*/ IBinder* service);

    CARAPI OnServiceDisconnected (
        /*[in]*/ IComponentName* name);

private:
    AutoPtr<CMainActivity> mHost;
};
```
+ 对应的实现，在绑定连接建立后获取服务接口。
```c++
ECode CMainActivity::CServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    assert(service != NULL);
    ((CBindHelloService::HelloBinder*)service)->GetService(mHost->mBindService);
    assert(mHost->mBindService != NULL);
    Logger::D(TAG, "OnServiceConnected");
    return NOERROR;
}
```
- 组件OnStart()时绑定服务
```c++
ECode CMainActivity::OnStart()
{
    Activity::OnStart();
    AutoPtr<IComponentName> com;
    CComponentName::New(this, String("HelloService.CBindHelloService"), (IComponentName**)&com);
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->SetComponent(com);

    Logger::D(TAG,"=== CMainActivity::OnStart : Ready to BindService === ");
    Boolean result;
    BindService(intent, mConn, IContext::BIND_AUTO_CREATE, &result);

    Logger::D(TAG, "CMainActivity OnStart");
    return NOERROR;
}
```
- 获得服务接口后便可以调用服务
```c++
ECode CMainActivity::ClickListener::OnClick(
    /*[in]*/ IView* view)
{
    Int32 viewId;
    view->GetId(&viewId);
    switch(viewId){
    case R::id::btn_hello:
        AutoPtr<IToastHelper> helper;
        CToastHelper::AcquireSingleton((IToastHelper**)&helper);
        StringBuilder sb("Hello this is a service!");
        AutoPtr<ICharSequence> cs = sb.ToCharSequence();
        AutoPtr<IToast> toast;
        helper->MakeText(mHost,cs, IToast::LENGTH_LONG, (IToast**)&toast);
        toast->Show();
        mHost->mBindService->SayHello();
        break;
    }

    return NOERROR;
}
```
- 组件停止时解绑服务
```c++
ECode CMainActivity::OnStop()
{
    Activity::OnStop();
    UnbindService(mConn);
    Logger::D(TAG, "CMainActivity OnStop");
    return NOERROR;
}
```
- 注册服务
在manifest文件中注册服务，在application添加子节点service声明服务。
```xml
<application
    android:allowBackup="true"
    android:icon="@drawable/ic_launcher"
    android:label="@string/app_name"
    android:theme="@style/AppTheme" >
    <activity
        android:name="CMainActivity"
        android:label="@string/app_name" >
        <intent-filter>
            <action android:name="android.intent.action.MAIN" />
            <category android:name="android.intent.category.DEFAULT" />
            <category android:name="android.intent.category.LAUNCHER" />
        </intent-filter>
    </activity>

    <service android:name="CHelloService" />
    <service android:name="CBindHelloService" />
</application>
```
到此，简单的Bound式服务完成。
