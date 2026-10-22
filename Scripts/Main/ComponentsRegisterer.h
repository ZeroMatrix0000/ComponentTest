/*
 * FileName:     ComponentsRegisterer.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/22
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

namespace ComponentsRegisterer
{
	// コンポーネント対応付け
	void RegisterComponents
	(
		Ref<ComponentManager>              componentManager,
		Ref<GameObjectManager>             gameObjectManager,
		Ref<Renderings::IModel3DRenderer>  iModelRenderer,
		Ref<Renderings::IImageRenderer>    iimageRenderer,
		Ref<Renderings::ITextRenderer>     iTextRenderer,
		Ref<Renderings::IColliderRenderer> iColliderRenderer,
		Ref<Systems::IResources>           iResources
	);
}
