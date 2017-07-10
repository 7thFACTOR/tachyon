#include "physics/physics_material.h"
#include "core/globals.h"
#include "core/resource_repository.h"

namespace engine
{
//TODO: make it a proper resource for new POD paradigm
PhysicsMaterial::PhysicsMaterial()
{
	dynamicFriction = 0.5f;
	staticFriction = 0.5f;
	restitution = 0.5f;
	disableFriction = false;
	solid = true;
}

PhysicsMaterial::~PhysicsMaterial()
{}

bool PhysicsMaterial::load(const String& filename)
{
	//XmlDocument xml;

	//if (!xml.load(pFilename))
	//{
	//	return false;
	//}

	//XmlElement* pPhysxMtlElem = xml.getRoot().getChild("physicsMaterial");
	//XmlElement* propsElem = pPhysxMtlElem->getChild("properties");

	//m_name = pPhysxMtlElem->getAttributeValue("name");

	//if (propsElem->isValid())
	//{
	//	loadPropertiesFromXmlElements(propsElem);
	//	return true;
	//}

	return false;
}

bool PhysicsMaterial::unload()
{
	return true;
}

void PhysicsMaterial::update()
{}

}