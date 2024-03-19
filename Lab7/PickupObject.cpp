#include "pch.h"
#include "PickupObject.h"

PickupObject::PickupObject(SceneManager* scene_manager, const char* mesh_file_name,
	Vector3 position, Vector3 scale)
{
	scene_manager_ = scene_manager;
	entity_ = scene_manager_->createEntity(mesh_file_name);
	entity_node_ = scene_manager_->getRootSceneNode()->createChildSceneNode();
	entity_node_->attachObject(entity_);
	entity_node_->setPosition(position);
	entity_node_->setScale(scale);
	entity_->setCastShadows(true);
	//pickup_effects_ = nullptr;
	picked_up_ = false;
}

PickupObject::~PickupObject()
{
	for (auto i = pickup_effects_.begin(); i != pickup_effects_.end();)
	{
		delete(*i);
	}

	
}

SceneNode* PickupObject::getSceneNode() const
{
	return  entity_node_;
}

std::list<IPickupEffect*> PickupObject::getPickupEffects() const
{
	return pickup_effects_;
}

Entity* PickupObject::getEntity() const
{
	return entity_;
}

bool PickupObject::isPickedUp() {
	return picked_up_;
}

bool PickupObject::collidesWith(SceneNode* other_node, float distance) {
	bool collision = false;

	Vector2f pickupNodePos = Vector2f(entity_node_->getPosition().x, entity_node_->getPosition().z);
	Vector2f otherNodePos = Vector2f(other_node->getPosition().x, other_node->getPosition().z);

	float distBetween = pickupNodePos.distance(otherNodePos);

	// TODO: Assign true to "collision" if the pickup object collides with the "other_node"
	if (pickupNodePos.distance(otherNodePos) <= distance)
	{
		collision = true;
	}
		return collision;
}

void PickupObject::runPickupEffects()
{
	picked_up_ = true;

	IPickupEffect* particleEffect = new ParticleEffect(scene_manager_, entity_node_);
	pickup_effects_.push_back(particleEffect);
	particleEffect->run();

	IPickupEffect* swirlEffect = new SwirlEffect(entity_node_, Ogre::Vector3(5.0, -5, 50.0));
	pickup_effects_.push_back(swirlEffect);
	swirlEffect->run();
}

void PickupObject::update(float delta_time) const
{
	for (auto i = pickup_effects_.begin(); i != pickup_effects_.end();)
	{
		IPickupEffect* pickupEffect = *i;
		if (pickupEffect->isRunning())
			pickupEffect->update(delta_time);

		++i;
	}
}

