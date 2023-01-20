/* Copyright (c) 2021-2023 by Inoland */


#include "VivoxSubsystem.h"

#include "Library/VivoxStructLibrary.h"

#define IV_Thread_Normal                          ENamedThreads::GameThread
//ENamedThreads::AnyBackgroundThreadNormalTask

DEFINE_LOG_CATEGORY_STATIC(LogVivoxSubsystem, Display, Display);

void UVivoxSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

#if !DISABLE_VIVOXCORE
	VivoxCoreModule = static_cast<FVivoxCoreModule *>(&FModuleManager::Get().LoadModuleChecked(TEXT("VivoxCore")));
#endif
	
}

void UVivoxSubsystem::Deinitialize()
{
	Super::Deinitialize();

#if !DISABLE_VIVOXCORE
	if (IsModuleLoaded() && IsInitilized())
	{
		MyVoiceClient->Uninitialize();
		
	}
#endif
}

VivoxCoreError UVivoxSubsystem::GetError(int32 ErrorInt)
{
	return ErrorInt;
}

AccountId* UVivoxSubsystem::GetAccountId(FString Username)
{
	if (!AccountIds.Contains(Username))
	{
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("account id not found"));
		return nullptr;
	}
	return &AccountIds[Username];
}

ILoginSession* UVivoxSubsystem::GetLoginSession(FString Username)
{
	if (Username.IsEmpty())
	{
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("plz enter username"));
		return nullptr;
	}
	
	if (!IsModuleLoaded() || !IsInitilized())
	{
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("module not loaded or not initilized yet"));
		return nullptr;
	}
	 
	AccountId* TempAccountId = GetAccountId(Username);
	if (TempAccountId == nullptr)
	{
		return nullptr;
	}

	ILoginSession* TempLoginSession = &MyVoiceClient->GetLoginSession(*TempAccountId);
	if (TempLoginSession == nullptr)
	{
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("Login session not found or not valid"));
		return nullptr;
	}
	
	return TempLoginSession;
}

ChannelId* UVivoxSubsystem::GetChannelId(FString Channelname)
{
	if (!ChannelIds.Contains(Channelname))
	{
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("channel id not found"));
		return nullptr;
	}
	return &ChannelIds[Channelname];
}

IChannelSession* UVivoxSubsystem::GetChannelSession(ILoginSession* TempLoginSession, FString Channelname)
{
	if (Channelname.IsEmpty() || TempLoginSession == nullptr)
	{
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("enter channelname, or valid login session"));
		return nullptr;
	}
	
	ChannelId* TempChannelId = GetChannelId(Channelname);
	if (TempChannelId == nullptr)
	{
		return nullptr;
	}

	IChannelSession* TempChannelSession = &TempLoginSession->GetChannelSession(*TempChannelId);
	if (TempChannelSession == nullptr)
	{
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("Channel session not found or not valid"));
		return nullptr;
	}
	return TempChannelSession;
}

void UVivoxSubsystem::OnLoginSessionStateChanged(LoginState State, FString Username)
{
	if (OnLoginStateChanged.IsBound())
	{
		OnLoginStateChanged.Broadcast(Username, State);
	}
}

void UVivoxSubsystem::OnChannelSessionStateChanged(const IChannelConnectionState& State, FString Username,
	FString ChannelName)
{
	if (OnChannelStateChanged.IsBound())
	{
		OnChannelStateChanged.Broadcast(Username, ChannelName, State.State());
	}
}

void UVivoxSubsystem::OnChannelSessionTextMessageReceived(const IChannelTextMessage& TextMessage, FString Username,
	FString ChannelName)
{
	if (OnChannelTextMessageReceived.IsBound())
	{
		OnChannelTextMessageReceived.Broadcast(Username, ChannelName, TextMessage.Message(), TextMessage.Sender().Name(), TextMessage.ReceivedTime());
	}
}

void UVivoxSubsystem::OnDirectedTextMessageReceived(const IDirectedTextMessage& TextMessage, FString Username)
{
	if (OnDirectTextMessageReceived.IsBound())
	{
		OnDirectTextMessageReceived.Broadcast(Username, TextMessage.Message(), TextMessage.Sender().Name(), TextMessage.ReceivedTime());
	}
}

