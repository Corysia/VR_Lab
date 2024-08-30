// Copyright 2024 Corysia Taware / Shoebox Games.  All rights reserved.

#include "DesktopCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

DEFINE_LOG_CATEGORY(LogDesktopCharacter);

//////////////////////////////////////////////////////////////////////////
// ADesktopCharacter

/**
 * Sets up the character's properties and components.
 *
 * This function is called when the character is spawned.
 * It sets up the input bindings for the character, the character's size and collision properties,
 * the character's movement properties, and the camera boom that follows the character.
 */
ADesktopCharacter::ADesktopCharacter()
{
    // Set size for collision capsule
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    // Don't rotate when the controller rotates. Let that just affect the camera.
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // Configure character movement
    GetCharacterMovement()->bOrientRotationToMovement = true;            // Character moves in the direction of input...
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
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = CameraBoomMaxLength / 2.0f; // The camera follows at this distance behind the character
    CameraBoom->bUsePawnControlRotation = true;               // Rotate the arm based on the controller

    // Create a follow camera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

    // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
    FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

    FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>("FirstPersonCamera");
    FirstPersonCamera->SetupAttachment(GetMesh(), FName("head"));
    FirstPersonCamera->bUsePawnControlRotation = true;
}

/**
 * Called when the game starts or when spawned.
 *
 * This function is called when the game starts or when the character is spawned.
 * It sets up the input bindings for the character.
 */
void ADesktopCharacter::BeginPlay()
{
    // Call the base class
    Super::BeginPlay();

    // Note that the camera is positioned in the blueprint and not in the code
    FirstPersonCamera->Deactivate(); // Always start in 3rd person

    // Add input mapping context
    // The input mapping context is used to determine which input mapping set to use
    // for the character's input bindings. The mapping context is set up in the
    // character's blueprint and is used to remap the character's bindings to
    // gamepad or keyboard and mouse.
    if (const APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
            UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
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
void ADesktopCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    // Add Input Mapping Context
    // The mapping context is used to determine which input mapping set to use
    // for the character's input bindings. The mapping context is set up in the
    // character's blueprint and is used to remap the character's bindings to
    // gamepad or keyboard and mouse.
    if (const APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
            UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
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
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // Jumping
        // The jumping action is bound to the space bar and the left mouse button.
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

        // Moving
        // The moving action is bound to the W, A, S, and D keys.
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);

        // Looking
        // The looking action is bound to the mouse.
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);

        // Zooming
        // The zooming action is bound to the mouse wheel.
        EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &ThisClass::BoomZoom);

        // Perspective
        // The perspective action is bound to the 'p' key by default.
        EnhancedInputComponent->BindAction(PerspectiveAction, ETriggerEvent::Triggered, this, &ThisClass::TogglePerspective);
    }
    else
    {
        // If the input component is not an enhanced input component, log an error
        // message.
        UE_LOG(LogDesktopCharacter,
               Error,
               TEXT(
                   "'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."
               ),
               *GetNameSafe(this));
    }
}

/**
 * Called for movement input.
 *
 * @param Value The value from the input action, which is a Vector2D that will be used to control the movement of the player character.
 */
void ADesktopCharacter::Move(const FInputActionValue& Value)
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
void ADesktopCharacter::Look(const FInputActionValue& Value)
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
void ADesktopCharacter::BoomZoom(const FInputActionValue& Value)
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

/**
 * Checks if the character is currently in first person perspective.
 *
 * @return true if the character is in first person, false otherwise
 */
bool ADesktopCharacter::IsInFirstPerson() const
{
    // Check if the first person camera is active
    return FirstPersonCamera->IsActive();
}

/**
 * Toggles the character's perspective between first person and third person.
 *
 * If the character is in first person, this function will switch to third person.
 * If the character is in third person, this function will switch to first person.
 *
 * This function is called on the client, and it will call the server function
 * Server_TogglePerspective_Implementation() to perform the actual toggling.
 */
void ADesktopCharacter::TogglePerspective()
{
    if (!HasAuthority())
    {
        // If we are on the client, we need to call the server function
        // to perform the actual toggling
        UE_LOG(LogDesktopCharacter, Verbose, TEXT("Client_TogglePerspective"))
        Server_TogglePerspective();
    }
    else
    {
        // If we are on the server, we can perform the actual toggling here
        // If we are in first person, switch to third person
        if (IsInFirstPerson())
        {
            UE_LOG(LogDesktopCharacter, Verbose, TEXT("Switching to third person"))
            FollowCamera->Activate();
            FirstPersonCamera->Deactivate();
            bUseControllerRotationYaw = false;
            GetCharacterMovement()->bOrientRotationToMovement = true;
        }
        else // If we are in third person, switch to first person
        {
            UE_LOG(LogDesktopCharacter, Verbose, TEXT("Switching to first person"))
            FollowCamera->Deactivate();
            FirstPersonCamera->Activate();
            GetController()->SetControlRotation(GetActorRotation()); // re-orient the camera to the direction the character is facing
            bUseControllerRotationYaw = true;
            GetCharacterMovement()->bOrientRotationToMovement = false;
        }
    }
}

/**
 * Called on the server to toggle the character's perspective between first person and third person.
 *
 * This function is called by the client when the player presses the toggle perspective button.
 * It will call the TogglePerspective() function, which will actually perform the toggling.
 */
void ADesktopCharacter::Server_TogglePerspective_Implementation()
{
    UE_LOG(LogDesktopCharacter, Verbose, TEXT("Server_TogglePerspective_Implementation"));
    TogglePerspective();
}

/**
 * Validation function for the Server_TogglePerspective_Implementation() function.
 *
 * This function is called before the Server_TogglePerspective_Implementation() function is called.
 * It is used to validate the parameters of the function, and to check if the function should be called.
 *
 * @return true if the function should be called, false otherwise
 */
bool ADesktopCharacter::Server_TogglePerspective_Validate()
{
    UE_LOG(LogDesktopCharacter, Verbose, TEXT("Server_TogglePerspective_Validate"));
    return true;
}
