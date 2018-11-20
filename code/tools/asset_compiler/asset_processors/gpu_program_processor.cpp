// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "../common.h"
#include "../asset_processor.h"
#include "gpu_program_processor.h"
#include "base/logger.h"
#include "base/util.h"
#include "base/platform.h"
#include "base/json.h"
#include "base/mutex.h"
#include "base/stream.h"
#include "base/file.h"
#include "graphics/types.h"
#include "../project.h"

namespace ac
{
using namespace base;
using namespace engine;

B_BEGIN_ENUM_STRINGIFY(GpuProgramConstantType)
	B_ADD_ENUM_STRINGIFY("", GpuProgramConstantType::Unknown)
	B_ADD_ENUM_STRINGIFY("float", GpuProgramConstantType::Float)
	B_ADD_ENUM_STRINGIFY("int", GpuProgramConstantType::Integer)
	B_ADD_ENUM_STRINGIFY("matrix", GpuProgramConstantType::Matrix)
	B_ADD_ENUM_STRINGIFY("vec2", GpuProgramConstantType::Vec2)
	B_ADD_ENUM_STRINGIFY("vec3", GpuProgramConstantType::Vec3)
	B_ADD_ENUM_STRINGIFY("color", GpuProgramConstantType::Color)
	B_ADD_ENUM_STRINGIFY("texture", GpuProgramConstantType::Texture)

	B_ADD_ENUM_STRINGIFY("world", GpuProgramConstantType::WorldMatrix)
	B_ADD_ENUM_STRINGIFY("worldInverse", GpuProgramConstantType::WorldInverseMatrix)
	B_ADD_ENUM_STRINGIFY("worldInverseTranspose", GpuProgramConstantType::WorldInverseTransposeMatrix)
	B_ADD_ENUM_STRINGIFY("view", GpuProgramConstantType::ViewMatrix)
	B_ADD_ENUM_STRINGIFY("viewInverse", GpuProgramConstantType::ViewInverseMatrix)
	B_ADD_ENUM_STRINGIFY("viewInverseTranspose", GpuProgramConstantType::ViewInverseTransposeMatrix)
	B_ADD_ENUM_STRINGIFY("viewRotation", GpuProgramConstantType::ViewRotationMatrix)
	B_ADD_ENUM_STRINGIFY("worldView", GpuProgramConstantType::WorldViewMatrix)
	B_ADD_ENUM_STRINGIFY("worldViewInverse", GpuProgramConstantType::WorldViewInverseMatrix)
	B_ADD_ENUM_STRINGIFY("worldViewInverseTranspose", GpuProgramConstantType::WorldViewInverseTransposeMatrix)
	B_ADD_ENUM_STRINGIFY("projection", GpuProgramConstantType::ProjectionMatrix)
	B_ADD_ENUM_STRINGIFY("worldProjection", GpuProgramConstantType::WorldProjectionMatrix)
	B_ADD_ENUM_STRINGIFY("viewProjection", GpuProgramConstantType::ViewProjectionMatrix)
	B_ADD_ENUM_STRINGIFY("worldViewProjection", GpuProgramConstantType::WorldViewProjectionMatrix)
	B_ADD_ENUM_STRINGIFY("textureMatrix", GpuProgramConstantType::TextureMatrix)
	
