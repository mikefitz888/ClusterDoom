#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include "RenderManager.h"
#include "gameobject.h"

namespace gamecontroller {
	using graphics::IRenderable;
	using gameobject::GameObject;
	class GameController : public IRenderable {
	public:
		inline GameController(){}
		void init() final override {}
		void render() final override {}
		void renderGUI() final override {}
		void release() final override {}
		inline GameObject* createObject(int key) {return new GameObject(key, NULL);}
	};
}

#endif //GAMECONTROLLER_H