bool UVivoxSubsystem::IsModuleLoaded()
{
	if (VivoxCoreModule == nullptr)
	{
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("Vivox core module not loadded"));
		return false;
	}
	return true;
}

int32 UVivoxSubsystem::Initilize(FString InputEndPoint, FString InputDomain, FString InputIssuer, FString InputSecretKey)
{
	if ( !IsModuleLoaded())
	{
		return false;
	}

	if ( InputEndPoint.IsEmpty() || InputDomain.IsEmpty() || InputIssuer.IsEmpty() || InputSecretKey.IsEmpty() )
	{
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("Make sure EndPoint and Domain and Issuer and SecretKey is set")); 
		return false;
	}

	EndPoint = InputEndPoint;
	Domain = InputDomain;
	Issuer = InputIssuer;
	SecretKey = InputSecretKey;
	
	MyVoiceClient = &VivoxCoreModule->VoiceClient();
	return MyVoiceClient->Initialize();
}

bool UVivoxSubsystem::IsInitilized()
{
	if ( !IsModuleLoaded())
	{
		return false;
	}

	if (MyVoiceClient == nullptr)
	{
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("ino vivox not initilized yet"));
		return false;
	}
	
	return true;
}

int32 UVivoxSubsystem::SignIn(FString InputUsername, FString InputDisplayName, TArray<FString> InputSpokenLanguages, FTimespan Expiration, bool BindToReceiveDirectMessage, const F_IV_D_OnBeginLoginCompleted& Callback)
{
	
	if ( InputUsername.IsEmpty() )
	{
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("make sure username is set")); 
		return 99;
	}

	if ( InputDisplayName.IsEmpty() )
	{
		InputDisplayName = InputUsername;
	}

	AccountId TempAccountId;
	if ( InputSpokenLanguages.Max() > 0)
	{
		TempAccountId = AccountId(*Issuer, *InputUsername, *Domain, InputDisplayName, InputSpokenLanguages);
	}else
	{
		TempAccountId = AccountId(*Issuer, *InputUsername, *Domain, InputDisplayName);
	}

	if ( !TempAccountId.IsValid())
	{
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("AccountId id not valid, try again and make sure vivox cridentials and input is good"));
		return 98;
	}

	AccountIds.Add(InputUsername, TempAccountId);

	ILoginSession &MyLoginSession(MyVoiceClient->GetLoginSession(TempAccountId));
	MyLoginSession.EventStateChanged.AddUObject(this, &UVivoxSubsystem::OnLoginSessionStateChanged, TempAccountId.Name());
	
	AsyncTask(IV_Thread_Normal, [this, TempAccountId, Expiration, BindToReceiveDirectMessage, Callback]()
	{
		ILoginSession &MyLoginSession(MyVoiceClient->GetLoginSession(TempAccountId));
		FString VivoxLoginToken = MyLoginSession.GetLoginToken(SecretKey, Expiration);
		
		ILoginSession::FOnBeginLoginCompletedDelegate OnBeginLoginCompleted;

		OnBeginLoginCompleted.BindLambda([this, Callback, &MyLoginSession](VivoxCoreError Error)
		{
			UE_LOG(LogVivoxSubsystem, Warning, TEXT("logged in successfully"));
			Callback.ExecuteIfBound(Error);
		});
		
		if (BindToReceiveDirectMessage)
		{
			MyLoginSession.EventDirectedTextMessageReceived.AddUObject(this, &UVivoxSubsystem::OnDirectedTextMessageReceived, TempAccountId.Name());
			//MyLoginSession.EventSendDirectedTextMessageFailed
		}
		
		MyLoginSession.BeginLogin(EndPoint, VivoxLoginToken, OnBeginLoginCompleted);
	});
	
	return 0; 
}

bool UVivoxSubsystem::GetLoginState(FString InputUsername, LoginState& LoginState)
{
	if (InputUsername.IsEmpty())
	{
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("please enter username"));
		return false;
	}
	ILoginSession* TempLoginSession = GetLoginSession(InputUsername);
	if (TempLoginSession == nullptr){
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("user its not valid"));
		return false;
	}
	
	LoginState = TempLoginSession->State();
	return true;
}

