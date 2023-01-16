/* Copyright (c) 2021-2023 by Inoland */

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#if !DISABLE_VIVOXCORE
	#include "VivoxCore.h"
#endif

#include "Library/VivoxStructLibrary.h"

#include "VivoxSubsystem.generated.h"


//BP Dynamic Deligates ----------------------------------------
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(F_IV_M_OnLoginStateChanged, FString, Username, LoginState, LoginState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(F_IV_M_OnChannelStateChanged, FString, Username, FString, ChannelName, ConnectionState, ChannelState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(F_IV_M_OnChannelTextMessageReceived, FString, Username, FString, ChannelName, FString, Text, FString, Sender, FDateTime, RecieveTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(F_IV_M_OnDirectTextMessageReceived, FString, Username, FString, Text, FString, Sender, FDateTime, RecieveTime);

DECLARE_DYNAMIC_DELEGATE_OneParam(F_IV_D_OnBeginLoginCompleted, int32, VivoxError);
//BP Dynamic Deligates ----------------------------------------

UCLASS(DisplayName = "Vivox Subsystem", category="Vivox Subsystem")
class UVivoxSubsystem : public UGameInstanceSubsystem
{
 GENERATED_BODY()
public:
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:

#if !DISABLE_VIVOXCORE
	FVivoxCoreModule *VivoxCoreModule = nullptr;
	IClient *MyVoiceClient = nullptr;
	
	TMap<FString, AccountId> AccountIds;
	TMap<FString, ChannelId> ChannelIds;
	
	FString EndPoint;
	FString Domain;
	FString Issuer;
	FString SecretKey;
	
	VivoxCoreError GetError(int32 ErrorInt);

	AccountId* GetAccountId(FString Username);
	ILoginSession* GetLoginSession(FString Username);

	ChannelId* GetChannelId(FString Channelname);
	IChannelSession* GetChannelSession (ILoginSession* TempLoginSession, FString Channelname);

	void OnLoginSessionStateChanged(LoginState State, FString Username);
	void OnChannelSessionStateChanged(const IChannelConnectionState& State, FString Username, FString ChannelName);

	void OnChannelSessionTextMessageReceived(const IChannelTextMessage& TextMessage, FString Username, FString ChannelName);
	void OnDirectedTextMessageReceived(const IDirectedTextMessage& TextMessage, FString Username);


	//3d positional channel system
	FVector TempSpeakerPosition;
	FVector TempListenerPosition;
	FVector TempListenerForwardVector;
	FVector TempListenerUpVector;
#endif
	
public:

	UPROPERTY(BlueprintAssignable, DisplayName = "On Login State Changed", category="Vivox Subsystem|Account|Deligates")
	F_IV_M_OnLoginStateChanged OnLoginStateChanged;

	UPROPERTY(BlueprintAssignable, DisplayName = "On Channel State Changed", category="Vivox Subsystem|Channel|Deligates")
	F_IV_M_OnChannelStateChanged OnChannelStateChanged;

	UPROPERTY(BlueprintAssignable, DisplayName = "On Channel Text Message Received", category="Vivox Subsystem|Channel|Deligates")
	F_IV_M_OnChannelTextMessageReceived OnChannelTextMessageReceived;

	UPROPERTY(BlueprintAssignable, DisplayName = "On Direct Text Message Received", category="Vivox Subsystem|Account|Deligates")
	F_IV_M_OnDirectTextMessageReceived OnDirectTextMessageReceived;


	
	UFUNCTION(BlueprintPure, DisplayName = "Is Module Loaded", category="Vivox Subsystem|Base")
		bool IsModuleLoaded();
	
	UFUNCTION(BlueprintCallable, DisplayName = "Initilize", category="Vivox Subsystem|Base")
		int32 Initilize( FString InputEndPoint, FString InputDomain, FString InputIssuer, FString InputSecretKey );

	UFUNCTION(BlueprintPure, DisplayName = "Is Initilized", category="Vivox Subsystem|Base")
		bool IsInitilized();
	
	UFUNCTION(BlueprintCallable, DisplayName = "Sign In", category="Vivox Subsystem|Account", meta = (AutoCreateRefTerm = "Callback"))
		int32 SignIn( FString InputUsername, FString InputDisplayName, TArray<FString> InputSpokenLanguages, FTimespan Expiration, bool BindToReceiveDirectMessage, const F_IV_D_OnBeginLoginCompleted& Callback);

	UFUNCTION(BlueprintPure, DisplayName = "Get Login State", category="Vivox Subsystem|Account")
		bool GetLoginState( FString InputUsername, LoginState& LoginState);
	
	UFUNCTION(BlueprintCallable, DisplayName = "Sign Out", category="Vivox Subsystem|Account")
		bool SignOut( FString InputUsername);

	UFUNCTION(BlueprintCallable, DisplayName = "Join Channel", category="Vivox Subsystem|Channel", meta = (AutoCreateRefTerm = "Callback"))
		int32 JoinChannel( FString InputUserName, FString InputChannelName, ChannelType InputChannelType, FInoChannel3DProperties InputChannel3dProperties, bool ConnectAudio, bool ConnectText, bool SwitchTransmition, FTimespan Expiration, const F_IV_D_OnBeginLoginCompleted& Callback);

	UFUNCTION(BlueprintPure, DisplayName = "Get Channel State", category="Vivox Subsystem|Channel")
		bool GetChannelState( FString InputUsername, FString InputChannelName, ConnectionState& ChannelState);

	UFUNCTION(BlueprintPure, DisplayName = "Get Channel Type", category="Vivox Subsystem|Channel")
		bool GetChannelType( FString InputUsername, FString InputChannelName, ChannelType& ChannelType);
	
	UFUNCTION(BlueprintCallable, DisplayName = "Leave Channel", category="Vivox Subsystem|Channel")
		bool LeaveChannel( FString InputUsername, FString InputChannelName, bool DeleteOnDisconnect);

	UFUNCTION(BlueprintPure, DisplayName = "Get Transmission Mode", category="Vivox Subsystem|Channel")
		bool GetTransmissionMode( FString InputUsername, TransmissionMode& TransmissionMode );

	UFUNCTION(BlueprintPure, DisplayName = "Get Transmitting Channels", category="Vivox Subsystem|Channel")
		bool GetTransmittingChannels( FString InputUsername, TArray<FString>& TransmittingChannels );

	UFUNCTION(BlueprintCallable, DisplayName = "Set Transmission Mode", category="Vivox Subsystem|Channel")
		int32 SetTransmissionMode( FString InputUsername, FString InputChannelName, TransmissionMode TransmissionMode );

	UFUNCTION(BlueprintCallable, DisplayName = "Send Channel Text Message", category="Vivox Subsystem|Channel", meta = (AutoCreateRefTerm = "Callback"))
		int32 SendChannelText( FString InputUsername, FString InputChannelName, FString InputText , const F_IV_D_OnBeginLoginCompleted& Callback);

	UFUNCTION(BlueprintCallable, DisplayName = "Send Direct Text Message", category="Vivox Subsystem|Account", meta = (AutoCreateRefTerm = "Callback"))
	int32 SendDirectText( FString SenderUsername, FString RecieverUsername,  FString InputText,
const F_IV_D_OnBeginLoginCompleted& Callback);

	UFUNCTION(BlueprintCallable, DisplayName = "Update 3D Positional Channel", category="Vivox Subsystem|Channel")
	int32 Update3dPositionalChannel( FString InputUsername, FString InputChannelName,
	FVector SpeakerPosition, FVector ListenerPosition, FVector ListenerForwardVector, FVector ListenerUpVector, bool& Updated );
};

