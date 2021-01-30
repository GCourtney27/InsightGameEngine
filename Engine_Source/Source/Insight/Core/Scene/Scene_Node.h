#pragma once

#include "Insight/Core.h"

#include "Insight/Math/Transform.h"


#include <rapidjson/json.h>
#include <rapidjson/writer.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/filewritestream.h>
namespace Insight {
	
	class Scene;

	class INSIGHT_API SceneNode
	{
	public:
		SceneNode(std::string displayName = "Default Scene Node");
		virtual ~SceneNode();

		void SetParent(SceneNode* parent) 
		{
			if (m_Parent) { m_Parent = nullptr; } 
			m_Parent = parent;
		}

		const char* GetDisplayName() { return m_DisplayName.c_str(); }
		void SetDisplayName(std::string Name) { m_DisplayName = Name; }
		void SetCanBeFileParsed(bool CanBeParsed) { m_CanBeFileParsed = CanBeParsed; }

		void ResizeNumChildren(size_t NumChildren) { m_Children.reserve(NumChildren); }
		uint32_t GetNumChildrenNodes() { return static_cast<uint32_t>(m_Children.size()); }

		void AddChild(SceneNode* childNode);
		void RemoveChild(SceneNode* ChildNode);
		std::vector<SceneNode*>::const_iterator GetChildIteratorStart() { return m_Children.begin(); }
		std::vector<SceneNode*>::const_iterator GetChildIteratorEnd() { return m_Children.end(); }

		virtual bool WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>* Writer);
		virtual bool LoadFromJson(const rapidjson::Value* JsonActor);

		virtual void RenderSceneHeirarchy();
		virtual bool OnInit();
		virtual bool OnPostInit();
		virtual void OnUpdate(const float DeltaMs);
		virtual void OnRender();
		virtual void Destroy();

		virtual void BeginPlay();
		virtual void Tick(const float DeltaMs);
		virtual void Exit();

		virtual void EditorEndPlay();

		std::vector<SceneNode*> m_Children;
	protected:
		SceneNode* m_Parent = nullptr;
		std::string m_DisplayName;
		bool m_CanBeFileParsed = true;
	};

}