bool UVivoxSubsystem::SignOut(FString InputUsername)
{
	if (InputUsername.IsEmpty())
	{
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("please enter username"));
		return false;
	}
	ILoginSession* TempLoginSession = GetLoginSession(InputUsername);
	if (TempLoginSession == nullptr){
		return false;
	}
	
	if (TempLoginSession->State() != LoginState::LoggedIn){
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("user its not logged in"));
		return false;
	}
	
	TempLoginSession->Logout();
	return true;
}

int32 UVivoxSubsystem::JoinChannel(FString InputUserName, FString InputChannelName, ChannelType InputChannelType, FInoChannel3DProperties InputChannel3dProperties, bool ConnectAudio, bool ConnectText, bool SwitchTransmition, FTimespan Expiration, const F_IV_D_OnBeginLoginCompleted& Callback)
{
	if (InputUserName.IsEmpty() || InputChannelName.IsEmpty())
	{
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("make sure user name and channel name is set")); 
		return 99;
	}

	Channel3DProperties TempChannel3DProperties(InputChannel3dProperties.audibleDistance, InputChannel3dProperties.conversationalDistance, InputChannel3dProperties.audioFadeIntensityByDistance, InputChannel3dProperties.audioFadeMode);
	
	ChannelId TempChannelId(Issuer, InputChannelName, Domain, InputChannelType, TempChannel3DProperties);
	if (!TempChannelId.IsValid())
	{
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("ChannelId id not valid, try again and make sure vivox cridentials and input is good"));
		return 98;
	}

	ChannelIds.Add(InputChannelName, TempChannelId);

	ILoginSession* TempLoginSession = GetLoginSession(InputUserName);
	if (TempLoginSession == nullptr){
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("user its not valid"));
		return false;
	}
	
	if (TempLoginSession->State() != LoginState::LoggedIn){
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("user its not logged in"));
		return false;
	}
	
	IChannelSession& MyChannelSession(TempLoginSession->GetChannelSession(TempChannelId));
	MyChannelSession.EventChannelStateChanged.AddUObject(this, &UVivoxSubsystem::OnChannelSessionStateChanged, TempLoginSession->LoginSessionId().Name(),TempChannelId.Name());
	if (ConnectText)
	{
		MyChannelSession.EventTextMessageReceived.AddUObject(this, &UVivoxSubsystem::OnChannelSessionTextMessageReceived, TempLoginSession->LoginSessionId().Name(), TempChannelId.Name());
	}
	
	AsyncTask(IV_Thread_Normal, [this, TempLoginSession, TempChannelId, Expiration, ConnectAudio, ConnectText, SwitchTransmition, Callback]()
	{
		IChannelSession& MyChannelSession(TempLoginSession->GetChannelSession(TempChannelId));
		
		FString ConnectToken = MyChannelSession.GetConnectToken(EndPoint, Expiration);
		IChannelSession::FOnBeginConnectCompletedDelegate OnBeginConnectCompleted;
		
		OnBeginConnectCompleted.BindLambda([this, Callback, &MyChannelSession](VivoxCoreError Error)
		{
			UE_LOG(LogVivoxSubsystem, Warning, TEXT("Join channel completed"));
			Callback.ExecuteIfBound(Error);
		});
		
		MyChannelSession.BeginConnect(ConnectAudio, ConnectText, SwitchTransmition, ConnectToken, OnBeginConnectCompleted);
	});

	return 0;
}

bool UVivoxSubsystem::GetChannelState(FString InputUsername, FString InputChannelName,
	ConnectionState& ChannelConnectionState)
{
	if (InputUsername.IsEmpty() || InputChannelName.IsEmpty())
	{
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("please enter username"));
		return false;
	}

	ILoginSession* TempLoginSession = GetLoginSession(InputUsername);
	if (TempLoginSession == nullptr){
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("user its not valid"));
		return false;
	}
	
	if (TempLoginSession->State() != LoginState::LoggedIn){
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("user its not logged in"));
		return false;
	}
	
	IChannelSession* TempChannelSession = GetChannelSession(TempLoginSession, InputChannelName);

	if (TempChannelSession == nullptr)
	{
		return false;
	}
	
	if (!TempChannelSession->Channel().IsValid())
	{
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("channel not valid"));
		return false;
	}

	ChannelConnectionState = TempChannelSession->ChannelState();
	return true;
}

