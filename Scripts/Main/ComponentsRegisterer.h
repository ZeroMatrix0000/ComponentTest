/*
 * FileName:     ComponentsRegisterer.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/24
 *
 * コンポーネント登録者
 */

#pragma once

namespace Components
{
	class ComponentManager;
}
namespace GameObjects
{
	class GameObjectManager;
}
namespace Renderings
{
	class IModel3DRenderer;
	class IImageRenderer;
	class ITextRenderer;
	class IColliderRenderer;
}
namespace Systems
{
	class IResources;
}

// コンポーネント登録者
namespace ComponentsRegisterer
{
	// コンポーネント登録
	void RegisterComponents
	(
		Ref<ComponentManager>              componentManagerRef,
		Ref<GameObjectManager>             gameObjectManagerRef,
		Ref<Renderings::IModel3DRenderer>  iModelRendererRef,
		Ref<Renderings::IImageRenderer>    iimageRendererRef,
		Ref<Renderings::ITextRenderer>     iTextRendererRef,
		Ref<Renderings::IColliderRenderer> iColliderRendererRef,
		Ref<Systems::IResources>           iResourcesRef
	);
}
