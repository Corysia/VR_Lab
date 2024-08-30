// Copyright Epic Games, Inc. All Rights Reserved.

#include "TP_ThirdPersonCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ATP_ThirdPersonCharacter

/**
 * Sets up the character's properties and components.
 *
 * This function is called when the character is spawned.
 * It sets up the input bindings for the character, the character's size and collision properties,
 * the character's movement properties, and the camera boom that follows the character.
 */
ATP_ThirdPersonCharacter::ATP_ThirdPersonCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = CameraBoomMaxLength / 2.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

/**
 * Called when the game starts or when spawned.
 *
 * This function is called when the game starts or when the character is spawned.
 * It sets up the input bindings for the character.
 */
void ATP_ThirdPersonCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// Add input mapping context
	// The input mapping context is used to determine which input mapping set to use
	// for the character's input bindings. The mapping context is set up in the
	// character's blueprint and is used to remap the character's bindings to
	// gamepad or keyboard and mouse.
	if (const APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Add the mapping context to the subsystem. This will set up the
			// input mappings for the character.
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

/**
 * Sets up the player input component.
 *
 * This function is called when the character is spawned and sets up the input
 * bindings for the character. It adds the mapping context and binds the actions
 * to the input events.
 *
 * @param PlayerInputComponent The input component to set up.
 */
void ATP_ThirdPersonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	// The mapping context is used to determine which input mapping set to use
	// for the character's input bindings. The mapping context is set up in the
	// character's blueprint and is used to remap the character's bindings to
	// gamepad or keyboard and mouse.
	if (const APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Add the mapping context to the subsystem. This will set up the
			// input mappings for the character.
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	// The action bindings are the input events that are used to trigger the
	// character's actions. These bindings are set up in the character's blueprint
	// and are used to map the input events to the character's actions.
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		// The jumping action is bound to the space bar and the left mouse button.
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		// The moving action is bound to the W, A, S, and D keys.
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATP_ThirdPersonCharacter::Move);

		// Looking
		// The looking action is bound to the mouse.
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATP_ThirdPersonCharacter::Look);
		
		// Zooming
		// The zooming action is bound to the mouse wheel.
        EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &ATP_ThirdPersonCharacter::BoomZoom);
	}
	else
	{
		// If the input component is not an enhanced input component, log an error
		// message.
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

/**
 * Called for movement input.
 *
 * @param Value The value from the input action, which is a Vector2D that will be used to control the movement of the player character.
 */
void ATP_ThirdPersonCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// Find out which way is forward.
		// The Controller is the PlayerController, which is the class that controls the character.
		// The ControlRotation is the rotation of the controller, which is the rotation of the player character.
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// Get the forward vector.
		// The forward vector is the vector that points forward from the character.
		// It is used to move the character forward and backward.
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// Get the right vector.
		// The right vector is the vector that points to the right of the character.
		// It is used to move the character left and right.
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// Add movement.
		// The AddMovementInput() function is called on the character, and it adds the movement to the character.
		// The first argument is the direction of the movement, and the second argument is the magnitude of the movement.
		// The magnitude is the length of the movement vector.
		// The direction is the unit vector of the movement vector.
		// The character will move in the direction of the vector by the magnitude.
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

/**
 * Called for looking input.
 *
 * @param Value The value from the input action, which is a Vector2D that will be used to control the yaw and pitch of the player character.
 */
void ATP_ThirdPersonCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		// The LookAxisVector.X is the horizontal input, and Y is the vertical input
		// These values are used to control the yaw and pitch of the player character
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

/**
 * Zooms the camera in and out.
 *
 * @param Value The value from the input action, which is a float that will be used to zoom the camera.
 */
// ReSharper disable once CppMemberFunctionMayBeConst
void ATP_ThirdPersonCharacter::BoomZoom(const FInputActionValue& Value)
{
    // Increase the arm length based on the input value
    CameraBoom->TargetArmLength += Value.Get<float>() * CameraBoomZoomSpeed;

    // Make sure the camera boom length is within the valid range
    if (CameraBoom->TargetArmLength < CameraBoomMinLength)
    {
        CameraBoom->TargetArmLength = CameraBoomMinLength;
    }
    else if (CameraBoom->TargetArmLength > CameraBoomMaxLength)
    {
        CameraBoom->TargetArmLength = CameraBoomMaxLength;
    }
}