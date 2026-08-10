#include "Tools/VoxelToolAction.h"

#include "Tools/VoxelToolComponent.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Subsystems/VoxelInventorySubsystem.h"

UWorld* UVoxelToolAction::GetWorld() const
{
	return ToolComponent ? ToolComponent->GetWorld() : nullptr;
}

bool UVoxelToolAction::ExecuteTool_Implementation(const FHitResult& HitResult)
{
	return false;
}

AActor* UVoxelToolAction::GetToolOwner() const
{
	return ToolComponent ? ToolComponent->GetOwner() : nullptr;
}

UVoxelInventorySubsystem* UVoxelToolAction::GetInventory() const
{
	const APawn* Pawn = Cast<APawn>(GetToolOwner());
	const APlayerController* PlayerController = Pawn ? Cast<APlayerController>(Pawn->GetController()) : nullptr;
	ULocalPlayer* LocalPlayer = PlayerController ? PlayerController->GetLocalPlayer() : nullptr;
	return LocalPlayer ? LocalPlayer->GetSubsystem<UVoxelInventorySubsystem>() : nullptr;
}

void UVoxelToolAction::Initialize(UVoxelToolComponent* InToolComponent)
{
	ToolComponent = InToolComponent;
	ReceiveToolInitialized();
}
