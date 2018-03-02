#include "RedPandaStudio.h"

namespace rps{

//////////////////////////////
//  DEVICE CONSTRUCTORS
RedPandaStudio& RedPandaStudio::createDevice(int width, int height, int depth, int framerate, bool vsync, bool fullscreen){

    static RedPandaStudio rps;

    rps.initSDLWindow(width, height, depth, framerate, vsync, fullscreen);
    rps.initOpenGL();
    rps.initScene();

    return rps;

}

void RedPandaStudio::updateDevice() {

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	renderCamera();
	renderLights();

	scene->draw();

	SDL_GL_SwapWindow(window);

}

void RedPandaStudio::dropDevice() {

	// Delete our OpengL context
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

    //delete scene;
    delete resourceManager;

}

void RedPandaStudio::initSDLWindow(int width, int height, int depth, int framerate, bool vsync, bool fullscreen) {

    // Initialize SDL 
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	  std::cerr << "Couldn't init SDL" << std::endl;
	SDL_GL_LoadLibrary(NULL);

	// Request an OpenGL 4.5 context (should be core)
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	// Also request a depth buffer
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, depth);

	// Create the window
	if (fullscreen) {
	  window = SDL_CreateWindow(
	    "XKating", 
	    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
	    0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_OPENGL
	  );
	} else {
	  window = SDL_CreateWindow(
	    "XKating", 
	    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
	    width, height, SDL_WINDOW_OPENGL
	  );
	}

	if(vsync){
		SDL_GL_SetSwapInterval(1);
	}

	context = SDL_GL_CreateContext(window);

    //Give window to RedPandaStudio
    setWindow(window);

    //Output message
    std::cout << "SDL Window opened..." << std::endl;

	//Initilize Scene and ResourceManager here
	scene = new TNode();
    resourceManager = new ResourceManager();

}

void RedPandaStudio::initOpenGL() {

    const char * vertex_file_path = "test.vert";
    const char * fragment_file_path = "test.frag";

	glewExperimental = GL_TRUE;

	std::cout << "GLEW: " << glewGetErrorString(glewInit()) << std::endl;

    //Init VBO
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);  

    //Create shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	//Read vertex shader from file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open()){
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}else{
		printf("Couldn't open %s\n", vertex_file_path);
	}

	//Read fragment shader from file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}else{
		printf("Couldn't open %s\n", vertex_file_path);
	}

    //Init some variables
	GLint Result = GL_FALSE;
	int InfoLogLength;

	//Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	//Check vertex shader is ok
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	//Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	//Check fragment shader is ok
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	//Link OpenGL program using the id
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	//Check the program is ok
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

    //We no longer need the shaders (we have them in the program)
	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);
	
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

    //Use the program we have just created
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(ProgramID);

    //Give the ProgramID to our engine
    scene->getEntity()->setProgramID(ProgramID);

	glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

	GLuint model = glGetUniformLocation(ProgramID, "ModelMatrix");   
    GLuint view  = glGetUniformLocation(ProgramID, "ViewMatrix");
    GLuint projection = glGetUniformLocation(ProgramID, "ProjectionMatrix");
	scene->getEntity()->setModelID(model);
	scene->getEntity()->setViewID(view);
	scene->getEntity()->setProjectionID(projection);

}

void RedPandaStudio::initScene() {

    //Initialize Projection Matrix
    glm::mat4& Projection = scene->getEntity()->projectionMatrix();
    Projection = glm::mat4(1.0f);

    //Initilize View Matrix
    glm::mat4& View = scene->getEntity()->viewMatrix();
    View = glm::mat4(1.0f);

    //Initilize Model Matrix
    glm::mat4& Model = scene->getEntity()->modelMatrix();
    Model = glm::mat4(1.0f);
}

//////////////////////////////
//  NODE CONSTRUCTORS
TNode* RedPandaStudio::createObjectNode(TNode* parent, glm::vec3 pos, const char* mesh, const char* text) {

	//Check parent node is valid
	if(parent != nullptr && (parent->getEntity() == nullptr || dynamic_cast<TTransform*>(parent->getEntity()) != nullptr)){

		//Create new transformation tree
		TNode* transformT = addRotScaPos(parent, pos);

		//Create new mesh entity
		TMesh* m = new TMesh();
		m->setMesh(resourceManager->getResourceMesh(mesh));
		m->setTexture(resourceManager->getResourceTexture(text));
		m->setTextActive(true);
		TNode* mesh = new TNode(transformT, m);
		transformT->addChild(mesh);

		//Return mesh
		return mesh;
	}
	else{
		return nullptr;
	}
}

TNode* RedPandaStudio::createCamera(TNode* parent, glm::vec3 position) {

	//Check parent node is valid
	if(parent != nullptr && (parent->getEntity() == nullptr || dynamic_cast<TTransform*>(parent->getEntity()) != nullptr)){

		//Create new transformation tree
		TNode* transformT = addRotScaPos(parent, position);

		//Create new camera entity
		TCamera* c = new TCamera(45.0f);
		TNode* cam = new TNode(transformT, c);
		transformT->addChild(cam);

		//Register camera
		camera = cam;

		//Return camera
		return cam;
	}
	else{
		return nullptr;
	}

}

TNode* RedPandaStudio::createLight(TNode* parent, glm::vec3 position, glm::vec3 intensity) {

	//Check parent node is valid
	if(parent != nullptr && (parent->getEntity() == nullptr || dynamic_cast<TTransform*>(parent->getEntity()) != nullptr)){

		//Create new transformation tree
		TNode* transformT = addRotScaPos(parent, position);

		//Create new light entity
		TLight* l = new TLight(intensity);
		TNode* light = new TNode(transformT, l);
		transformT->addChild(light);

		//Register light
		lights.push_back(light);

		//Return light
		return light;
	}
	else{
		return nullptr;
	}

}

