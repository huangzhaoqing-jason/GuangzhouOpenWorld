#pragma once

#include "CoreMinimal.h"

#if WITH_EDITOR
#include "Factories/Factory.h"
#include "GZGSAssetFactory.generated.h"

UCLASS()
class GUANGZHOUOPENWORLD_API UGZGSAssetFactory : public UFactory
{
	GENERATED_BODY()

public:
	UGZGSAssetFactory();

	virtual bool FactoryCanImport(const FString& Filename) override;
	virtual UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
		const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled) override;

private:
	bool ParsePLYHeader(const FString& FilePath, int32& OutVertexCount, FBox& OutBounds);
};

#endif
