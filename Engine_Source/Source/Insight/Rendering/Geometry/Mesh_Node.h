#pragma once

#include <Insight/Core.h>

namespace Insight {

	class INSIGHT_API MeshNode
	{
	public:
		MeshNode(std::vector<Mesh*> meshChildren, ieTransform transform, std::string displayName = "Default Mesh Node")
			: m_MeshChildren(meshChildren), m_Transform(transform), m_DisplayName(displayName) {}
		~MeshNode() {}

		void PreRender(XMMATRIX& parentMat, UINT32& gpuAddressOffset);
		void Render();
		void RenderSceneHeirarchy();

		ieTransform& GetTransformRef() { return m_Transform; }
		const ieTransform& GetTransform() const { return m_Transform; }
		void AddChild(std::unique_ptr<MeshNode> child);
		int GetNumChildren() { return (int)m_MeshChildren.size(); }

	private:
		std::vector<std::unique_ptr<MeshNode>> m_Children;
		std::vector<Mesh*> m_MeshChildren;
		ieTransform m_Transform;
		std::string m_DisplayName;
		
		int ConstantBufferPerObjectAlignedSize = (sizeof(CB_VS_PerObject) + 255) & ~255;
	};

}