bool UVivoxSubsystem::GetChannelType(FString InputUsername, FString InputChannelName, ChannelType& ChannelType)
{
	if (InputUsername.IsEmpty() || InputChannelName.IsEmpty())
	{
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("please enter username"));
		return false;
	}

	ILoginSession* TempLoginSession = GetLoginSession(InputUsername);
	if (TempLoginSession == nullptr){
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("user its not valid"));
		return false;
	}
	
	if (TempLoginSession->State() != LoginState::LoggedIn){
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("user its not logged in"));
		return false;
	}
	
	IChannelSession* TempChannelSession = GetChannelSession(TempLoginSession, InputChannelName);

	if (TempChannelSession == nullptr)
	{
		return false;
	}
	
	if (!TempChannelSession->Channel().IsValid())
	{
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("channel not valid"));
		return false;
	}

	ChannelType = TempChannelSession->Channel().Type();
	return true;
}

bool UVivoxSubsystem::LeaveChannel(FString InputUsername, FString InputChannelName, bool DeleteOnDisconnect)
{
	if (InputUsername.IsEmpty() || InputChannelName.IsEmpty())
	{
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("please enter username"));
		return false;
	}

	ILoginSession* TempLoginSession = GetLoginSession(InputUsername);
	if (TempLoginSession == nullptr){
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("user its not valid"));
		return false;
	}
	
	if (TempLoginSession->State() != LoginState::LoggedIn){
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("user its not logged in"));
		return false;
	}
	
	IChannelSession* TempChannelSession = GetChannelSession(TempLoginSession, InputChannelName);

	if (TempChannelSession == nullptr)
	{
		return false;
	}
	
	if (TempChannelSession->ChannelState() != ConnectionState::Connected)
	{
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("channel not connected"));
		return false;
	}
	TempChannelSession->Disconnect(DeleteOnDisconnect);
	return true;
}

bool UVivoxSubsystem::GetTransmissionMode( FString InputUsername, TransmissionMode& TransmissionMode )
{
	if (InputUsername.IsEmpty())
	{
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("please enter username"));
		return false;
	}

	ILoginSession* TempLoginSession = GetLoginSession(InputUsername);
	if (TempLoginSession == nullptr){
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("user its not valid"));
		return false;
	}
	
	if (TempLoginSession->State() != LoginState::LoggedIn){
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("user its not logged in"));
		return false;
	}
	
	TransmissionMode = TempLoginSession->GetTransmissionMode();
	return true;
}

bool UVivoxSubsystem::GetTransmittingChannels(FString InputUsername, TArray<FString>& TransmittingChannels)
{
	if (InputUsername.IsEmpty())
	{
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("please enter username"));
		return false;
	}

	ILoginSession* TempLoginSession = GetLoginSession(InputUsername);
	if (TempLoginSession == nullptr){
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("user its not valid"));
		return false;
	}
	
	if (TempLoginSession->State() != LoginState::LoggedIn){
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("user its not logged in"));
		return false;
	}

	TArray<ChannelId> TempChannelIds = TempLoginSession->GetTransmittingChannels();
	for (auto i : TempChannelIds) {
		TransmittingChannels.Add(i.Name());
	}
	return true;
	
}

int32 UVivoxSubsystem::SetTransmissionMode(FString InputUsername, FString InputChannelName,
	TransmissionMode TransmissionMode)
{
	if (InputUsername.IsEmpty())
	{
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("please enter username and channelname"));
		return 99;
	}

	ILoginSession* TempLoginSession = GetLoginSession(InputUsername);
	if (TempLoginSession == nullptr){
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("user its not valid"));
		return false;
	}
	
	if (TempLoginSession->State() != LoginState::LoggedIn){
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("user its not logged in"));
		return false;
	}

	if (TransmissionMode == TransmissionMode::Single)
	{
		ChannelId* TempChannelId = GetChannelId(InputChannelName);
		if (TempChannelId == nullptr)
		{
			return 99;
		}
		if (!TempChannelId->IsValid())
		{
			return 98;
		}
	
		return TempLoginSession->SetTransmissionMode(TransmissionMode, *TempChannelId);
	}
	
	return TempLoginSession->SetTransmissionMode(TransmissionMode);
}

