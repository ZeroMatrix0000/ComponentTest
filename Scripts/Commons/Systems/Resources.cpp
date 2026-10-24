/*
 * FileName:     Resources.cpp
 * Author:       Takao Hayata
 * Last Updated: 2026/10/24
 *
 * リソース管理
 */

#include "Pch.h"
#include "Resources.h"

#include "IErrorMessage.h"

Systems::Resources::Resources()
	: IResources{}
	, m_modelSources{}
	, m_imageSources{}
	, m_jsons{}
	, m_meshes{}
	, m_vertexShaders{}
	, m_pixelShaders{}
{
}

// モデルを追加
void Systems::Resources::LoadModelSources(ID3D11Device5* device, Renderings::InstancingEffectFactory* fx, const std::wstring& directoryPath)
{
	// パスが存在しないなら
	if (!std::filesystem::exists(directoryPath))
	{
		// エラーメッセージを追加
		IErrorMessage::GetInstance()->AddMessage(Utility::FormatWString
		(
			L"パスが間違っています。 | path: %s",
			directoryPath.c_str()
		));
		return;
	}
	
	// ディレクトリ内を全て検索
	for (const auto& entry : std::filesystem::recursive_directory_iterator(directoryPath))
	{
		// ファイルなら
		if (entry.is_regular_file())
		{
			m_modelSources.emplace(entry.path().stem().string(), Renderings::Model3DSource::Create(device, fx, &m_nullPixelShader, entry.path().wstring()));
			//try
			//{
			//	m_modelSources.emplace(entry.path().stem().string(), Renderings::Model3DSource::Create(device, fx, entry.path().wstring()));
			//}
			//catch (std::exception e)
			//{
			//	// エラーメッセージを追加
			//	IErrorMessage::GetInstance()->AddMessage(Utility::FormatWString
			//	(
			//		L"モデルの読み込みに失敗しました。 | path: %s",
			//		entry.path().c_str()
			//	));
			//}
		}
	}
}

// 画像を読み込む
void Systems::Resources::LoadImageSources(ID3D11Device5* device, const std::wstring& directoryPath)
{
	// パスが存在しないなら
	if (!std::filesystem::exists(directoryPath))
	{
		// エラーメッセージを追加
		IErrorMessage::GetInstance()->AddMessage(Utility::FormatWString
		(
			L"パスが間違っています。 | path: %s",
			directoryPath.c_str()
		));
		return;
	}

	// ディレクトリ内を全て検索
	for (const auto& entry : std::filesystem::recursive_directory_iterator(directoryPath))
	{
		// ファイルなら
		if (entry.is_regular_file())
		{
			try
			{
				m_imageSources.emplace(entry.path().stem().string(), Renderings::ImageSource::Create(Ref{ *device }, entry.path().wstring()));
			}
			catch (std::exception e)
			{
				// エラーメッセージを追加
				IErrorMessage::GetInstance()->AddMessage(Utility::FormatWString
				(
					L"画像の読み込みに失敗しました。 | path: %s",
					entry.path().c_str()
				));
			}
		}
	}
}

// Jsonを読み込む
void Systems::Resources::LoadJsons(const std::wstring& directoryPath)
{
	// パスが存在しないなら
	if (!std::filesystem::exists(directoryPath))
	{
		// エラーメッセージを追加
		IErrorMessage::GetInstance()->AddMessage(Utility::FormatWString
		(
			L"パスが間違っています。 | path: %s",
			directoryPath.c_str()
		));
		return;
	}

	// ディレクトリ内を全て検索
	for (const auto& entry : std::filesystem::recursive_directory_iterator(directoryPath))
	{
		// ファイルなら
		if (entry.is_regular_file())
		{
			std::ifstream fileStream{ entry.path() };
			// 開けなかったら終了
			if (!fileStream.is_open())
			{
				// エラーメッセージの追加
				Systems::IErrorMessage::GetInstance()->AddMessage(Utility::FormatWString
				(
					L"JSONファイルを開くことが出来ませんでした。 | path: %s",
					entry.path().c_str()
				));
				fileStream.close();
				continue;
			}

			try
			{
				m_jsons.emplace(entry.path().stem().string(), nlohmann::ordered_json::parse(fileStream, nullptr, true, true));
			}
			catch (std::exception e)
			{
				// エラーメッセージを追加
				IErrorMessage::GetInstance()->AddMessage(Utility::FormatWString
				(
					L"JSONファイルのフォーマットが正しくありません。 | path: %s",
					entry.path().c_str()
				));
			}
			fileStream.close();
		}
	}
}

