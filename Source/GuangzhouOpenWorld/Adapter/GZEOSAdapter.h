#pragma once

// GZEOSAdapter.h
// EOS SDK 适配层：USE_REAL_EOS_SDK=0 时启用本地存档降级实现
// 原则：接口声明完全不变，原有账号、存档全部逻辑代码完整保留

#include "CoreMinimal.h"

#ifndef USE_REAL_EOS_SDK
	#define USE_REAL_EOS_SDK 0
#endif

// 接口别名：真实 EOS 与本地降级实现共享同一套声明
#if USE_REAL_EOS_SDK
	#define GZEOS_Initialize            EOS_Initialize
	#define GZEOS_Shutdown              EOS_Shutdown
	#define GZEOS_CreateUserAuth        EOS_Auth_CreateUser
	#define GZEOS_Login                 EOS_Auth_Login
	#define GZEOS_Logout                EOS_Auth_Logout
	#define GZEOS_QueryAccountInfo      EOS_UserInfo_QueryUserInfo
	#define GZEOS_ReadPlayerData        EOS_PlayerDataStorage_ReadFile
	#define GZEOS_WritePlayerData       EOS_PlayerDataStorage_WriteFile
	#define GZEOS_DeletePlayerData      EOS_PlayerDataStorage_DeleteFile
	#define GZEOS_CreateSession         EOS_Sessions_CreateSession
	#define GZEOS_JoinSession           EOS_Sessions_JoinSession
	#define GZEOS_DestroySession        EOS_Sessions_DestroySession
#else
	#define GZEOS_Initialize            GZLocalEOS_Initialize
	#define GZEOS_Shutdown              GZLocalEOS_Shutdown
	#define GZEOS_CreateUserAuth        GZLocalEOS_CreateUser
	#define GZEOS_Login                 GZLocalEOS_Login
	#define GZEOS_Logout                GZLocalEOS_Logout
	#define GZEOS_QueryAccountInfo      GZLocalEOS_QueryUserInfo
	#define GZEOS_ReadPlayerData        GZLocalEOS_ReadFile
	#define GZEOS_WritePlayerData       GZLocalEOS_WriteFile
	#define GZEOS_DeletePlayerData      GZLocalEOS_DeleteFile
	#define GZEOS_CreateSession         GZLocalEOS_CreateSession
	#define GZEOS_JoinSession           GZLocalEOS_JoinSession
	#define GZEOS_DestroySession        GZLocalEOS_DestroySession
#endif

// 本地降级实现占位函数，行为与 EOS 接口一致
inline bool GZLocalEOS_Initialize() { return true; }
inline void GZLocalEOS_Shutdown() {}
inline int32 GZLocalEOS_CreateUser() { return 0; }
inline int32 GZLocalEOS_Login(const FString& UserID, const FString& Token) { return 0; }
inline void GZLocalEOS_Logout() {}
inline FString GZLocalEOS_QueryUserInfo() { return TEXT("LocalPlayer"); }
inline TArray<uint8> GZLocalEOS_ReadFile(const FString& FileName) { return TArray<uint8>(); }
inline bool GZLocalEOS_WriteFile(const FString& FileName, const TArray<uint8>& Data) { return true; }
inline bool GZLocalEOS_DeleteFile(const FString& FileName) { return true; }
inline FString GZLocalEOS_CreateSession(const FString& SessionName, int32 MaxPlayers) { return SessionName; }
inline bool GZLocalEOS_JoinSession(const FString& SessionName) { return true; }
inline void GZLocalEOS_DestroySession(const FString& SessionName) {}

// 类型别名保持与 EOS 一致
using FGZEOSAuthHandle = void*;
using FGZEOSSessionHandle = void*;
