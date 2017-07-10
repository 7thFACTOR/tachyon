void OpenGLGpuProgram::commitConstants()
{
	f32 fValue;
	u32 iValue;
	i32 crtTexStage = 0;

	for (auto& constant : methodConstants)
	{
		switch (constant.getType())
		{
		case GpuProgramConstantType::WorldMatrix:
			{
				setMatrixValueInGpuProgram(
					oglCrtRenderMethod,
					methodConstants[i].getName().c_str(),
					oglGraphics->worldMatrix);
				break;
			}

		case GpuProgramConstantType::WorldInverseMatrix:
			{
				if (oglGraphics->dirtyMatrixFlags.worldInverse)
				{
					oglGraphics->worldInverseMatrix = oglGraphics->worldMatrix;
					oglGraphics->worldInverseMatrix.invert();
					oglGraphics->dirtyMatrixFlags.worldInverse = 0;
				}

				setMatrixValueInGpuProgram(
						oglCrtRenderMethod,
						methodConstants[i].getName().c_str(),
						oglGraphics->worldInverseMatrix);

				break;
			}

		case GpuProgramConstantType::WorldInverseTransposeMatrix:
			{
				if (oglGraphics->dirtyMatrixFlags.worldInverseTranspose)
				{
					oglGraphics->worldInverseTransposeMatrix = oglGraphics->worldMatrix;
					oglGraphics->worldInverseTransposeMatrix.invert();
					oglGraphics->worldInverseTransposeMatrix.transpose();
					oglGraphics->dirtyMatrixFlags.worldInverseTranspose = 0;
				}

				setMatrixValueInGpuProgram(
						oglCrtRenderMethod,
						methodConstants[i].getName().c_str(),
						oglGraphics->worldInverseTransposeMatrix);

				break;
			}

		case GpuProgramConstantType::ViewMatrix:
			{
				setMatrixValueInGpuProgram(
						oglCrtRenderMethod,
						methodConstants[i].getName().c_str(),
						oglGraphics->viewMatrix);
				break;
			}

		case GpuProgramConstantType::ViewInverseMatrix:
			{
				if (oglGraphics->dirtyMatrixFlags.viewInverse)
				{
					oglGraphics->viewInverseMatrix = oglGraphics->viewMatrix;
					oglGraphics->viewInverseMatrix.invert();
					oglGraphics->dirtyMatrixFlags.viewInverse = 0;
				}

				setMatrixValueInGpuProgram(
						oglCrtRenderMethod,
						methodConstants[i].getName().c_str(),
						oglGraphics->viewInverseMatrix);
				break;
			}

		case GpuProgramConstantType::ViewInverseTransposeMatrix:
			{
				if (oglGraphics->dirtyMatrixFlags.viewInverseTranspose)
				{
					oglGraphics->viewInverseTransposeMatrix = oglGraphics->viewMatrix;
					oglGraphics->viewInverseTransposeMatrix.invert();
					oglGraphics->viewInverseTransposeMatrix.transpose();
					oglGraphics->dirtyMatrixFlags.viewInverseTranspose = 0;
				}

				setMatrixValueInGpuProgram(
					oglCrtRenderMethod,
					methodConstants[i].getName().c_str(),
					oglGraphics->viewInverseTransposeMatrix);

				break;
			}

		case GpuProgramConstantType::ViewRotationMatrix:
			{
				Matrix mtx = getGraphics().getViewMatrix();
				mtx = mtx.getRotationOnly();

				if (oglGraphics->dirtyMatrixFlags.viewRotation)
				{
					oglGraphics->viewRotationMatrix = oglGraphics->viewMatrix.getRotationOnly();
					oglGraphics->dirtyMatrixFlags.viewRotation = 0;
				}

				setMatrixValueInGpuProgram(
					oglCrtRenderMethod,
					methodConstants[i].getName().c_str(),
					oglGraphics->viewRotationMatrix);

				break;
			}

		case GpuProgramConstantType::WorldViewMatrix:
			{
				if (oglGraphics->dirtyMatrixFlags.worldView)
				{
					oglGraphics->worldViewMatrix = oglGraphics->worldMatrix;
					oglGraphics->worldViewMatrix = oglGraphics->worldViewMatrix * oglGraphics->viewMatrix;
					oglGraphics->dirtyMatrixFlags.worldView = 0;
				}

				setMatrixValueInGpuProgram(
						oglCrtRenderMethod,
						methodConstants[i].getName().c_str(),
						oglGraphics->worldViewMatrix);

				break;
			}

		case GpuProgramConstantType::WorldViewInverseMatrix:
			{
				if (oglGraphics->dirtyMatrixFlags.worldViewInverse)
				{
					oglGraphics->worldViewInverseMatrix = oglGraphics->worldMatrix;
					oglGraphics->worldViewInverseMatrix = oglGraphics->worldViewInverseMatrix * oglGraphics->viewMatrix;
					oglGraphics->worldViewInverseMatrix.invert();
					oglGraphics->dirtyMatrixFlags.worldViewInverse = 0;
				}

				setMatrixValueInGpuProgram(
						oglCrtRenderMethod,
						methodConstants[i].getName().c_str(),
						oglGraphics->worldViewInverseMatrix);

				break;
			}

		case GpuProgramConstantType::WorldViewInverseTransposeMatrix:
			{
				if (oglGraphics->dirtyMatrixFlags.worldViewInverseTranspose)
				{
					oglGraphics->worldViewInverseTransposeMatrix = oglGraphics->worldMatrix;
					oglGraphics->worldViewInverseTransposeMatrix = oglGraphics->worldViewInverseTransposeMatrix * oglGraphics->viewMatrix;
					oglGraphics->worldViewInverseTransposeMatrix.invert();
					oglGraphics->worldViewInverseTransposeMatrix.transpose();
					oglGraphics->dirtyMatrixFlags.worldViewInverseTranspose = 0;
				}

				setMatrixValueInGpuProgram(
					oglCrtRenderMethod,
					methodConstants[i].getName().c_str(),
					oglGraphics->worldViewInverseTransposeMatrix);

				break;
			}

		case GpuProgramConstantType::ProjectionMatrix:
			{
				setMatrixValueInGpuProgram(
						oglCrtRenderMethod,
						methodConstants[i].getName().c_str(),
						oglGraphics->projectionMatrix);
				break;
			}

		case GpuProgramConstantType::ViewProjectionMatrix:
			{
				if (oglGraphics->dirtyMatrixFlags.viewProjection)
				{
					oglGraphics->viewProjectionMatrix = oglGraphics->viewMatrix;
					oglGraphics->viewProjectionMatrix = oglGraphics->viewProjectionMatrix * oglGraphics->projectionMatrix;
					oglGraphics->dirtyMatrixFlags.viewProjection = 0;
				}

				setMatrixValueInGpuProgram(
						oglCrtRenderMethod,
						methodConstants[i].getName().c_str(),
						oglGraphics->viewProjectionMatrix);

				break;
			}

		case GpuProgramConstantType::WorldProjectionMatrix:
			{
				if (oglGraphics->dirtyMatrixFlags.worldProjection)
				{
					oglGraphics->worldProjectionMatrix = oglGraphics->worldMatrix;
					oglGraphics->worldProjectionMatrix = oglGraphics->worldProjectionMatrix * oglGraphics->projectionMatrix;
					oglGraphics->dirtyMatrixFlags.worldProjection = 0;
				}

				setMatrixValueInGpuProgram(
						oglCrtRenderMethod,
						methodConstants[i].getName().c_str(),
						oglGraphics->worldProjectionMatrix);
				break;
			}

		case GpuProgramConstantType::WorldViewProjectionMatrix:
			{
				if (oglGraphics->dirtyMatrixFlags.worldViewProjection)
				{
					oglGraphics->worldViewProjectionMatrix = oglGraphics->worldMatrix;
					oglGraphics->worldViewProjectionMatrix = oglGraphics->worldViewProjectionMatrix * oglGraphics->viewMatrix;
					oglGraphics->worldViewProjectionMatrix = oglGraphics->worldViewProjectionMatrix * oglGraphics->projectionMatrix;
					oglGraphics->dirtyMatrixFlags.worldViewProjection = 0;
				}

				setMatrixValueInGpuProgram(
						oglCrtRenderMethod,
						methodConstants[i].getName().c_str(),
						oglGraphics->worldViewProjectionMatrix);

				break;
			}

		case GpuProgramConstantType::EngineTime:
			{
				iValue = getClock().getTimer(SystemTimer::game)->getTime();
				setIntValueInGpuProgram(
						oglCrtRenderMethod,
						methodConstants[i].getName().c_str(),
						iValue);
				break;
			}

		case GpuProgramConstantType::EngineFrameDeltaTime:
			{
				fValue = getGameDeltaTime();
				setFloatValueInGpuProgram(
						oglCrtRenderMethod,
						methodConstants[i].getName().c_str(),
						fValue);
				break;
			}

		case GpuProgramConstantType::UnitDomainRotatorTime:
			{
				static f32 s_fValue = 0;

				s_fValue += getGameDeltaTime();

				if (s_fValue > 1.0f)
					s_fValue = 0.0f;

				setFloatValueInGpuProgram(
						oglCrtRenderMethod,
						methodConstants[i].getName().c_str(),
						s_fValue);
				break;
			}

		case GpuProgramConstantType::RenderTargetSize:
			{
				//IDirect3DSurface9* pSurf = nullptr;

				////TODO: render target for MRT index
				//if (!FAILED(s_pD3D9Manager->m_pD3DDev->GetRenderTarget(0, &pSurf)))
				//{
				//	if (pSurf)
				//	{
				//		f32 targetSize[2];
				//		D3DSURFACE_DESC desc;

				//		pSurf->GetDesc(&desc);
				//		targetSize[0] = (f32)desc.Width;
				//		targetSize[1] = (f32)desc.Height;
				//		setFloatArrayValueInShaderPass(m_pD3DCrtPass, rConstants[i].name().c_str(), targetSize, 2);
				//		pSurf->Release();
				//	}
				//}

				break;
			}

		case GpuProgramConstantType::ViewportSize:
			{
				f32 targetSize[2];

				targetSize[0] = getGraphics().getViewport().width;
				targetSize[1] = getGraphics().getViewport().height;
				setFloatArrayValueInGpuProgram(oglCrtRenderMethod, methodConstants[i].getName().c_str(), targetSize, 2);
				break;
			}

		case GpuProgramConstantType::EngineFps:
			{
				setFloatValueInGpuProgram(
						oglCrtRenderMethod,
						methodConstants[i].getName().c_str(),
						getGameFps());
				break;
			}

			//case ShaderConstant::eType_CameraFov :
			//	{
			//		if (s_pD3DProvider->camera())
			//		{
			//			CGparameter param = cgGetNamedEffectParameter(m_cgEffect, rConstants[i].name().c_str());

			//			if (param)
			//			{
			//				cgSetParameter1f(param, s_pD3DProvider->camera()->fov());
			//			}
			//		}

			//		break;
			//	}

			//case ShaderConstant::eType_CameraNearPlane :
			//	{
			//		if (s_pD3DProvider->camera())
			//		{
			//			CGparameter param = cgGetNamedEffectParameter(m_cgEffect, rConstants[i].name().c_str());

			//			if (param)
			//			{
			//				cgSetParameter1f(param, s_pD3DProvider->camera()->nearPlane());
			//			}
			//		}

			//		break;
			//	}

			//case ShaderConstant::eType_CameraFarPlane :
			//	{
			//		if (s_pD3DProvider->camera())
			//		{
			//			CGparameter param = cgGetNamedEffectParameter(m_cgEffect, rConstants[i].name().c_str());

			//			if (param)
			//			{
			//				cgSetParameter1f(param, s_pD3DProvider->camera()->farPlane());
			//			}
			//		}

			//		break;
			//	}

			//case ShaderConstant::eType_CameraUpAxis :
			//	{
			//		if (s_pD3DProvider->camera())
			//		{
			//			CGparameter param = cgGetNamedEffectParameter(m_cgEffect, rConstants[i].name().c_str());

			//			if (param)
			//			{
			//				cgSetParameterValuefr(param, 3, &s_pD3DProvider->camera()->upAxis().x);
			//			}
			//		}

			//		break;
			//	}
		case GpuProgramConstantType::CameraPosition :
			{
				Vec3 pos = getGraphics().getViewMatrix().getInverted().getTranslation();
				f32 fv[3] = {pos.x, pos.y, pos.z};

				setFloatArrayValueInGpuProgram(
						oglCrtRenderMethod,
						methodConstants[i].getName().c_str(),
						fv, 3);

				break;
			}

		case GpuProgramConstantType::Float:
			{
				if (!methodConstants[i].getData())
				{
					break;
				}

				if (methodConstants[i].getCount() == 1)
				{
					setFloatValueInGpuProgram(
						oglCrtRenderMethod,
						methodConstants[i].getName().c_str(),
						*((f32*)methodConstants[i].getData()));
				}
				else
				{
					setFloatArrayValueInGpuProgram(
						oglCrtRenderMethod,
						methodConstants[i].getName().c_str(),
						(f32*)methodConstants[i].getData(),
						methodConstants[i].getCount());
				}

				break;
			}

		case GpuProgramConstantType::Integer:
			{
				if (!methodConstants[i].getData())
					break;

				if (methodConstants[i].getCount() == 1)
				{
					setIntValueInGpuProgram(
						oglCrtRenderMethod,
						methodConstants[i].getName().c_str(),
						*((int*)methodConstants[i].getData()));
				}
				else
				{
					setIntArrayValueInGpuProgram(
						oglCrtRenderMethod,
						methodConstants[i].getName().c_str(),
						(int*)methodConstants[i].getData(),
						methodConstants[i].getCount());
				}

				break;
			}

		case GpuProgramConstantType::Matrix:
			{
				if (!methodConstants[i].getData())
					break;

				if (methodConstants[i].getCount() == 1)
				{
					setMatrixValueInGpuProgram(
						oglCrtRenderMethod,
						methodConstants[i].getName().c_str(),
						*((Matrix*)methodConstants[i].getData()));
				}
				else
				{
					setMatrixArrayValueInGpuProgram(
						oglCrtRenderMethod,
						methodConstants[i].getName().c_str(),
						(Matrix*)methodConstants[i].getData(),
						methodConstants[i].getCount());
				}

				break;
			}

		case GpuProgramConstantType::Vec2:
			{
				if (!methodConstants[i].getData())
					break;

				if (methodConstants[i].getCount() == 1)
				{
					setFloatArrayValueInGpuProgram(
						oglCrtRenderMethod,
						methodConstants[i].getName().c_str(),
						(f32*)methodConstants[i].getData(),
						2);
				}
				else
				{
					setFloatArrayValueInGpuProgram(
						oglCrtRenderMethod,
						methodConstants[i].getName().c_str(),
						(f32*)methodConstants[i].getData(),
						methodConstants[i].getCount() * 2);
				}

				break;
			}

		case GpuProgramConstantType::Vec3:
			{
				if (!methodConstants[i].getData())
					break;

				if (methodConstants[i].getCount() == 1)
				{
					setFloatArrayValueInGpuProgram(
						oglCrtRenderMethod,
						methodConstants[i].getName().c_str(),
						(f32*)methodConstants[i].getData(),
						3);
				}
				else
				{
					setFloatArrayValueInGpuProgram(
						oglCrtRenderMethod,
						methodConstants[i].getName().c_str(),
						(f32*)methodConstants[i].getData(),
						methodConstants[i].getCount() * 3);
				}

				break;
			}

		case GpuProgramConstantType::Color:
			{
				if (!methodConstants[i].getData())
					break;

				if (methodConstants[i].getCount() == 1)
				{
					setFloatArrayValueInGpuProgram(
						oglCrtRenderMethod,
						methodConstants[i].getName().c_str(),
						(f32*)methodConstants[i].getData(),
						4);
				}
				else
				{
					setFloatArrayValueInGpuProgram(
						oglCrtRenderMethod,
						methodConstants[i].getName().c_str(),
						(f32*)methodConstants[i].getData(),
						methodConstants[i].getCount() * 4);
				}

				break;
			}

		case GpuProgramConstantType::Bool:
			{
				if (!methodConstants[i].getData())
					break;

				if (methodConstants[i].getCount() == 1)
				{
					setIntValueInGpuProgram(
						oglCrtRenderMethod,
						methodConstants[i].getName().c_str(),
						*((int*)methodConstants[i].getData()));
				}
				else
				{
					setIntArrayValueInGpuProgram(
						oglCrtRenderMethod,
						methodConstants[i].getName().c_str(),
						(int*)methodConstants[i].getData(),
						methodConstants[i].getCount());
				}

				break;
			}

		case GpuProgramConstantType::ColorTexture:
			{
				//if (s_pD3D9Manager->m_pD3DBackBuffer)
				{
					//TODO:
					//setSamplerValueInShaderPass(
					//	m_pD3DCrtPass,
					//	rConstants[i].name().c_str(),
					//	s_pD3D9Manager->m_pD3DBackBuffer);
				}

				break;
			}

		case GpuProgramConstantType::Texture:
			{
				ResourceId resId = *((ResourceId*)methodConstants[i].getData());
				auto iter = getGraphics().getTextures().find(resId);

				if (iter == getGraphics().getTextures().end())
				{
					break;
				}

				OpenGLTexture* pTex = (OpenGLTexture*)iter->value;
				
				if (!pTex)
				{
					break;
				}

				setSamplerValueInGpuProgram(
					oglCrtRenderMethod,
					pTex,
					methodConstants[i].getName().c_str(),
					crtTexStage++);
				break;
			}

		case GpuProgramConstantType::SystemTexture :
			{
				OpenGLTexture* pTex =
					(OpenGLTexture*) oglGraphics->getSystemTexture((*(String*)methodConstants[i].getData()).c_str());

				if (!pTex)
				{
					break;
				}

				setSamplerValueInGpuProgram(
					oglCrtRenderMethod,
					pTex,
					methodConstants[i].getName().c_str(),
					0);
				break;
			}

		case GpuProgramConstantType::TextureMatrix:
			{
				setMatrixValueInGpuProgram(
					oglCrtRenderMethod,
					methodConstants[i].getName().c_str(),
					oglGraphics->textureMatrix);
				break;
			}
		}
	}
}
