// Jolt Physics Library (https://github.com/jrouwe/JoltPhysics)
// SPDX-FileCopyrightText: 2024 Jorrit Rouwe
// SPDX-License-Identifier: MIT

#include <TestFramework.h>

#include <Tests/General/SimulationShapeFilterTest.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/StaticCompoundShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Layers.h>

JPH_IMPLEMENT_RTTI_VIRTUAL(SimulationShapeFilterTest)
{
	JPH_ADD_BASE_CLASS(SimulationShapeFilterTest, Test)
}

SimulationShapeFilterTest::~SimulationShapeFilterTest()
{
	// Unregister shape filter
	mPhysicsSystem->SetSimulationShapeFilter(nullptr);
}

void SimulationShapeFilterTest::Initialize()
{
	// Register shape filter
	mPhysicsSystem->SetSimulationShapeFilter(&mShapeFilter);

	// Floor
	CreateFloor();

	// Platform
	mShapeFilter.mPlatformID = mBodyInterface->CreateAndAddBody(BodyCreationSettings(new BoxShape(Vec3(5.0f, 0.5f, 5.0f)), RVec3(0, 7.5f, 0), Quat::sRotation(Vec3::sAxisX(), 0.25f * JPH_PI), EMotionType::Static, Layers::NON_MOVING), EActivation::DontActivate);

	// Compound shape
	Ref<Shape> capsule = new CapsuleShape(2, 0.1f);
	capsule->SetUserData(1); // Don't want the capsule to collide with the platform
	Ref<Shape> sphere = new SphereShape(0.5f);
	sphere->SetUserData(1); // Don't want the sphere to collide with the platform
	Ref<Shape> box = new BoxShape(Vec3::sReplicate(0.5f));
	Ref<StaticCompoundShapeSettings> compound = new StaticCompoundShapeSettings;
	compound->AddShape(Vec3::sZero(), Quat::sIdentity(), capsule);
	compound->AddShape(Vec3(0, -2, 0), Quat::sIdentity(), sphere);
	compound->AddShape(Vec3(0, 2, 0), Quat::sIdentity(), box);
	mShapeFilter.mCompoundID = mBodyInterface->CreateAndAddBody(BodyCreationSettings(compound, RVec3(0, 15, 0), Quat::sIdentity(), EMotionType::Dynamic, Layers::MOVING), EActivation::Activate);
}

bool SimulationShapeFilterTest::Filter::ShouldCollide(const Shape *inShape1, const SubShapeID &inSubShapeIDOfShape1, const Shape *inShape2, const SubShapeID &inSubShapeIDOfShape2) const
{
	// If the platform is colliding with the compound, filter out collisions where the shape has user data 1
	if (mBodyID1 == mPlatformID && mBodyID2 == mCompoundID)
		return inShape2->GetUserData() != 1;
	else if (mBodyID1 == mCompoundID && mBodyID2 == mPlatformID)
		return inShape1->GetUserData() != 1;
	return true;
}
