// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


namespace SpriteDirection {
	extern FVector Up = FVector(1, 0, 0);
	extern FVector Down = FVector(-1, 0, 0);
	extern FVector Right = FVector(0, 1, 0);
	extern FVector Left = FVector(0, -1, 0);
}

extern float CameraAngle = -60.0f;