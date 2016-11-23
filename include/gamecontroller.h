#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include "RenderManager.h"
#include "gameobject.h"

namespace gamecontroller {
	using graphics::IRenderable;
	using gameobject::GameObject;
	class GameController : public IRenderable {
	public:
		GameController();
		void init();
		void render();
		void renderGUI();
		void release();
		GameObject* createObject(int key);
	};
}

#endif //GAMECONTROLLER_H