#pragma once

UENUM(BlueprintType)
enum class ConstructionName : uint8
{
    Factory01    UMETA(DisplayName = "Factory01"),
    None         UMETA(DisplayName = "None"),
};

UENUM(BlueprintType)
enum class ConstructionStatus : uint8
{
    Constructing       UMETA(DisplayName = "Constructing"),
    Constructed        UMETA(DisplayName = "Constructed"),
    Unconstructable    UMETA(DisplayName = "Unconstructable"),
    None               UMETA(DisplayName = "None"),
};