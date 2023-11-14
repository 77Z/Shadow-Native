#ifndef SHADOW_NATIVE_SCENE_ENTITY_INSPECTOR
#define SHADOW_NATIVE_SCENE_ENTITY_INSPECTOR

#include "Scene/Entity.hpp"

namespace Shadow {

	class EntityInspector {
	public:
		EntityInspector();
		~EntityInspector();

		void onUpdate();
		void unload();

		// Primary method utilized to show the data and components of an entity.
		void pickEntity(Entity& entity) { this->entity = &entity; }

		void deselectEntity() { entity = nullptr; }

	private:
		Entity* entity = nullptr;
	};

}

#endif /* SHADOW_NATIVE_SCENE_ENTITY_INSPECTOR */