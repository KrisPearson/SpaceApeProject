#pragma once

#include "CoreMinimal.h"
#include "TeamOwnerEnum.generated.h"


UENUM(BlueprintType)
namespace TeamOwner {
	//256 entries max
	enum ETeamOwner {
		TO_NoOwner     UMETA(DisplayName = "NoTeam"),
		TO_Player    UMETA(DisplayName = "Player"),
		TO_FriendlyNPC         UMETA(DisplayName = "NPC Friendly"),
		TO_EnemyNPC        UMETA(DisplayName = "NPC Enemy"),

		//~~~

		//256th entry
		TO_Max        UMETA(Hidden)
	};
}