#include "Tools/VoxelToolComponent.h"

#include "Tools/VoxelToolAction.h"

UVoxelToolComponent::UVoxelToolComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UVoxelToolComponent::BeginPlay()
{
	Super::BeginPlay();
	CreateTools();

	const int32 FirstValidIndex = FindNextValidToolIndex(-1, 1);
	if (FirstValidIndex != INDEX_NONE)
	{
		SetActiveToolIndex(FirstValidIndex);
	}
}

bool UVoxelToolComponent::ActivateTool(const FHitResult& HitResult)
{
	UVoxelToolAction* Tool = GetActiveTool();
	return Tool && Tool->ExecuteTool(HitResult);
}

bool UVoxelToolComponent::SelectNextTool()
{
	const int32 NextIndex = FindNextValidToolIndex(ActiveToolIndex, 1);
	return NextIndex != INDEX_NONE && SetActiveToolIndex(NextIndex);
}

bool UVoxelToolComponent::SelectPreviousTool()
{
	const int32 PreviousIndex = FindNextValidToolIndex(ActiveToolIndex, -1);
	return PreviousIndex != INDEX_NONE && SetActiveToolIndex(PreviousIndex);
}

bool UVoxelToolComponent::SetActiveToolIndex(const int32 NewToolIndex)
{
	if (!ToolObjects.IsValidIndex(NewToolIndex) || !ToolObjects[NewToolIndex])
	{
		return false;
	}

	if (ActiveToolIndex == NewToolIndex)
	{
		return true;
	}

	ActiveToolIndex = NewToolIndex;
	OnActiveToolChanged.Broadcast(ActiveToolIndex, ToolConfigurations[ActiveToolIndex]);
	return true;
}

UVoxelToolAction* UVoxelToolComponent::GetActiveTool() const
{
	return ToolObjects.IsValidIndex(ActiveToolIndex) ? ToolObjects[ActiveToolIndex] : nullptr;
}

bool UVoxelToolComponent::GetActiveToolConfig(FVoxelToolConfig& OutToolConfig) const
{
	if (!ToolConfigurations.IsValidIndex(ActiveToolIndex))
	{
		return false;
	}

	OutToolConfig = ToolConfigurations[ActiveToolIndex];
	return true;
}

void UVoxelToolComponent::CreateTools()
{
	ToolObjects.SetNum(ToolConfigurations.Num());

	for (int32 Index = 0; Index < ToolConfigurations.Num(); ++Index)
	{
		const TSubclassOf<UVoxelToolAction> ToolClass = ToolConfigurations[Index].ToolObjectClass;
		if (!ToolClass)
		{
			continue;
		}

		UVoxelToolAction* Tool = NewObject<UVoxelToolAction>(this, ToolClass);
		ToolObjects[Index] = Tool;
		Tool->Initialize(this);
	}
}

int32 UVoxelToolComponent::FindNextValidToolIndex(const int32 StartIndex, const int32 Direction) const
{
	if (ToolObjects.IsEmpty() || Direction == 0)
	{
		return INDEX_NONE;
	}

	int32 Index = StartIndex;
	for (int32 Attempt = 0; Attempt < ToolObjects.Num(); ++Attempt)
	{
		Index = (Index + Direction + ToolObjects.Num()) % ToolObjects.Num();
		if (ToolObjects[Index])
		{
			return Index;
		}
	}

	return INDEX_NONE;
}
