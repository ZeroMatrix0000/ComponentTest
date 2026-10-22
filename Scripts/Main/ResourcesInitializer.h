/*
 * FileName:     ResourcesInitializer.h
 * Author:       Takao Hayata
 * Last Updated: 2026/10/23
 *
 * リソース初期化
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
	class DeviceResources;
	class RenderingResources;
	class ITextRenderer;
	class IColliderRenderer;
}
namespace Systems
{
	class Resources;
}

namespace ResourcesInitializer
{
	// リソース初期化
	void InitializeResources
	(
		const Renderings::DeviceResources&    deviceResources,
		const Renderings::RenderingResources& renderingResources,
		Ref<Systems::Resources>               resources
	);
}
