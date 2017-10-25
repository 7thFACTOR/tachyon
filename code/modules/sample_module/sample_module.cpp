#ifdef _WINDOWS
#include <tchar.h>
#endif
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include "base/cmdline_arguments.h"
#include "core/application.h"
#include "base/process.h"
#include "base/math/util.h"
#include "core/globals.h"
#include "core/module.h"
#include "base/variable.h"
#include "base/string.h"
#include "base/logger.h"
#include "core/resource_repository.h"
#include "logic/logic.h"
#include "graphics/graphics.h"
#include "input/window.h"
#include "graphics/graphics.h"
#include "base/stdio_logger.h"
#include "core/resource.h"
#include "base/math/conversion.h"
#include "graphics/shape_renderer.h"
//#include "base/profiler.h"
//#include "../editor/editor_core/component_editor_proxy.h"

#include "logic/component.h"
#include "logic/entity.h"
#include "logic/logic.h"
#include "logic/component_pool.h"
#include "logic/component_updater.h"
#include "logic/components/render/transform.h"
#include "logic/components/render/camera.h"
#include "logic/components/render/mesh.h"
#include "logic/components/render/mesh_renderer.h"
#include "logic/components/input/flyby.h"
#include "logic/component_updaters/render/render_component_updater.h"

#ifdef _WINDOWS
	#include <windows.h>
#endif

#ifdef _LINUX
	#include <unistd.h>
#endif

using namespace base;
using namespace engine;

struct TestNewComponent : Component
{
	static const ComponentTypeId typeId = 0x416ebd5115f15713;

	f32 speed = 0;
};

class TestComponentUpdater : public ComponentUpdater
{
public:
	ComponentUpdaterId updaterId = (ComponentUpdaterId)hashString("MyCompany:TestComponentUpdater");

	ComponentUpdaterId getId() const override { return updaterId; }

	void update(f32 deltaTime) override
	{
		auto pool = getLogic().getComponentPool<TestNewComponent>();
	}

	void debugRender(f32 deltaTime, ShapeRenderer* shapeRenderer)
	{
		Matrix xform;

		xform.setTranslation(0, 0, 0);
		getGraphics().setWorldMatrix(xform);
		shapeRenderer->begin();
		shapeRenderer->setColor(Color::red);
		shapeRenderer->drawSimpleAxis(111);
		shapeRenderer->setColor(Color::magenta);
		shapeRenderer->drawCross(50);
		shapeRenderer->drawGrid(1500, 10, 100, Color::gray, Color::lightGray, Color::red);
		xform.setTranslation(100, 0, 0);
		getGraphics().setWorldMatrix(xform);
		shapeRenderer->setColor(Color::white);
		shapeRenderer->texturingEnabled = true;
		shapeRenderer->setTexture(loadResource("textures/missing_texture.png"));
		shapeRenderer->drawSolidEllipsoid(34, 34, 34);
		//shapeRenderer->setTexturing(false);
		xform.setTranslation(0, 300, 0);
		getGraphics().setWorldMatrix(xform);
		shapeRenderer->setColor(Color::red);
		//shapeRenderer->drawWireEllipsoid(42, 42, 62, 6, 6);
		shapeRenderer->setColor({ 1,1,1,0.5f });
		shapeRenderer->drawSimpleWireEllipsoid(111, 111, 111, 110);

		Matrix looky = xform;

		//auto trans = camera->entity->getComponent<TransformComponent>()->translation;

		//looky.lookAt(Vec3(0, 300, 0), Vec3(trans.x, trans.y, trans.z), Vec3(0, 1, 0));
		shapeRenderer->setColor({ 1,1,1,1 });
		//getGraphics().setWorldMatrix(looky);

		shapeRenderer->drawSimpleCircle(111, 111, 110);
		//getGraphics().setWorldMatrix(xform);

		shapeRenderer->setColor(Color::red);
		shapeRenderer->drawSolidBox(111, 111, 111);

		//xform.setTranslation(-100, 100, 0);
		//getGraphics().setWorldMatrix(xform);
		shapeRenderer->setColor(Color::white);
		shapeRenderer->drawSolidBox(60, 40, 33);
		shapeRenderer->end();
	}

	void onComponentAdded(World& world, Entity& entity, Component* component) override {}
	void onComponentRemoved(World& world, Entity& entity, Component* component) override
	{
	}
};

