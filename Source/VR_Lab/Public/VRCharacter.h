// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "VRCharacter.generated.h"

struct FInputActionValue;
class UXRDeviceVisualizationComponent;
class UCameraComponent;
class UMotionControllerComponent;
class UInputAction;
class UInputMappingContext;
DECLARE_LOG_CATEGORY_EXTERN(LogVRCharacter, Log, All);

UENUM()
enum class EPose : uint8 { Standing, Crouching, Crawling };

UENUM()
enum class EForwardSource : uint8 { HMD, LeftController, RightController };

UCLASS()
class VR_LAB_API AVRCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AVRCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Move(const FInputActionValue& Value);
	void SmoothTurn(const FInputActionValue& Value);
	void SnapTurn(const FInputActionValue& Value);
	void PerformJump(const FInputActionValue& Value);
	void ToggleCrouch(const FInputActionValue& Value);
	void GrabAxisLeft(const float AxisValue) const;
	void GrabAxisRight(const float AxisValue) const;
	void UpdateRoomScaleLocation();
	void UpdateCapsuleHeight();

		/** Is this a seated or standing VR experience? */
	UPROPERTY(EditAnywhere, Category="VR|Camera")
	bool SeatedVR = false;

	/** Does the player want to allow toggling of crouch? This is always true if Seated. */
	bool AllowCrouchToggle = true;

	/** Should smooth rotation be used instead of snap turning? (default: false) */
	UPROPERTY(EditAnywhere, Category="VR|Movement|Turn")
	bool EnableSmoothRotation = false;

	/** At what rate should smooth rotation turn at? Default: 30 */
	UPROPERTY(EditAnywhere, Category="VR|Movement|Turn")
	float SmoothRotationRate = 30.0f;

	/** How long of a delay should there be when snap turning? (default: 0.3) */
	UPROPERTY(EditAnywhere, Category="VR|Movement|Turn")
	float SnapTurnDelay = 0.2f;

	/** How many degrees should the character turn when snap turning? (default: 15.0) */
	UPROPERTY(EditAnywhere, Category="VR|Movement|Turn")
	float SnapTurnAngle = 15.0f;

	/** Should the left or right hand control movement? (default: true) */
	UPROPERTY(EditAnywhere, Category="VR|Movement")
	bool bRightHandedControls = true;

	/** At what height is the character considered to be in a crouch? (default: 70) */
	UPROPERTY(EditAnywhere, Category="VR|Movement|Speed")
	float CrouchHeight = 70.f;

	/** At what height is the character considered to be crawling? (default: 50) */
	UPROPERTY(EditAnywhere, Category="VR|Movement|Speed")
	float CrawlHeight = 50.f;

	/** What is the movement rate of someone who is crouching? (default: 100) */
	UPROPERTY(EditAnywhere, Category="VR|Movement|Speed")
	float CrouchSpeed = 100.f;

	/** What is the movement rate of someone who is crawling? (default: 50) */
	UPROPERTY(EditAnywhere, Category="VR|Movement|Speed")
	float CrawlSpeed = 50.f;

	/** What is the normal walking speed? (default of 4mph is 178.8 cm/s) */
	UPROPERTY(EditAnywhere, Category="VR|Movement|Speed")
	float WalkSpeed = 178.8f;

	/** What is the normal running speed? (default of 8mph is 357.6 cm/s) */
	UPROPERTY(EditAnywhere, Category="VR|Movement|Speed")
	float RunSpeed = 357.6f;

	/** What speed is considered to be sprinting? (default of 15mph for an athlete is 670.6 cm/s) */
	UPROPERTY(EditAnywhere, Category="VR|Movement|Speed")
	float SprintSpeed = 670.6f;

	/** What should be used as the source of the Forward vector when moving? (default: left) */
	UPROPERTY(EditAnywhere, Category="VR|Movement")
	EForwardSource ForwardSource = EForwardSource::LeftController;

	/** Right hand mesh */
	UPROPERTY(EditAnywhere, Category="VR|Mesh")
	USkeletalMeshComponent* RightHandMesh;

	/** Skeletal mesh for the right hand */
	UPROPERTY(EditAnywhere, Category="VR|Mesh")
	USkeletalMesh* RightHandMeshSkeleton;

	/** Left hand mesh */
	UPROPERTY(EditAnywhere, Category="VR|Mesh")
	USkeletalMeshComponent* LeftHandMesh;

	/** Skeletal mesh for the left hand */
	UPROPERTY(EditAnywhere, Category="VR|Mesh")
	USkeletalMesh* LeftHandMeshSkeleton;

	/** Toggles whether to display controllers or hand meshes */
	UPROPERTY(EditAnywhere, Category="VR|Mesh")
	bool ShowControllers = true;

private:
	UPROPERTY(EditAnywhere, Category="VR|Input|Context")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, Category="VR|Input|Actions")
	TObjectPtr<UInputAction> CrouchAction;
	
	UPROPERTY(EditAnywhere, Category="VR|Input|Actions")
	TObjectPtr<UInputAction> JumpAction;
	
	UPROPERTY(EditAnywhere, Category="VR|Input|Actions")
	TObjectPtr<UInputAction> MoveAction;
	
	UPROPERTY(EditAnywhere, Category="VR|Input|Actions")
	TObjectPtr<UInputAction> SmoothTurnAction;
	
	UPROPERTY(EditAnywhere, Category="VR|Input|Actions")
	TObjectPtr<UInputAction> SnapTurnAction;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category="VR|MotionController", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMotionControllerComponent> LeftMotionController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category="VR|MotionController", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMotionControllerComponent> RightMotionController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="VR|Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> VROrigin;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="VR|Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> Camera;
	
	TObjectPtr<UXRDeviceVisualizationComponent> RightControllerVisualization;
	TObjectPtr<UXRDeviceVisualizationComponent> LeftControllerVisualization;
	TObjectPtr<UArrowComponent> LeftHandForwardArrow;
	TObjectPtr<UArrowComponent> LeftHandForwardGoArrow;
	TObjectPtr<UArrowComponent> LeftHandRightGoArrow;
	TObjectPtr<UArrowComponent> LeftHandRightArrow;
	TObjectPtr<UArrowComponent> RightHandForwardArrow;
	TObjectPtr<UArrowComponent> RightHandRightArrow;
	
	bool bCanSnapTurn = false;
	float PreviousCapsuleHeight;

	EPose CurrentPose = EPose::Standing;
};
