using ObjectPointer = std::shared_ptr<GameObject>;

class GameObject {
public:
	int test;
}

int main() {
	std::vector<ObjectPointer> object_pool; // An alias for shared_ptr<ClassName> can be used for prettiness
	
	object_pool.push_back( ObjectPointer(new GameObject()) );

	anotherMethod(object_pool[0]);
	
}

void anotherMethod(std::weak_ptr<GameObject> some_object){
	//Thread safe usage
	if(auto obj_ptr = some_object.lock()){
		//Do some stuff..

	} else { //Optional error clause
		//Throw some errors...

	} // The created shared pointer is dereferenced here, no leaks!
}

std::weak_ptr<abstract::channel> weak_ch = ch;