/*
struct MyModuleComponentEditorProxy : ComponentEditorProxy
{
	ComponentTypeId getComponentTypeId() const override { return TestNewComponent::typeId; }
	void onComponentGizmoRender(ShapeRenderer* shapeRenderer, Component* component, bool selected)
	{

	}

	void onComponentGizmoMouseEvent(ShapeRenderer* shapeRenderer, Component* component, InputEvent& mouseEvent)
	{

	}

} myModuleComponentEditorProxy;
*/

enum class MyFlags
{
	None,
	One = B_BIT(0),
	Two = B_BIT(1),
	Three = B_BIT(2)
};
B_ENUM_AS_FLAGS(MyFlags)

class SampleModule : public Module, public RenderObserver
{
public:
	Entity* entCamera;
	Entity* entMesh;
	Entity* entMesh2;
	Entity* entMesh3;
	Entity* entMesh4;
	Array<Entity*> meshes;
	ShapeRenderer shpRenderer;
	ComponentPoolTpl<TestNewComponent> testNewComponentPool;
	TestComponentUpdater testComponentUpdater;

	SampleModule()
	{
		FileOpenFlags fo = FileOpenFlags::None;

		fo |= FileOpenFlags::Append;
		fo |= FileOpenFlags::Write;
		fo |= FileOpenFlags::Read;


		MyFlags f = MyFlags::None;
		u32 fff = fromFlags(MyFlags::Three);

		f = (MyFlags::One | MyFlags::Three);
		f |= MyFlags::Two;
		f |= MyFlags::Three;
		//f &= ~MyFlags::Three;

		if (has(f, MyFlags::Three))
		{
			B_LOG_DEBUG("OKO");
		}

		// logic is not yet created here, just initialize raw data if any
	}

	~SampleModule()
	{}

	void onAfterWorldRender() override
	{
		shpRenderer.begin();
		shpRenderer.setTransform({ 0, 0, 0 }, Quat(), Vec3(1, 1, 1));
		shpRenderer.drawGrid(10000, 40, 500, Color::black, Color::gray, Color::red);

		shpRenderer.texturingEnabled = true;
		shpRenderer.textureScale.set(5, 5);
		shpRenderer.setTexture(loadResource("textures/missing_material.png"));
		shpRenderer.setTransform({ -20, 0, 0 }, Quat(), Vec3(1, 1, 1));
		shpRenderer.drawSolidEllipsoid(50, 50, 50, 30, 30);
		shpRenderer.setTexture(loadResource("textures/missing_material.png"));
		shpRenderer.setTransform({ 200, 0, 0 }, Quat(), Vec3(1, 1, 1));
		shpRenderer.textureScale.set(1, 1);
		shpRenderer.drawSolidBox(50, 50, 50);

		shpRenderer.texturingEnabled = false;
		shpRenderer.setTransform({ 0, 1, 0 }, Quat(), Vec3(1, 1, 1));
		shpRenderer.drawSimpleAxis(200);

		shpRenderer.end();
	}

