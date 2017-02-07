#ifndef _CORE_HEADER
#define _CORE_HEADER

// Standard library headers
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <vector>
#include <deque>
#include <stack>
#include <unordered_map>
#include <cmath>
#include <algorithm>
#include <map>
#include <string>
#include <memory>
#include <exception>
#include <functional>

// Shared GLEW
#include <GL/glew.h>

// Shared SFML
#include <SFML/Graphics.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/Packet.hpp>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Main.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <SFML/System/Time.hpp>

// Shared OpenCV
#include <opencv/cv.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>

// Shared GLM
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Forward declarations
class Spawn;
class Buffer;
class AudioManager;
class FrameBuffer;
class ObjectLogic;
class ResourceManager;
class String;


namespace smartpointers {
	class InvalidatedSmartPointerException;
	template <typename T> class watch_ptr;
	template <typename T> class slave_ptr;
	template <typename T> class master_ptr;
}

namespace network {
	class NetworkManager;
	class NetworkClient;
	class INetworkInstance;
}

namespace tower {
	class Base;
	class BasicTower;
	class Tower;
	enum TYPE : unsigned int;
}

namespace towerlogic {
	class TowerLogic;
	typedef smartpointers::slave_ptr<tower::Tower> tower_ptr;
}

namespace unit {
	class Unit;
	class BasicUnit;
	enum TYPE : unsigned int;
}

namespace unitlogic {
	class UnitLogic;
	typedef smartpointers::slave_ptr<unit::Unit> unit_ptr;
}

namespace graphics {
	class AnimatedTexture;
	class RenderManager;
	class VertexBuffer;
	class GCamera;
	struct Colour;
	class IRenderable;
	class Texture;
	class RenderUtils;
	struct Vec2;
	struct Vec3;
	struct Vec4;
	class Vertex;
	class VertexBuffer;
}

namespace manager {
	class Manager;
}

namespace controller {
	class Controller;
}

namespace cvinterface {
	class CVInterface;
}

namespace gameobject {
	class GameObject;
	struct BoundingBox;
	enum CollisionType : unsigned int;
	class Collision;
	enum TYPE : unsigned int;
	enum OBJECT_TYPE : unsigned int;
	typedef size_t id_t;
	template <typename T> struct Point;
}

namespace gamecontroller {
	struct Matching;
	class GameController;
}

namespace containers {
	class InvalidOptionalAccessException;
	struct none_t;
	template <typename T> class optional;
}

namespace worldrenderer {
	class WorldRenderer;
}

#endif //_CORE_HEADER