void RedPandaStudio::deleteObject(TNode* leaf) {

}

TNode* RedPandaStudio::addRotScaPos(TNode* parent, glm::vec3 pos) {

		//Rotation transformation
		TTransform* tR = new TTransform();
		tR->identity();
		TNode* transformR = new TNode(parent, tR);
		parent->addChild(transformR);

		//Scale transformation
		TTransform* tS = new TTransform();
		tS->identity();
		TNode* transformS = new TNode(transformR, tS);
		transformR->addChild(transformS);

		//Translation transformation
		TTransform* tT = new TTransform();
		tT->identity();
		tT->translate(pos.x, pos.y, pos.z);
		TNode* transformT = new TNode(transformS, tT);
		transformS->addChild(transformT);

		return transformT;

}

////////////////////////////////////
//  LIGHTS AND CAMERA MANAGEMENT
void RedPandaStudio::renderLights() {

	for(unsigned int i = 0; i < lights.size(); i++){

		glm::mat4 mat = glm::mat4(1.0);
		calculateNodeTransform(lights[i], mat);

		glm::vec4 pos = mat * glm::vec4(0.0, 0.0, 0.0, 1.0);

		std::string lightName = std::string("light[" + std::to_string(i) + "].position");
		GLuint posID = glGetUniformLocation(scene->getEntity()->getProgramID(), lightName.c_str());
		glUniform4fv(posID, 1, &pos[0]);

		std::string lightName2 = std::string("light[" + std::to_string(i) + "].intensity");
		GLuint intID = glGetUniformLocation(scene->getEntity()->getProgramID(), lightName2.c_str());
		TLight* l = (TLight*)lights[i]->getEntity();
		glUniform4fv(intID, 1, &l->getIntensity()[0]);
	}

	GLuint numL = glGetUniformLocation(scene->getEntity()->getProgramID(), "numLights");
	glUniform1i(numL, lights.size());

}
void RedPandaStudio::renderCamera() {

	if(camera != nullptr){
		glm::mat4 mat = glm::mat4(1.0);
		calculateNodeTransform(camera, mat);

		glm::mat4& view = scene->getEntity()->viewMatrix();
    	view = glm::inverse(mat);

		std::cout << view[0][0] << " " << view[0][1] << " " << view[0][2] << " " << view[0][3] << " " << std::endl;
    	std::cout << view[1][0] << " " << view[1][1] << " " << view[1][2] << " " << view[1][3] << " " << std::endl;
    	std::cout << view[2][0] << " " << view[2][1] << " " << view[2][2] << " " << view[2][3] << " " << std::endl;
    	std::cout << view[3][0] << " " << view[3][1] << " " << view[3][2] << " " << view[3][3] << " " << std::endl;
		std::cout << "_________________" << std::endl;
    	std::cout << std::endl;

		glUniformMatrix4fv(scene->getEntity()->getViewID(), 1, GL_FALSE, &scene->getEntity()->viewMatrix()[0][0]);
    	glUniformMatrix4fv(scene->getEntity()->getProjectionID(), 1, GL_FALSE, &scene->getEntity()->projectionMatrix()[0][0]);
	}
}

//Recursive function. Should receive an identity as input. Returns the accumulated transformation
void RedPandaStudio::calculateNodeTransform(TNode* node, glm::mat4& mat) {

	if(node!= nullptr && node->getFather() != nullptr) {

		TTransform* t = dynamic_cast<TTransform*>(node->getEntity());
		if( t != nullptr) 
			mat *= t->getMatrix(); 

		calculateNodeTransform(node->getFather(), mat);
	}
}

//////////////////////////////
//  TRANSFORMATION FACADE
void translateNode(TNode* node, glm::vec3 position) {

	TEntity* t;

	//Check the input is a mesh, camera or light
	if(node != nullptr && ((t = dynamic_cast<TMesh*>(node->getEntity())) != nullptr ||
		(t = dynamic_cast<TCamera*>(node->getEntity())) != nullptr ||
		(t = dynamic_cast<TLight*>(node->getEntity())) != nullptr)) {

		TTransform* tr = (TTransform*)node->getFather()->getEntity();

		tr->translate(position.x, position.y, position.z);

	}

}

void scaleNode(TNode* node, glm::vec3 scale) {

	TEntity* t;

	//Check the input is a mesh, camera or light
	if(node != nullptr && ((t = dynamic_cast<TMesh*>(node->getEntity())) != nullptr ||
		(t = dynamic_cast<TCamera*>(node->getEntity())) != nullptr ||
		(t = dynamic_cast<TLight*>(node->getEntity())) != nullptr)) {

		TTransform* tr = (TTransform*)node->getFather()->getFather()->getEntity();

		tr->scale(scale.x, scale.y, scale.z);

	}

}

void rotateNode(TNode* node, float rotation, int axis) {

	TEntity* t;

	//Check the input is a mesh, camera or light
	if(node != nullptr && ((t = dynamic_cast<TMesh*>(node->getEntity())) != nullptr ||
		(t = dynamic_cast<TCamera*>(node->getEntity())) != nullptr ||
		(t = dynamic_cast<TLight*>(node->getEntity())) != nullptr)) {

		TTransform* tr = (TTransform*)node->getFather()->getFather()->getFather()->getEntity();

		if(axis == 0)
			tr->rotate(1, 0, 0, rotation);
		if(axis == 1)
			tr->rotate(0, 1, 0, rotation);
		if(axis == 2)
			tr->rotate(0, 0, 1, rotation);
	}

}

}