	void onLoad() override
	{
		B_LOG_INFO("Setup my module");
		auto& logic = getLogic();
		auto& mainWorld = logic.getWorld();

		logic.addComponentPool(&testNewComponentPool);
		logic.addComponentUpdater(&testComponentUpdater, ComponentUpdater::UpdatePriority::Later);

		RenderComponentUpdater* rcu = (RenderComponentUpdater*)getLogic().getComponentUpdater(StdComponentUpdaterId_Render);
		rcu->addObserver(this);

		entCamera = mainWorld.createEntity();
		entMesh = mainWorld.createEntity();
		entMesh2 = mainWorld.createEntity();
		entMesh3 = mainWorld.createEntity();
		entMesh4 = mainWorld.createEntity();

		entCamera->name = "MainCamera";
		auto camXform = entCamera->addComponent<TransformComponent>();
		auto cam = entCamera->addComponent<CameraComponent>();

		setRandomSeed(2);

		if (1)
			for (int i = 0; i < 5; i++)
			{
				for (int j = 0; j < 5; j++)
				{
					meshes.append(mainWorld.createEntity());
					auto meshEntity = meshes.back();

					auto meshXform = meshEntity->addComponent<TransformComponent>();
					auto mesh = meshEntity->addComponent<MeshComponent>();
					auto meshRend = meshEntity->addComponent<MeshRendererComponent>();
					auto testComp = meshEntity->addComponent<TestNewComponent>();

					testComp->speed = randomFloat(10, 40);

					float s = randomFloat(3, 100);
					float a = randomFloat(-200, 245);
					float aa = randomFloat(-1, 1);

					meshXform->scale.set(111, 111, 111);
					meshXform->translation = {
						randomFloat(-5000.0f, 5000.0f),
						randomFloat(-0.0f, 0.0f),
						randomFloat(-5000.0f, 5000.0f) };

					mesh->meshResourceId = loadResource("meshes/primitives_cube.mesh");
					meshRend->materials.append(loadResource("meshes/animated_box_01 - Default.material"));
				}
			}

		auto fly = entCamera->addComponent<FlybyComponent>();
		B_LOG_DEBUG("Cam xfrm " << (u64)camXform);

		fly->speed = 120;
		cam->autoAspectRatio = true;
		cam->aspectRatio = 0.56f;
		cam->fov = 70;
		cam->nearPlane = 3.0f;
		cam->farPlane = 100000;
		cam->dirty = true;
		camXform->translation.set(0, 120, 0);
		//toQuat(45, 0, 0, 1, camXform->rotation);
		camXform->dirty = true;

		auto meshXform = entMesh->addComponent<TransformComponent>();
		auto mesh = entMesh->addComponent<MeshComponent>();
		auto meshRend = entMesh->addComponent<MeshRendererComponent>();

		meshXform->scale.set(1, 1, 1);
		meshXform->translation = {0, 0, 210};

		//mesh->meshResourceId = loadResource("ed/ed209.fbx");
		//meshRend->materials.append(loadResource("ed/ed209.material"));
		mesh->meshResourceId = loadResource("meshes/sphere.fbx");
		//getResources().waitLoadAll();
		ScopedResourceMapping<MeshResource> meshRes(mesh->meshResourceId);

		if (meshRes)
		{
			B_LOG_DEBUG("Elements " << meshRes->elements.size());
		}

		meshRend->materials.append(loadResource("materials/default01.material"));
		meshRend->materials.append(loadResource("materials/default02.material"));
		meshRend->materials.append(loadResource("materials/default03.material"));
		meshRend->materials.append(loadResource("materials/default04.material"));
		meshRend->materials.append(loadResource("materials/default05.material"));
	}

	void onUnload() override
	{
		getLogic().removeComponentPool(&testNewComponentPool);
		getLogic().removeComponentUpdater(&testComponentUpdater);
	}

	void onUpdate(f32 deltaTime) override
	{
		if (getLogic().inputMap.isAction("exit"))
		{
			getApplication().stopped = true;
		}

		if (getLogic().inputMap.isActionCombo("ultrăîțFire话"))
		{
			B_LOG_INFO("ULTRAFIRE!" << getTimeMilliseconds());
		}
		// do not fire yet if a combo was initiated
		else if (getLogic().inputMap.isAction("fire") && !getLogic().inputMap.isAnyActionComboInitiated())
		{
			B_LOG_INFO("FIRE!" << getTimeMilliseconds());
		}

		Quat q;
		static float u = 0;
		toQuat(u, 0, 1, 0, q);
		u += deltaTime * 3;

		//entMesh->getComponent<TransformComponent>()->scale.y = 10 + 45 * fabs(cosf(u));
		//entMesh->getComponent<TransformComponent>()->scale.x = 10 + 45 * (1.0f - cosf(u));
		//entMesh->getComponent<TransformComponent>()->scale.z = 10 + 45 * (1.0f - cosf(u));
		//entMesh->getComponent<TransformComponent>()->rotation = q;
		//entMesh->getComponent<TransformComponent>()->rotation.normalize();
		//entMesh->getComponent<TransformComponent>()->dirty = true;

		for (size_t i = 0; i < meshes.size(); i++)
		{
			auto testComp = meshes[i]->getComponent<TestNewComponent>();
			meshes[i]->getComponent<TransformComponent>()->translation.z -= deltaTime * testComp->speed;
		}
	}
};

E_DECLARE_MODULE(SampleModule, "My Sample Module", "(C) 2017 7thFACTOR", Version(1, 0, 0));