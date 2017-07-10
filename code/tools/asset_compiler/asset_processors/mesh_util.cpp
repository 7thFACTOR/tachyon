#include "mesh_util.h"
#include "base/math/conversion.h"
#include "base/math/util.h"
#include "base/logger.h"
#include "base/platform.h"
#include "base/json.h"
#include "base/util.h"

namespace ac
{
void logDebugMatrix(const Matrix& m)
{
	B_LOG_INFO("(" << m.m[0][0] << "," << m.m[0][1] << "," << m.m[0][2] << "," << m.m[0][3] << ")");
	B_LOG_INFO("(" << m.m[1][0] << "," << m.m[1][1] << "," << m.m[1][2] << "," << m.m[1][3] << ")");
	B_LOG_INFO("(" << m.m[2][0] << "," << m.m[2][1] << "," << m.m[2][2] << "," << m.m[2][3] << ")");
	B_LOG_INFO("(" << m.m[3][0] << "," << m.m[3][1] << "," << m.m[3][2] << "," << m.m[3][3] << ")");
	B_LOG_INFO("---------------------------------------------------------------------------------");
}

void logDebugVec(const Vec3& v)
{
	B_LOG_INFO("V(" << v.x << "," << v.y << "," << v.z << ")");
	B_LOG_INFO("---------------------------------------------------------------------------------");
}

String f32ToString(f32 value)
{
	if (fabs(value) <= 0.0000001f)
	{
		return "0";
	}

	int ival = (int)value;
	f32 frac = value - (f32)ival;
	
	if (fabs(frac) <= 0.0000001f)
	{
		return toString(ival);
	}

	return toString(value);
}

Vec3 toEngine(const Vec3& v)
{
	Vec3 vv;

	vv.x = -v.x;
	vv.z = v.y;
	vv.y = v.z;

	return vv;
}

ImportMesh::ImportMesh()
{}

ImportMesh::~ImportMesh()
{
	free();
}

void ImportMesh::free()
{
	geometryBuffer.indices.clear();
	geometryBuffer.vertices.clear();

	for (size_t i = 0; i < elements.size(); i++)
	{
		delete elements[i];
	}

	elements.clear();
}

}