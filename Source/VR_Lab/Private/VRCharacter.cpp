// Fill out your copyright notice in the Description page of Project Settings.


#include "VRCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "MotionControllerComponent.h"
#include "XRDeviceVisualizationComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetInteractionComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

DEFINE_LOG_CATEGORY(LogVRCharacter);

// Sets default values
AVRCharacter::AVRCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

		// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the objects we'll need
	VROrigin = CreateDefaultSubobject<USceneComponent>(TEXT("VROrigin"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	LeftMotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("Left Controller"));
	LeftMotionController->SetTrackingSource(EControllerHand::Left);
	LeftControllerVisualization = CreateDefaultSubobject<UXRDeviceVisualizationComponent>(
		TEXT("Left Controller Visualization"));
	LeftControllerVisualization->SetupAttachment(LeftMotionController);

	RightMotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("Right Controller"));
	RightMotionController->SetTrackingSource(EControllerHand::Right);
	RightControllerVisualization = CreateDefaultSubobject<UXRDeviceVisualizationComponent>(
		TEXT("Right Controller Visualization"));
	RightControllerVisualization->SetupAttachment(RightMotionController);

	UWidgetInteractionComponent* LeftWidgetInteractionComponent = CreateDefaultSubobject<UWidgetInteractionComponent>(
		TEXT("Left Widget Interaction"));
	UWidgetInteractionComponent* RightWidgetInteractionComponent = CreateDefaultSubobject<UWidgetInteractionComponent>(
		TEXT("Right Widget Interaction"));

	// Attach all the objects to their locations for a VR Character
	VROrigin->SetupAttachment(GetRootComponent());
	Camera->SetupAttachment(VROrigin);
	LeftMotionController->SetupAttachment(VROrigin);
	RightMotionController->SetupAttachment(VROrigin);
	LeftWidgetInteractionComponent->SetupAttachment(LeftMotionController);
	RightWidgetInteractionComponent->SetupAttachment(RightMotionController);

	LeftHandMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LeftHandMesh"));
	LeftHandMesh->SetupAttachment(LeftMotionController);
	RightHandMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RightHandMesh"));
	RightHandMesh->SetupAttachment(RightMotionController);
	// RightHandMeshSkeleton = ConstructorHelpers::FObjectFinder<USkeletalMesh>(TEXT("SkeletalMesh'/Game/VirtualReality/Mannequin/Character/Mesh/MannequinHand_Right.MannequinHand_Right'")).Object;
	// if (RightHandMeshSkeleton != nullptr)
	// {
	// RightHandMesh->SetSkeletalMesh(RightHandMeshSkeleton);
	// }
	// RightHandMesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	// LeftHandMeshSkeleton = ConstructorHelpers::FObjectFinder<USkeletalMesh>(TEXT("SkeletalMesh'/Game/VirtualReality/Mannequin/Character/Mesh/MannequinHand_Right.MannequinHand_Right'")).Object;
	// if (LeftHandMeshSkeleton != nullptr)
	// {
	// LeftHandMesh->SetSkeletalMesh(LeftHandMeshSkeleton);
	// }
	// AnimBp = ConstructorHelpers::FObjectFinder<UClass>(TEXT("Class'/Game/_Main/Blueprints/Animations/ABP_VRHandAnimInstance.ABP_VRHandAnimInstance_C'")).Object;
	// if (AnimBp != nullptr)
	// {
	// LeftHandMesh->SetAnimInstanceClass(AnimBp);
	// RightHandMesh->SetAnimInstanceClass(AnimBp);
	// }

	LeftHandForwardArrow = CreateDefaultSubobject<UArrowComponent>("LeftHandForwardArrow");
	LeftHandForwardArrow->SetupAttachment(LeftMotionController);
	LeftHandForwardArrow->SetArrowColor(FColor::Red);
	LeftHandForwardArrow->SetArrowSize(0.2f);
	LeftHandForwardArrow->SetArrowLength(40.0f);

	LeftHandForwardGoArrow = CreateDefaultSubobject<UArrowComponent>("LeftHandForwardGoArrow");
	LeftHandForwardGoArrow->SetupAttachment(LeftMotionController);
	LeftHandForwardGoArrow->SetArrowColor(FColor::Cyan);
	LeftHandForwardGoArrow->SetArrowSize(0.2f);
	LeftHandForwardGoArrow->SetArrowLength(80.0f);

	LeftHandRightGoArrow = CreateDefaultSubobject<UArrowComponent>("LeftHandRightGoArrow");
	LeftHandRightGoArrow->SetupAttachment(LeftMotionController);
	LeftHandRightGoArrow->SetArrowColor(FColor::Yellow);
	LeftHandRightGoArrow->SetArrowSize(0.2f);
	LeftHandRightGoArrow->SetArrowLength(80.0f);

	LeftHandRightArrow = CreateDefaultSubobject<UArrowComponent>("LeftHandRightArrow");
	LeftHandRightArrow->SetupAttachment(LeftMotionController);
	LeftHandRightArrow->SetArrowColor(FColor::Green);
	LeftHandRightArrow->SetArrowSize(0.2f);
	LeftHandRightArrow->SetArrowLength(40.0f);

	RightHandForwardArrow = CreateDefaultSubobject<UArrowComponent>("RightHandForwardArrow");
	RightHandForwardArrow->SetupAttachment(RightMotionController);
	RightHandForwardArrow->SetArrowColor(FColor::Red);
	RightHandForwardArrow->SetArrowSize(0.2f);
	RightHandForwardArrow->SetArrowLength(40.0f);

	RightHandRightArrow = CreateDefaultSubobject<UArrowComponent>("RightHandRightArrow");
	RightHandRightArrow->SetupAttachment(RightMotionController);
	RightHandRightArrow->SetArrowColor(FColor::Green);
	RightHandRightArrow->SetArrowSize(0.2f);
	RightHandRightArrow->SetArrowLength(40.0f);
}

