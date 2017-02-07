#ifndef _CORE_HEADER
#define _CORE_HEADER

// Standard library headers
#include <algorithm>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <deque>
#include <exception>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <queue>
#include <stack>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

// Shared GLEW
#include <GL/glew.h>

// Shared SFML
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Main.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/Packet.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/System.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window.hpp>
#include <SFML/Window/Keyboard.hpp>

// Shared OpenCV
#include <opencv/cv.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>

// Shared GLM
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

// Forward declarations
class AudioManager;
class Buffer;
class FrameBuffer;
class ObjectLogic;
class ResourceManager;
class Spawn;
class String;

typedef unsigned char byte;

namespace containers {
    class InvalidOptionalAccessException;
    template <typename T> class optional;
}

namespace smartpointers {
    class InvalidatedSmartPointerException;
    template <typename T> class watch_ptr;
    template <typename T> class slave_ptr;
    template <typename T> class master_ptr;
}

namespace controller {
    class Controller;
}

namespace cvinterface {
    class CVInterface;
}

namespace gamecontroller {
    class GameController;

    struct Matching;
}

namespace graphics {
    class AnimatedTexture;
    class GCamera;
    class IRenderable;
    class RenderManager;
    class RenderUtils;
    class Texture;
    class Vertex;
    class VertexBuffer;

    struct Colour;
    struct Vec2;
    struct Vec3;
    struct Vec4;
}

namespace manager {
    class Manager;

    typedef size_t id_t;
}

namespace network {
    class INetworkInstance;
    class NetworkClient;
    class NetworkManager;
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
    class BasicUnit;
    class Unit;

    enum TYPE : unsigned int;
}

namespace gameobject {
	class Collision;
	class GameObject;

	struct BoundingBox;
	template <typename T> struct Point;
  
	enum CollisionType : unsigned int;
	enum OBJECT_TYPE : unsigned int;
	enum TYPE : unsigned int;

	typedef size_t id_t;
	typedef smartpointers::slave_ptr<tower::Tower> tower_ptr;
	typedef smartpointers::slave_ptr<unit::Unit> unit_ptr;
	typedef smartpointers::slave_ptr<GameObject> gameobject_ptr;
}

namespace unitlogic {
    class UnitLogic;

    typedef smartpointers::slave_ptr<unit::Unit> unit_ptr;
}

namespace worldrenderer {
    class WorldRenderer;
}

#endif //_CORE_HEADER
