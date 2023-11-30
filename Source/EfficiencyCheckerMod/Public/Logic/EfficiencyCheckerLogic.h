﻿#pragma once

#include <map>

#include "ComponentFilter.h"
#include "FGFactoryConnectionComponent.h"
#include "FGPipeConnectionComponent.h"
#include "Internationalization/Regex.h"
#include "Resources/FGItemDescriptor.h"

#include "EfficiencyCheckerLogic.generated.h"

UCLASS()
class EFFICIENCYCHECKERMOD_API AEfficiencyCheckerLogic : public AActor
{
	GENERATED_BODY()

public:
	static const FRegexPattern indexPattern;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, Category="EfficiencyCheckerLogic")
	virtual void Initialize
	(
		UPARAM(DisplayName = "None Item Descriptor") const TSet<TSubclassOf<class UFGItemDescriptor>>& in_noneItemDescriptors,
		UPARAM(DisplayName = "Wildcard Item Descriptor") const TSet<TSubclassOf<class UFGItemDescriptor>>& in_wildcardItemDescriptors,
		UPARAM(DisplayName = "Any Undefined Item Descriptor") const TSet<TSubclassOf<class UFGItemDescriptor>>& in_anyUndefinedItemDescriptors,
		UPARAM(DisplayName = "Overflow Item Descriptor") const TSet<TSubclassOf<class UFGItemDescriptor>>& in_overflowItemDescriptors,
		UPARAM(DisplayName = "Nuclear Waste Item Descriptor") const TSet<TSubclassOf<class UFGItemDescriptor>>& in_nuclearWasteItemDescriptors
	);

	UFUNCTION(BlueprintCallable, Category="EfficiencyCheckerLogic")
	virtual void Terminate();

	UFUNCTION(BlueprintCallable, Category="EfficiencyCheckerLogic")
	static void DumpInformation(AActor* worldContext, TSubclassOf<UFGItemDescriptor> equipmentDescriptor);

	UFUNCTION(BlueprintCallable, Category="EfficiencyCheckerLogic")
	virtual bool IsValidBuildable(class AFGBuildable* newBuildable);

	static void collectInput
	(
		EResourceForm resourceForm,
		bool customInjectedInput,
		class UFGConnectionComponent* connector,
		float& out_injectedInput,
		float& out_limitedThroughput,
		TMap<AActor*, TSet<TSubclassOf<UFGItemDescriptor>>>& seenActors,
		TSet<class AFGBuildable*>& connected,
		TSet<TSubclassOf<UFGItemDescriptor>>& out_injectedItems,
		const TSet<TSubclassOf<UFGItemDescriptor>>& restrictItems,
		class AFGBuildableSubsystem* buildableSubsystem,
		int level,
		bool& overflow,
		const FString& indent,
		const time_t& timeout,
		int32 machineStatusIncludeType
	);

	static void collectOutput
	(
		EResourceForm resourceForm,
		class UFGConnectionComponent* connector,
		float& out_requiredOutput,
		float& out_limitedThroughput,
		TMap<AActor*, TSet<TSubclassOf<class UFGItemDescriptor>>>& seenActors,
		TSet<AFGBuildable*>& connected,
		const TSet<TSubclassOf<class UFGItemDescriptor>>& in_injectedItems,
		class AFGBuildableSubsystem* buildableSubsystem,
		int level,
		bool& overflow,
		const FString& indent,
		const time_t& timeout,
		int32 machineStatusIncludeType
	);

	static bool containsActor(const TMap<AActor*, TSet<TSubclassOf<UFGItemDescriptor>>>& seenActors, AActor* actor);
	static bool actorContainsItem(const TMap<AActor*, TSet<TSubclassOf<UFGItemDescriptor>>>& seenActors, AActor* actor, const TSubclassOf<UFGItemDescriptor>& item);
	static void addAllItemsToActor(TMap<AActor*, TSet<TSubclassOf<UFGItemDescriptor>>>& seenActors, AActor* actor, const TSet<TSubclassOf<UFGItemDescriptor>>& items);

	static bool inheritsFrom(AActor* owner, const FString& className);
	static void dumpUnknownClass(const FString& indent, AActor* owner);

	static float getPipeSpeed(class AFGBuildablePipeline* pipe);

	TSet<TSubclassOf<class UFGItemDescriptor>> nuclearWasteItemDescriptors;
	TSet<TSubclassOf<class UFGItemDescriptor>> noneItemDescriptors;
	TSet<TSubclassOf<class UFGItemDescriptor>> wildCardItemDescriptors;
	TSet<TSubclassOf<class UFGItemDescriptor>> anyUndefinedItemDescriptors;
	TSet<TSubclassOf<class UFGItemDescriptor>> overflowItemDescriptors;

	FCriticalSection eclCritical;

	static AEfficiencyCheckerLogic* singleton;
	UClass* baseStorageTeleporterClass = nullptr;
	UClass* baseUndergroundSplitterInputClass = nullptr;
	UClass* baseUndergroundSplitterOutputClass = nullptr;
	UClass* baseModularLoadBalancerClass = nullptr;
	UClass* baseBuildableFactorySimpleProducerClass = nullptr;

	TSet<class AEfficiencyCheckerBuilding*> allEfficiencyBuildings;
	TSet<class AFGBuildableConveyorBelt*> allBelts;
	TSet<class AFGBuildablePipeline*> allPipes;
	TSet<class AFGBuildableFactory*> allTeleporters;
	TSet<class AFGBuildableStorage*> allUndergroundInputBelts;

	FActorEndPlaySignature::FDelegate removeEffiencyBuildingDelegate;
	FActorEndPlaySignature::FDelegate removeBeltDelegate;
	FActorEndPlaySignature::FDelegate removePipeDelegate;
	FActorEndPlaySignature::FDelegate removeTeleporterDelegate;
	FActorEndPlaySignature::FDelegate removeUndergroundInputBeltDelegate;

	virtual void addEfficiencyBuilding(class AEfficiencyCheckerBuilding* actor);
	virtual void addBelt(class AFGBuildableConveyorBelt* actor);
	virtual void addPipe(class AFGBuildablePipeline* actor);
	virtual void addTeleporter(class AFGBuildableFactory* actor);
	virtual void addUndergroundInputBelt(class AFGBuildableStorage* actor);

	UFUNCTION()
	virtual void removeEfficiencyBuilding(AActor* actor, EEndPlayReason::Type reason);
	UFUNCTION()
	virtual void removeBelt(AActor* actor, EEndPlayReason::Type reason);
	UFUNCTION()
	virtual void removePipe(AActor* actor, EEndPlayReason::Type reason);
	UFUNCTION()
	virtual void removeTeleporter(AActor* teleporter, EEndPlayReason::Type reason);
	UFUNCTION()
	virtual void removeUndergroundInputBelt(AActor* undergroundInputBelt, EEndPlayReason::Type reason);

	static EPipeConnectionType GetConnectedPipeConnectionType(class UFGPipeConnectionComponent* component);

	static void collectUndergroundBeltsComponents
	(
		class AFGBuildableStorage* undergroundBelt,
		TSet<class UFGFactoryConnectionComponent*>& components,
		TSet<AActor*>& actors
	);

	static void collectModularLoadBalancerComponents
	(
		AFGBuildableFactory* modularLoadBalancer,
		TSet<UFGFactoryConnectionComponent*>& components,
		TSet<AActor*>& actors
	);

	static void collectSmartSplitterComponents
	(
		class UFGConnectionComponent* connector,
		const FComponentFilter& currentFilter,
		class AFGBuildableSplitterSmart* smartSplitter,
		TMap<UFGFactoryConnectionComponent*, FComponentFilter>& connectedInputs,
		TMap<UFGFactoryConnectionComponent*, FComponentFilter>& componentOutputs,
		EFactoryConnectionDirection direction,
		const FString& indent,
		const time_t& timeout,
		bool& overflow
	);
};