// Called when the game starts or when spawned
void AVRCharacter::BeginPlay()
{
	Super::BeginPlay();

		LeftHandRightArrow->SetHiddenInGame(false);
	LeftHandForwardArrow->SetHiddenInGame(false);
	RightHandRightArrow->SetHiddenInGame(false);
	RightHandForwardArrow->SetHiddenInGame(false);
	LeftHandForwardGoArrow->SetHiddenInGame(false);
	LeftHandRightGoArrow->SetHiddenInGame(false);

	UE_LOG(LogVRCharacter, Warning, TEXT("Left Controller Name: %s"), *LeftMotionController->GetName());
	UE_LOG(LogVRCharacter, Warning, TEXT("Left Controller Motion Source: %s"),
	       *LeftMotionController->GetTrackingMotionSource().ToString());
	UE_LOG(LogVRCharacter, Warning, TEXT("Left Controller Full Name: %s"), *LeftMotionController->GetFullName());
	UE_LOG(LogVRCharacter, Warning, TEXT("Left Visualization Name: %s"), *LeftControllerVisualization->GetName());
	UE_LOG(LogVRCharacter, Warning, TEXT("Left Visualization Full Name: %s"),
	       *LeftControllerVisualization->GetFullName());


	// Add Input Mapping Context
	if (const APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
		else
		{
			UE_LOG(LogVRCharacter, Warning, TEXT("Unable to add mapping context"));
		}
	}
	else
	{
		UE_LOG(LogVRCharacter, Warning, TEXT("Unable to get the PlayerController"));
	}

	if (!UHeadMountedDisplayFunctionLibrary::EnableHMD(true))
	{
		UE_LOG(LogVRCharacter, Warning, TEXT("Unable to activate HMD"));
		return;
	}

	UE_LOG(LogVRCharacter, Warning, TEXT("HMD Activated: %s"),
	       *UHeadMountedDisplayFunctionLibrary::GetHMDDeviceName().ToString());
	FXRMotionControllerData MotionControllerData;
	UHeadMountedDisplayFunctionLibrary::GetMotionControllerData(GetWorld(), LeftMotionController->GetTrackingSource(),
	                                                            MotionControllerData);
	UE_LOG(LogVRCharacter, Warning, TEXT("Left Controller Device Name: %s"),
	       *MotionControllerData.DeviceName.ToString());


	// Set the tracking origin
	// CUSTOM_OPEN_XR: Custom OpenXR tracking space of some kind. You cannot set this space explicitly, it is automatically used by some platform plugin extensions.
	// LOCAL: For seated experiences. Always Supported. Typically centered around the HMDs initial position either at app startup or device startup. Useful for seated experiences. Previously called Eye Space.
	// LOCAL_FLOOR: For standing stationary experiences. Typically centered around HMDs initial position either at app startup or device startup, with Z 0 set to match the floor as in the Stage Space. Falls back to local.
	// STAGE: For walking-around experiences. The origin will be at floor level and typically within a defined play areas who’s bounds will be available. Falls back to local.
	// EYE: Previously sometimes used Eye space to query for the view transform, this space is fixed to the HMD, meaning that as the hmd moves this space moves relative to other spaces. This isn’t used as a tracking origin.
	if (SeatedVR)
	{
		AllowCrouchToggle = true;
		VROrigin->SetRelativeLocation(FVector(0.f, 0.f, 88.f));
		UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Local);
	}
	else
	{
		VROrigin->SetRelativeLocation(FVector(0.f, 0.f, -88.f));
		UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Stage);
	}

	const FRotator HandRotation = FRotator(-80.0f, 0.0f, 90.0f);
	const FVector RightHandPosition = FVector(0.0f, 2.0f, 6.0f);
	const FVector LeftHandPosition = FVector(0.0f, -2.0f, 6.0f);
	const FVector LeftHandScalar = FVector(1.0f, 1.0f, -1.0f);
	RightHandMesh->SetRelativeTransform(FTransform(HandRotation, RightHandPosition, FVector::OneVector));
	LeftHandMesh->SetRelativeTransform(FTransform(HandRotation, LeftHandPosition, LeftHandScalar));

	LeftControllerVisualization->SetIsVisualizationActive(ShowControllers);
	RightControllerVisualization->SetIsVisualizationActive(ShowControllers);
	LeftHandMesh->SetVisibility(!ShowControllers);
	RightHandMesh->SetVisibility(!ShowControllers);

	PreviousCapsuleHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
}

