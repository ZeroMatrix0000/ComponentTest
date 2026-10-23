/*
 * FileName:     IsDerivedRegister.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/23
 *
 * 継承関係の登録
 */

#pragma once

namespace Components
{
	class Component;
	class Transform;
	class RectTransform;
}
namespace Colliders
{
	class BoxCollider;
	class SphereCollider;
	class MeshCollider;
}
namespace Renderings
{
	class Model3D;
	class Image;
	class Text;
	class Particle;
	class ParticleSample;
	class ParticleManager;
	class ICameraScreen;
	template<typename TCamera> requires IsSame
	<
		TCamera,
		Camera::QuaternionCamera,
		Camera::QuaternionTargetCamera,
		Camera::EulerCamera,
		Camera::EulerTargetCamera
	>
	class CameraScreen;
	class Canvas;
}

#define IS_DERIVED_REGISTER(T, U) \
template<> \
struct IsDerivedTrait<T, U> : std::true_type {};

IS_DERIVED_REGISTER(Transform, Component)
IS_DERIVED_REGISTER(RectTransform, Component)
IS_DERIVED_REGISTER(Colliders::BoxCollider, Component)
IS_DERIVED_REGISTER(Colliders::SphereCollider, Component)
IS_DERIVED_REGISTER(Colliders::MeshCollider, Component)
IS_DERIVED_REGISTER(Renderings::Model3D, Component)
IS_DERIVED_REGISTER(Renderings::Image, Component)
IS_DERIVED_REGISTER(Renderings::Text, Component)
IS_DERIVED_REGISTER(Renderings::ParticleManager, Component)
IS_DERIVED_REGISTER(Renderings::ICameraScreen, Component)
IS_DERIVED_REGISTER(Renderings::CameraScreen<Camera::QuaternionCamera>, Component)
IS_DERIVED_REGISTER(Renderings::CameraScreen<Camera::QuaternionTargetCamera>, Component)
IS_DERIVED_REGISTER(Renderings::CameraScreen<Camera::EulerCamera>, Component)
IS_DERIVED_REGISTER(Renderings::CameraScreen<Camera::EulerTargetCamera>, Component)
IS_DERIVED_REGISTER(Renderings::Canvas, Component)

IS_DERIVED_REGISTER(Renderings::ParticleSample, Renderings::Particle)

#undef IS_DERIVED_REGISTER
