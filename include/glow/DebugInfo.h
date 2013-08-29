#pragma once

#include <glow/ObjectVisitor.h>

#include <set>
#include <string>
#include <vector>
#include <map>

namespace glow
{

class DebugInfo : public ObjectVisitor
{
public:
	struct Property
	{
		std::string name;
		std::string value;
	};
	struct InfoUnit
	{
		std::string name;
		std::vector<Property> properties;

		void addProperty(const std::string& name, const std::string& value);
		void addProperty(const std::string& name, GLint value);
	};
	struct InfoGroup
	{
		std::string name;
		std::vector<InfoUnit> units;

		void addInfoUnit(const InfoUnit& info);
	};
public:
	static std::vector<InfoGroup> generalInfo();
	static std::vector<InfoGroup> objectInfo();

	static void printObjectInfo();
	static void printGeneralInfo();
	static void printAll();

	static void print(const std::vector<InfoGroup>& info);
public:
	virtual void visitBuffer(Buffer* buffer);
	virtual void visitFrameBufferObject(FrameBufferObject* fbo);
	virtual void visitProgram(Program* program);
	virtual void visitRenderBufferObject(RenderBufferObject* rbo);
	virtual void visitShader(Shader* shader);
	virtual void visitTexture(Texture* texture);
	virtual void visitTransformFeedback(TransformFeedback* transformfeedback);
	virtual void visitVertexArrayObject(VertexArrayObject* vao);
protected:
	DebugInfo();
	virtual ~DebugInfo();
protected:
	std::map<std::string, InfoGroup> m_infoGroups;

	std::vector<InfoGroup> collectObjectInfo();

	InfoGroup& group(const std::string& name);
	void addInfo(const std::string& groupName, const InfoUnit& unit);

	static std::string name(const std::string& typeName, Object* object);
	static std::string humanReadableSize(long long bytes);
};

} // namespace glow
