
NativeDaemonCommand::NativeDaemonCommand(
    /* [in] */ const String& cmd,
    /* [in] */ ArrayOf<IInterface*>* args)
{
    mCmd = cmd;
    for (Int32 i = 0; i < args->GetLength(); i++) {
        IInterface* arg = args[i];
        AppendArg(arg);
    }
}

NativeDaemonCommand NativeDaemonCommand::AppendArg(
    /* [in] */ IInterface* arg)
{
    mArguments.Insert(arg);
    return this;
}