// Called every frame
void AVRCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	LeftHandForwardArrow->SetWorldRotation(LeftMotionController->GetForwardVector().Rotation());
	FRotator LeftHandForwardGoArrowRotator = (LeftMotionController->GetForwardVector().GetSafeNormal() -
		LeftMotionController->GetUpVector().GetSafeNormal()).GetSafeNormal().Rotation();
	LeftHandForwardGoArrowRotator.Pitch = 0;
	LeftHandForwardGoArrowRotator.Roll = 0;
	LeftHandForwardGoArrow->SetWorldRotation(LeftHandForwardGoArrowRotator);

	FRotator LeftHandRightGoArrowRotator = LeftMotionController->GetRightVector().GetSafeNormal().Rotation();
	LeftHandRightGoArrowRotator.Pitch = 0;
	LeftHandRightGoArrowRotator.Roll = 0;
	LeftHandRightGoArrow->SetWorldRotation(LeftHandRightGoArrowRotator);

	LeftHandRightArrow->SetWorldRotation(LeftMotionController->GetRightVector().Rotation());
	RightHandRightArrow->SetWorldRotation(RightMotionController->GetRightVector().Rotation());
	RightHandForwardArrow->SetWorldRotation(RightMotionController->GetForwardVector().Rotation());

	if (!SeatedVR)
	{
		UpdateRoomScaleLocation();
		UpdateCapsuleHeight();
	}
}

// Called to bind functionality to input
void AVRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ThisClass::PerformJump);
	EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &ThisClass::ToggleCrouch);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
	EnhancedInputComponent->BindAction(SmoothTurnAction, ETriggerEvent::Triggered, this, &ThisClass::SmoothTurn);
	EnhancedInputComponent->BindAction(SnapTurnAction, ETriggerEvent::Triggered, this, &ThisClass::SnapTurn);
}

void AVRCharacter::SmoothTurn(const FInputActionValue& Value)
{
	if (!EnableSmoothRotation)
	{
		return;
	}
	const float AxisValue = Value.Get<FVector2D>().X; // Ignore the Y value, we're only turning left/right
	AddControllerYawInput(SmoothRotationRate * AxisValue * GetWorld()->GetDeltaSeconds());
}

void AVRCharacter::SnapTurn(const FInputActionValue& Value)
{
	if (EnableSmoothRotation)
	{
		return;
	}

	if (const float AxisValue = Value.Get<FVector2D>().X; AxisValue > 0)
	{
		AddControllerYawInput(SnapTurnAngle);
	}
	else
	{
		AddControllerYawInput(SnapTurnAngle * -1.0f);
	}
}

void AVRCharacter::ToggleCrouch(const FInputActionValue& Value)
{
	const float AxisValue = Value.Get<FVector2D>().Y;
	if (!SeatedVR || AllowCrouchToggle)
	{
		return;
	}

	if (CurrentPose == EPose::Standing)
	{
		if (AxisValue < 0.0f)
		{
			CurrentPose = EPose::Crouching;
			Crouch();
		}
	}
	else if (CurrentPose == EPose::Crouching)
	{
		if (AxisValue > 0.0f)
		{
			CurrentPose = EPose::Standing;
			UnCrouch();
		}
		else
		{
			CurrentPose = EPose::Crawling;
			// Crawl
		}
	}
	else // Crawling
	{
		if (AxisValue > 0.0f)
		{
			CurrentPose = EPose::Crouching;
			Crouch();
		}
	}
}