	B_ADD_ENUM_STRINGIFY("engineTime", GpuProgramConstantType::EngineTime)
	B_ADD_ENUM_STRINGIFY("engineFrameDeltaTime", GpuProgramConstantType::EngineFrameDeltaTime)
	B_ADD_ENUM_STRINGIFY("unitDomainRotatorTime", GpuProgramConstantType::UnitDomainRotatorTime)
	B_ADD_ENUM_STRINGIFY("engineFps", GpuProgramConstantType::EngineFps)
	B_ADD_ENUM_STRINGIFY("renderTargetSize", GpuProgramConstantType::RenderTargetSize)
	B_ADD_ENUM_STRINGIFY("viewportSize", GpuProgramConstantType::ViewportSize)
	B_ADD_ENUM_STRINGIFY("cameraFov", GpuProgramConstantType::CameraFov)
	B_ADD_ENUM_STRINGIFY("cameraNearPlane", GpuProgramConstantType::CameraNearPlane)
	B_ADD_ENUM_STRINGIFY("cameraFarPlane", GpuProgramConstantType::CameraFarPlane)
	B_ADD_ENUM_STRINGIFY("cameraUpAxis", GpuProgramConstantType::CameraUpAxis)
	B_ADD_ENUM_STRINGIFY("cameraPosition", GpuProgramConstantType::CameraPosition)
	B_ADD_ENUM_STRINGIFY("colorTexture", GpuProgramConstantType::ColorTexture)
	B_ADD_ENUM_STRINGIFY("depthTexture", GpuProgramConstantType::DepthTexture)
	B_ADD_ENUM_STRINGIFY("systemTexture", GpuProgramConstantType::SystemTexture)
B_END_ENUM_STRINGIFY(GpuProgramConstantType)

B_BEGIN_ENUM_STRINGIFY(GraphicsApiType)
	B_ADD_ENUM_STRINGIFY("ogl", GraphicsApiType::OpenGL)
	B_ADD_ENUM_STRINGIFY("vulkan", GraphicsApiType::Vulkan)
	B_ADD_ENUM_STRINGIFY("direct3d", GraphicsApiType::Direct3D)
	B_ADD_ENUM_STRINGIFY("metal", GraphicsApiType::Metal)
B_END_ENUM_STRINGIFY(GraphicsApiType)

static void CgErrorHandler(CGcontext context, CGerror error, void* appdata)
{
	const char* errStr = cgGetErrorString(error);

	if (error != CG_NO_ERROR && context)
	{
		if (error == CG_COMPILER_ERROR)
		{
			String strErrors = cgGetLastListing(context);
			Array<String> strings;

			explodeString(strErrors, strings, "\r\n");

			B_LOG_ERROR("Cg Compiler Error: " << errStr);

			for (auto& str : strings)
			{
				B_LOG_ERROR("CgError: " << str);
			}
		}
		else
		{
			B_LOG_ERROR("Cg error " << error << ": " << errStr);
		}
	}
}

void CgErrorCallback()
{}

static String currentShaderFilename;
static Mutex includeLock;
// filename-source code pairs
static Dictionary<String, String> includeFiles;

static void CgIncludeCallback(CGcontext context, const char* filename)
{
	AutoLock<Mutex> lock(includeLock);
	String path, fullPath;
	String code;

	path = getFilenamePath(currentShaderFilename);
	B_LOG_DEBUG("Including gpu program file: '" << filename << "' (path: '" << path << "' using: '" << currentShaderFilename << "')");
	fullPath = mergePathName(path, filename);
	fullPath = beautifyPath(fullPath);

	auto iter = includeFiles.find(fullPath);

	if (iter == includeFiles.end())
	{
		if (!loadTextFile(fullPath, code))
		{
			B_LOG_ERROR("Could not open '" << filename << "', from fullpath '" << fullPath << "'");
			return;
		}

		includeFiles[fullPath] = code;
	}
	else
	{
		code = iter->value;
	}

	B_LOG_DEBUG("Including Cg file '" << filename << "', from fullpath '" << fullPath << "'");
	cgSetCompilerIncludeString(context, filename, code.c_str());
}

void processCodeNames(String& strCode)
{
	String code = strCode;
	size_t crtIndex = 0;
	String semantic;

	//TODO HACKY!
	while (true)
	{
		crtIndex = code.find("//semantic", crtIndex);
		
		if (crtIndex == String::noIndex)
		{
			break;
		}

		size_t endStrIndex = code.find("\n", crtIndex + 11);

		if (endStrIndex == String::noIndex)
		{
			crtIndex += 11;
			continue;
		}

		semantic = code.subString(crtIndex + 11, endStrIndex - (crtIndex + 11));
		strCode.replace(("_" + semantic), semantic);
		crtIndex += 11;
	}
}

GpuProgramProcessor::GpuProgramProcessor()
{
	Array<String> extensions;

	extensions.append(".gpu_program");
	supportedAssetInfo = SupportedAssetInfo(ResourceType::GpuProgram, extensions);
	cgSetErrorHandler(CgErrorHandler, nullptr);
	cgSetErrorCallback(CgErrorCallback);

#ifdef AC_USE_OPENGL
	cgContextGL = cgCreateContext();
	cgGLRegisterStates(cgContextGL);
	cgGLSetManageTextureParameters(cgContextGL, CG_TRUE);
	cgSetCompilerIncludeCallback(cgContextGL, CgIncludeCallback);
#endif
}

GpuProgramProcessor::~GpuProgramProcessor()
{
	cgSetErrorHandler(nullptr, nullptr);
#ifdef AC_USE_OPENGL
	cgDestroyContext(cgContextGL);
#endif
}

void GpuProgramProcessor::compileAndWriteProgram(
	CGcontext context,
	Stream* stream,
	const char** args,
	CGprofile profile,
	const String& shaderSrc,
	const String& mainEntry)
{
	if (mainEntry == "")
	{
		stream->writeString("");
		return;
	}

	CGprogram cgProg = cgCreateProgram(context, CG_SOURCE, shaderSrc.c_str(), profile, mainEntry.c_str(), args);

	cgCompileProgram(cgProg);
	String outputCode = cgGetProgramString(cgProg, CG_COMPILED_PROGRAM);
	cgDestroyProgram(cgProg);
	// cleanup code
	processCodeNames(outputCode);
	stream->writeString(outputCode);
}

bool GpuProgramProcessor::import(const String& importFilename, JsonDocument& assetCfg)
{
	assetCfg.addValue("type", "gpu_program");

	return true;
}

bool GpuProgramProcessor::process(Asset& asset, JsonDocument& assetCfg)
{
	String filename, crtPath;
	JsonDocument gpuProg(asset.absoluteName);

	auto cfg = asset.project->currentBuildConfig->processorConfigs["gpu_program"].cfg;
	GraphicsApiType gfxApi = GraphicsApiType::OpenGL;

	if (cfg)
	{
		gfxApi = (GraphicsApiType)B_STRING_TO_ENUM(GraphicsApiType, cfg->getString("api", "ogl"));
	}

	File file;

	if (!file.open(asset.absoluteOutputFilename, FileOpenFlags::BinaryWrite))
		return false;

	crtPath = getFilenamePath(asset.absoluteName);

	file << gfxApi;
	// options
	file << (u32)0; //TODO: from enum/flags

	String shaderSourceFile, program, outputCode;

	const String& vertexMain = gpuProg.getString("vertexMain");
	const String& pixelMain = gpuProg.getString("pixelMain");
	const String& geometryMain = gpuProg.getString("geometryMain");
	const String& hullMain = gpuProg.getString("hullMain");
	const String& domainMain = gpuProg.getString("domainMain");

	if (gpuProg.getRoot().hasKey("source"))
	{
		shaderSourceFile = gpuProg.getString("source");
		shaderSourceFile = mergePathName(crtPath, shaderSourceFile);
		currentShaderFilename = shaderSourceFile;

		if (!loadTextFile(shaderSourceFile, program))
		{
			return false;
		}
	}
	else
	{
		B_LOG_ERROR("No shader source file provided for shader: '" << asset.name << "'");
		return false;
	}

#ifdef AC_USE_OPENGL
	// OpenGL 3.0 and up
	if (gfxApi == GraphicsApiType::OpenGL)
	{
		const char* args[] = { "version=150", "userTexCoord", 0 };
		compileAndWriteProgram(cgContextGL, &file, args, CG_PROFILE_GLSLV, program, vertexMain);
		compileAndWriteProgram(cgContextGL, &file, args, CG_PROFILE_GLSLF, program, pixelMain);
		compileAndWriteProgram(cgContextGL, &file, args, CG_PROFILE_GLSLG, program, geometryMain);
		compileAndWriteProgram(cgContextGL, &file, args, CG_PROFILE_GP5TCP, program, domainMain);
		compileAndWriteProgram(cgContextGL, &file, args, CG_PROFILE_GP5TEP, program, hullMain);
	}
#elif AC_USE_VULKAN
#endif

	JsonObject* jsnConstants = gpuProg.getObject("constants");

	// constants count
	file << (u32)(jsnConstants ? jsnConstants->getMembers().size() : 0);

	if (jsnConstants)
	{
		for (size_t j = 0; j < jsnConstants->getMembers().size(); ++j)
		{
			JsonValue* jsnConstant = jsnConstants->getMembers()[j].value;

			if (jsnConstant->isObject())
			{
				file << (u32)jsnConstant->asObject()->getI32("count", 1);
				GpuProgramConstantType constType =
					(GpuProgramConstantType)B_STRING_TO_ENUM(GpuProgramConstantType, jsnConstant->asObject()->getString("type"));
				file << constType;
				file << jsnConstants->getMembers()[j].key;

				auto val = jsnConstant->asObject()->getValueOf("value");

				//TODO: also must handle array of constants
				switch (constType)
				{
				case GpuProgramConstantType::Color:
					file << toColor(val->asString());
					break;
				case GpuProgramConstantType::Float:
					file << val->asF32();
					break;
				case GpuProgramConstantType::Integer:
					file << val->asI32();
					break;
				case GpuProgramConstantType::Matrix:
					file << toMatrix(val->asString());
					break;
				case GpuProgramConstantType::Texture:
					file << (u64)toResourceId(val->asString());
					break;
				case GpuProgramConstantType::Vec2:
					file << toVec2(val->asString());
					break;
				case GpuProgramConstantType::Vec3:
					file << toVec3(val->asString());
					break;
				default:
					break;
				}
			}
			else
			{
				B_LOG_ERROR("Expecting gpu program constant object in JSON: " << asset.absoluteName);
				return false;
			}
		}
	}

	file.close();
	
	B_LOG_DEBUG("End gpu program converter");

	return true;
}

bool GpuProgramProcessor::isModified(Asset& asset)
{
	//TODO: need to check all .cg files, included recursive if datetime is different
	// need to keep a cache for them... add new processor for .cg?
	// for now, always compile gpu programs
	return true;
}


}