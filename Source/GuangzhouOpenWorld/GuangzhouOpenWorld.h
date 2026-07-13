// GuangzhouOpenWorld.h
// 主模块头文件 — GTA-广州 3A开放世界

#pragma once
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGZWorld, Log, All);

class FGuangzhouOpenWorldModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};