// Jsonを文字列から追加
void Systems::Resources::AddJsonFromStr(const std::string& jsonName, const std::string& str)
{
	try
	{
		m_jsons.emplace(jsonName, nlohmann::ordered_json::parse(str));
	}
	catch (std::exception e)
	{
		// エラーメッセージを追加
		IErrorMessage::GetInstance()->AddMessage(Utility::FormatWString
		(
			L"JSONファイルのフォーマットが正しくありません。 | str: %s",
			str.c_str()
		));
	}
}

// メッシュを読み込む
void Systems::Resources::LoadMeshes(const std::wstring& directoryPath)
{
	// パスが存在しないなら
	if (!std::filesystem::exists(directoryPath))
	{
		// エラーメッセージを追加
		IErrorMessage::GetInstance()->AddMessage(Utility::FormatWString
		(
			L"パスが間違っています。 | path: %s",
			directoryPath.c_str()
		));
		return;
	}

	// ディレクトリ内を全て検索
	for (const auto& entry : std::filesystem::recursive_directory_iterator(directoryPath))
	{
		// ファイルなら
		if (entry.is_regular_file())
		{
			std::string meshName = entry.path().stem().string();
			m_meshes.emplace(meshName, Mesh{});

			auto it = m_meshes.find(meshName);

			if (!it->second.Load(entry.path().string()))
			{
				m_meshes.erase(it);
				// エラーメッセージを追加
				IErrorMessage::GetInstance()->AddMessage(Utility::FormatWString
				(
					L"メッシュの読み込みに失敗しました。 | path: %s",
					entry.path().c_str()
				));
			}
		}
	}
}

// 頂点シェーダを読み込む
void Systems::Resources::LoadVertexShaders(ID3D11Device5* device, const std::wstring& directoryPath)
{
	// パスが存在しないなら
	if (!std::filesystem::exists(directoryPath))
	{
		// エラーメッセージを追加
		IErrorMessage::GetInstance()->AddMessage(Utility::FormatWString
		(
			L"パスが間違っています。 | path: %s",
			directoryPath.c_str()
		));
		return;
	}

	// ディレクトリ内を全て検索
	for (const auto& entry : std::filesystem::recursive_directory_iterator(directoryPath))
	{
		// ファイルなら
		if (entry.is_regular_file())
		{
			try
			{
				m_vertexShaders.emplace(entry.path().stem().string(), Renderings::VertexShader::Create(device, entry.path().wstring()));
			}
			catch (std::exception e)
			{
				// エラーメッセージを追加
				IErrorMessage::GetInstance()->AddMessage(Utility::FormatWString
				(
					L"ピクセルシェーダの読み込みに失敗しました。 | path: %s",
					entry.path().c_str()
				));
			}
		}
	}
}

// ピクセルシェーダを読み込む
void Systems::Resources::LoadPixelShaders(ID3D11Device5* device, const std::wstring& directoryPath)
{
	// パスが存在しないなら
	if (!std::filesystem::exists(directoryPath))
	{
		// エラーメッセージを追加
		IErrorMessage::GetInstance()->AddMessage(Utility::FormatWString
		(
			L"パスが間違っています。 | path: %s",
			directoryPath.c_str()
		));
		return;
	}

	// ディレクトリ内を全て検索
	for (const auto& entry : std::filesystem::recursive_directory_iterator(directoryPath))
	{
		// ファイルなら
		if (entry.is_regular_file())
		{
			try
			{
				m_pixelShaders.emplace(entry.path().stem().string(), Renderings::PixelShader::Create(device, entry.path().wstring()));
			}
			catch (std::exception e)
			{
				// エラーメッセージを追加
				IErrorMessage::GetInstance()->AddMessage(Utility::FormatWString
				(
					L"ピクセルシェーダの読み込みに失敗しました。 | path: %s",
					entry.path().c_str()
				));
			}
		}
	}
}

// モデルの取得
Ref<const Renderings::Model3DSource> Systems::Resources::GetModelSource(const std::string& modelName)
{
	return GetEditableModelSource(modelName).Const();
}