void AVRCharacter::PerformJump(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("Jump!"));
	if (CurrentPose == EPose::Standing || CurrentPose == EPose::Crouching)
	{
		Jump();
	}
}

void AVRCharacter::GrabAxisLeft(const float AxisValue) const
{
	// TODO: Add hand animation
}

void AVRCharacter::GrabAxisRight(const float AxisValue) const
{
	// TODO: Add hand animation
}

void AVRCharacter::UpdateRoomScaleLocation()
{
	FVector DeltaLocation = Camera->GetComponentLocation() - GetCapsuleComponent()->GetComponentLocation();
	DeltaLocation.Z = .0f;

	AddActorWorldOffset(DeltaLocation, false, nullptr, ETeleportType::TeleportPhysics);
	VROrigin->AddWorldOffset(-DeltaLocation, false, nullptr, ETeleportType::TeleportPhysics);
}

void AVRCharacter::UpdateCapsuleHeight()
{
	FRotator Rotation;
	FVector Position;
	UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(Rotation, Position);
	const float NewCapsuleHalfHeight = Position.Z / 2.0f + 10.0f;
	if (!SeatedVR)
	{
		GetCapsuleComponent()->SetCapsuleSize(GetCapsuleComponent()->GetScaledCapsuleRadius(), NewCapsuleHalfHeight);
		VROrigin->AddRelativeLocation(FVector(0, 0, PreviousCapsuleHeight - NewCapsuleHalfHeight));
		PreviousCapsuleHeight = NewCapsuleHalfHeight;
	}

	if (NewCapsuleHalfHeight < CrawlHeight)
	{
		GetCharacterMovement()->MaxWalkSpeed = CrawlSpeed;
		// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
		//                                  FString::Printf(TEXT("Crawling %f"), NewCapsuleHalfHeight));
		CurrentPose = EPose::Crawling;
	}
	else if (NewCapsuleHalfHeight < CrouchHeight)
	{
		GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;
		// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow,
		//                                  FString::Printf(TEXT("Crouching %f"), NewCapsuleHalfHeight));
		CurrentPose = EPose::Crouching;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
		// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green,
		//                                  FString::Printf(TEXT("Standing %f"), NewCapsuleHalfHeight));
		CurrentPose = EPose::Standing;
	}
}

/** Move the character in the direction of the input */
void AVRCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D InputAxisVector = Value.Get<FVector2D>();

	FRotator ForwardRotator;
	FRotator RightRotator;
	bool bOculus = true; // TODO: Make this a variable
	switch (ForwardSource)
	{
	case EForwardSource::LeftController:
		// Adjust the forward vector to match the controller's rotation
		if (bOculus)
		{
			// Additional adjustment of 45 degrees for the Oculus Touch controllers
			ForwardRotator = (LeftMotionController->GetForwardVector().GetSafeNormal() - LeftMotionController->
				GetUpVector().GetSafeNormal()).GetSafeNormal().Rotation();
			RightRotator = LeftMotionController->GetRightVector().Rotation();
		}
		else
		{
			ForwardRotator = LeftMotionController->GetForwardVector().Rotation();
			RightRotator = LeftMotionController->GetRightVector().Rotation();
		}
		break;
	case EForwardSource::RightController:
		// Adjust the forward vector to match the controller's rotation
		// Additional adjustment of 45 degrees for the Oculus Touch controllers
		ForwardRotator = (RightMotionController->GetForwardVector().GetSafeNormal() - RightMotionController->
			GetUpVector().GetSafeNormal()).GetSafeNormal().Rotation();
		RightRotator = RightMotionController->GetRightVector().Rotation();
		break;
	default: // HMD
		ForwardRotator = Camera->GetForwardVector().Rotation();
		RightRotator = Camera->GetRightVector().Rotation();
	}
	ForwardRotator.Pitch = 0;
	ForwardRotator.Roll = 0;
	const FVector ForwardVectorYawOnly = ForwardRotator.Vector().GetSafeNormal();
	AddMovementInput(ForwardVectorYawOnly, InputAxisVector.Y);

	RightRotator.Pitch = 0;
	RightRotator.Roll = 0;
	const FVector RightVectorYawOnly = RightRotator.Vector().GetSafeNormal();
	AddMovementInput(RightVectorYawOnly, InputAxisVector.X);
}