int32 UVivoxSubsystem::SendChannelText(FString InputUsername, FString InputChannelName, FString InputText, const F_IV_D_OnBeginLoginCompleted& Callback)
{
	if (InputUsername.IsEmpty())
	{
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("please enter username and channelname"));
		return 99;
	}

	ILoginSession* TempLoginSession = GetLoginSession(InputUsername);
	if (TempLoginSession == nullptr){
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("user its not valid"));
		return false;
	}
	
	if (TempLoginSession->State() != LoginState::LoggedIn){
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("user its not logged in"));
		return false;
	}

	IChannelSession* TempChannelSession = GetChannelSession(TempLoginSession, InputChannelName);
	if (TempChannelSession == nullptr){
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("channel its not valid"));
		return false;
	}
	
	AsyncTask(IV_Thread_Normal, [this, TempChannelSession, InputText, Callback]()
	{
		IChannelSession::FOnBeginSendTextCompletedDelegate SendChannelMessageCallback;

		SendChannelMessageCallback.BindLambda([this, Callback](VivoxCoreError Error)
		{
			UE_LOG(LogVivoxSubsystem, Warning, TEXT("Send text completed"));
			Callback.ExecuteIfBound(Error);
		});
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("Send text started"));
		TempChannelSession->BeginSendText(InputText, SendChannelMessageCallback);
	});
	
	return 0;
}

int32 UVivoxSubsystem::SendDirectText(FString SenderUsername, FString RecieverUsername,  FString InputText,
	const F_IV_D_OnBeginLoginCompleted& Callback)
{
	if (SenderUsername.IsEmpty() || RecieverUsername.IsEmpty())
	{
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("please enter username"));
		return 99;
	}

	ILoginSession* TempLoginSession = GetLoginSession(SenderUsername);
	if (TempLoginSession == nullptr){
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("user its not valid"));
		return false;
	}
	
	if (TempLoginSession->State() != LoginState::LoggedIn){
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("user its not logged in"));
		return false;
	}
	
	AccountId Target = AccountId(Issuer, RecieverUsername, Domain);
	if (!Target.IsValid())
	{
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("receiver user its not valid"));
		return 97;
	}
	
	ILoginSession::FOnBeginSendDirectedMessageCompletedDelegate SendDirectedMessageCallback;
	SendDirectedMessageCallback.BindLambda([this, Target, InputText](VivoxCoreError Error, FString Text)
	{
		UE_LOG(LogVivoxSubsystem, Log, TEXT("Message sent to %s: %s\n"), *Target.Name(), *InputText);
	});
	
	return TempLoginSession->BeginSendDirectedMessage(Target, InputText, SendDirectedMessageCallback);
}

int32 UVivoxSubsystem::Update3dPositionalChannel(FString InputUsername, FString InputChannelName,
	FVector SpeakerPosition, FVector ListenerPosition, FVector ListenerForwardVector, FVector ListenerUpVector, bool& Updated)
{
	if (InputUsername.IsEmpty() || InputChannelName.IsEmpty())
	{
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("please enter username and channelname"));
		return 99;
	}
	
	ILoginSession* TempLoginSession = GetLoginSession(InputUsername);
	if (TempLoginSession == nullptr){
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("user its not valid"));
		return 98;
	}
	
	if (TempLoginSession->State() != LoginState::LoggedIn){
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("user its not logged in"));
		return 97;
	}

	IChannelSession* TempChannelSession = GetChannelSession(TempLoginSession, InputChannelName);

	if (TempChannelSession == nullptr)
	{
		return 96;
	}
	
	if (TempChannelSession->Channel().Type() != ChannelType::Positional)
	{
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("selected channel is not positional"));
		return 95;
	}
	
	if ( TempSpeakerPosition == SpeakerPosition && TempListenerPosition == ListenerPosition && TempListenerForwardVector == ListenerForwardVector && TempListenerUpVector == ListenerUpVector)
	{
		UE_LOG(LogVivoxSubsystem, Warning, TEXT("dont need update, its the same value"));
		Updated = false;
		return 0;
	}
	
	Updated = true;
	return TempChannelSession->Set3DPosition(SpeakerPosition, ListenerPosition, ListenerForwardVector, ListenerUpVector);
}