// 画像の取得
Ref<const Renderings::ImageSource> Systems::Resources::GetImageSource(const std::string& imageName)
{
	return GetEditableImageSource(imageName).Const();
}

// Jsonの取得
Ref<const nlohmann::ordered_json> Systems::Resources::GetJson(const std::string& jsonName)
{
	return GetEditableJson(jsonName).Const();
}

// メッシュの取得
Ref<const Mesh> Systems::Resources::GetMesh(const std::string& meshName)
{
	return GetEditableMesh(meshName).Const();
}

// 頂点シェーダの取得
Ref<const Renderings::VertexShader> Systems::Resources::GetVertexShader(const std::string& shaderName)
{
	return GetEditableVertexShader(shaderName).Const();
}

// ピクセルシェーダの取得
Ref<const Renderings::PixelShader> Systems::Resources::GetPixelShader(const std::string& shaderName)
{
	return GetEditablePixelShader(shaderName).Const();
}

// モデルの取得
Ref<Renderings::Model3DSource> Systems::Resources::GetEditableModelSource(const std::string& modelName)
{
	auto it = m_modelSources.find(modelName);
	if (it == m_modelSources.end())
	{
		// エラーメッセージを追加
		IErrorMessage::GetInstance()->AddMessage(Utility::FormatWString
		(
			L"モデルが見つかりません。 | name: %s",
			Utility::string2wstring(modelName).c_str()
		));
		return Ref{ m_nullModelSource };
	}

	return Ref{ it->second };
}

// 画像の取得
Ref<Renderings::ImageSource> Systems::Resources::GetEditableImageSource(const std::string& imageName)
{
	auto it = m_imageSources.find(imageName);
	if (it == m_imageSources.end())
	{
		// エラーメッセージを追加
		IErrorMessage::GetInstance()->AddMessage(Utility::FormatWString
		(
			L"画像が見つかりません。 | name: %s",
			Utility::string2wstring(imageName).c_str()
		));
		return Ref{ m_nullImageSource };
	}

	return Ref{ it->second };
}

// Jsonの取得
Ref<nlohmann::ordered_json> Systems::Resources::GetEditableJson(const std::string& jsonName)
{
	auto it = m_jsons.find(jsonName);
	if (it == m_jsons.end())
	{
		// エラーメッセージを追加
		IErrorMessage::GetInstance()->AddMessage(Utility::FormatWString
		(
			L"Jsonファイルが見つかりません。 | name: %s",
			Utility::string2wstring(jsonName).c_str()
		));
		return Ref{ m_nullJson };
	}

	return Ref{ it->second };
}

// メッシュの取得
Ref<Mesh> Systems::Resources::GetEditableMesh(const std::string& meshName)
{
	auto it = m_meshes.find(meshName);
	if (it == m_meshes.end())
	{
		// エラーメッセージを追加
		IErrorMessage::GetInstance()->AddMessage(Utility::FormatWString
		(
			L"メッシュが見つかりません。 | name: %s",
			Utility::string2wstring(meshName).c_str()
		));
		return Ref{ m_nullMesh };
	}

	return Ref{ it->second };
}

// 頂点シェーダの取得
Ref<Renderings::VertexShader> Systems::Resources::GetEditableVertexShader(const std::string& shaderName)
{
	auto it = m_vertexShaders.find(shaderName);
	if (it == m_vertexShaders.end())
	{
		// エラーメッセージを追加
		IErrorMessage::GetInstance()->AddMessage(Utility::FormatWString
		(
			L"ピクセルシェーダが見つかりません。 | name: %s",
			Utility::string2wstring(shaderName).c_str()
		));
		return Ref{ m_nullVertexShader };
	}

	return Ref{ it->second };
}

// ピクセルシェーダの取得
Ref<Renderings::PixelShader> Systems::Resources::GetEditablePixelShader(const std::string& shaderName)
{
	auto it = m_pixelShaders.find(shaderName);
	if (it == m_pixelShaders.end())
	{
		// エラーメッセージを追加
		IErrorMessage::GetInstance()->AddMessage(Utility::FormatWString
		(
			L"ピクセルシェーダが見つかりません。 | name: %s",
			Utility::string2wstring(shaderName).c_str()
		));
		return Ref{ m_nullPixelShader };
	}

	return Ref{ it->second };
}
