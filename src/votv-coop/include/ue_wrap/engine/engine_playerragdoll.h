// ue_wrap/engine/engine_playerragdoll.h -- the plushie ragdoll body the game spawns for a faint,
// driven for a puppet. Engine-wrapper layer (principle 7): each call marshals one UFunction call
// or one reflected field access, with no gameplay, network or coop state. Game thread unless a
// declaration says otherwise. Implementation: src/ue_wrap/engine/engine_playerragdoll.cpp.

#pragma once

#include "ue_wrap/core/types.h"

namespace ue_wrap::engine {

// Spawn the game's playerRagdoll_C for `ownerPlayer` as the visible flop body: its Player field is
// stamped before Finish, so BeginPlay configures the mesh from the owner, and then the simulation
// starts. Death-free, since ragdollMode would kill the host. The AActor*, or nullptr. Game thread.
void* SpawnPlayerRagdollBody(void* ownerPlayer, const FVector& location, const FRotator& rotation);

// Attach `actor` to `body`'s mesh at the pelvis bone (KeepWorld), so it follows the flop; Detach
// reverses it. Game thread.
bool AttachActorToRagdollBody(void* actor, void* body);

bool DetachActorFromRagdollBody(void* actor);

// The sender's ragdoll stream: the native ragdoll's pelvis world transform and velocity, in cm/s
// and deg/s. False while not ragdolling. Game thread.
bool ReadLocalRagdollPelvisPhysics(void* mainPlayer, FVector& outLoc, FRotator& outRot,
                                   FVector& outLinVel, FVector& outAngVel);

// The receiver's ragdoll stream: set the mirror body's pelvis linear and angular velocity so it
// tracks the sender's flop. Game thread.
void DriveRagdollBodyPelvisVelocity(void* body, const FVector& linVel, const FVector& angVel);

// A ragdoll actor's simulating SkeletalMesh; null if dead. Game thread.
void* GetRagdollBodyMesh(void* ragdollActor);

// Dress a freshly spawned flop `body` in a player's custom skin, so the skin itself falls instead
// of the stock plushie: the body's mesh takes `skinMesh` and the materials of `skinComponent` (the
// puppet's visible skinned component). The skin's own physics asset is used when it has one (every
// bone simulates); otherwise the plushie's is kept as an override, and the bones both skeletons
// share (pelvis, chest, head) simulate while the rest ride them. If the dressed mesh has no pelvis
// the body is put back as it was. Call before AttachActorToRagdollBody. True when dressed.
// Game thread.
bool ReskinRagdollBody(void* body, void* skinMesh, void* skinComponent);

// The local player's native ragdoll mesh; the owning field is non-null exactly while ragdolling.
// Game thread.
void* GetLocalRagdollBodyMesh(void* mainPlayer);

}  // namespace ue_wrap::engine
