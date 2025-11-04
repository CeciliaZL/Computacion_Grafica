/*
Práctica 7: Iluminación 1
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture dadoTexture;
Texture logofiTexture;
Texture LlantaIzqAtras;
Texture LlantaDerAtras;
Texture LlantaIzqAd;
Texture LlantaDerAd;
Model Blackhawk_M;
Model Lampara;

//Carro
Model Kitt_M;
Model Llanta_M;
Model Dado_M;
Model LlantaIzqAtras1;
Model LlantaDerAtras1;
Model LlantaIzqAd1;
Model LlantaDerAd1;
Model CarAcomodado;
Model CarroSeparado;
Model hotdog;

Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];
SpotLight spotLights2[2];
#define MAX_POINT_LIGHTS 2


// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//función de calculo de normales por promedio de vértices 
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

void CrearDado() {
	GLfloat octaedro_vertices[] = {
		// Cara 1 (Triángulo superior frontal: Y+, Z+, X+)
		0.0f,  1.0f,  0.0f,     0.82f, 0.64f,     -1.0f, -1.0f, -1.0f,
		0.0f,  0.0f,  1.0f,     0.66f, 0.36f,     -1.0f, -1.0f, -1.0f,
		1.0f,  0.0f,  0.0f,     0.99f, 0.36f,     -1.0f, -1.0f, -1.0f,

		// Cara 2 (Triángulo superior derecho: Y+, X+, Z-)
		0.0f,  1.0f,  0.0f,     0.16, 0.28f,     -1.0f, -1.0f, 1.0f,
		1.0f,  0.0f,  0.0f,     0.0f, 0.0f,      -1.0f, -1.0f, 1.0f,
		0.0f,  0.0f, -1.0f,     0.33f, 0.0f,     -1.0f, -1.0f, 1.0f,

		// Cara 3 (Triángulo superior trasero: Y+, Z-, X-)
		0.0f,  1.0f,  0.0f,     0.49f, 0.28f,   1.0f, -1.0f, 1.0f,
		0.0f,  0.0f, -1.0f,     0.33f, 0.0f,    1.0f, -1.0f, 1.0f,
		-1.0f,  0.0f,  0.0f,    0.66f, 0.0f,    1.0f, -1.0f, 1.0f,

		// Cara 4 (Triángulo superior izquierdo: Y+, X-, Z+)
		0.0f,  1.0f,  0.0f,     0.82f, 0.28f,   1.0f, -1.0f, -1.0f,
		-1.0f,  0.0f,  0.0f,    0.66f, 0.0f,    1.0f, -1.0f, -1.0f,
		0.0f,  0.0f,  1.0f,     0.99f, 0.0f,    1.0f, -1.0f, -1.0f,

		// Cara 5 (Triángulo inferior frontal: Y-, X+, Z+)
		0.0f, -1.0f,  0.0f,     0.16f, 0.71f,   -1.0f, 1.0f, -1.0f,
		1.0f,  0.0f,  0.0f,     0.33f, 1.0f,    -1.0f, 1.0f, -1.0f,
		0.0f,  0.0f,  1.0f,     0.0f, 1.0f,     -1.0f, 1.0f, -1.0f,

		// Cara 6 (Triángulo inferior derecho: Y-, Z+, X-)
		0.0f, -1.0f,  0.0f,     0.49f, 0.71f,   1.0f, 1.0f, -1.0f,
		0.0f,  0.0f,  1.0f,     0.66f, 1.0f,    1.0f, 1.0f, -1.0f,
		-1.0f,  0.0f,  0.0f,    0.33f, 1.0f,    1.0f, 1.0f, -1.0f,

		// Cara 7 (Triángulo inferior trasero: Y-, X-, Z-)
		0.0f, -1.0f,  0.0f,     0.82f, 0.71f,   1.0f, 1.0f, 1.0f,
		-1.0f,  0.0f,  0.0f,    0.99f, 1.0f,    1.0f, 1.0f, 1.0f,
		0.0f,  0.0f, -1.0f,     0.66f,  1.0f,   1.0f, 1.0f, 1.0f,

		// Cara 8 (Triángulo inferior izquierdo: Y-, Z-, X+)
		0.0f, -1.0f,  0.0f,     0.16f, 0.36f,    -1.0f, 1.0f, 1.0f,
		0.0f,  0.0f, -1.0f,     0.33f, 0.64f,    -1.0f, 1.0f, 1.0f,
		1.0f,  0.0f,  0.0f,     0.0f,  0.64f,    -1.0f, 1.0f, 1.0f
	};

	unsigned int octaedro_indices[] = {
		// Cara 1
		0, 1, 2,
		// Cara 2
		3, 4, 5,
		// Cara 3
		6, 7, 8,
		// Cara 4
		9, 10, 11,
		// Cara 5
		12, 13, 14,
		// Cara 6
		15, 16, 17,
		// Cara 7
		18, 19, 20,
		// Cara 8
		21, 22, 23
	};

	Mesh* octaedro = new Mesh();
	octaedro->CreateMesh(octaedro_vertices, octaedro_indices, 192, 24);
	meshList.push_back(octaedro);
}

int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CrearDado();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 2.0f, 10.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	dadoTexture = Texture("Textures/dado-de-numeros.png");
	dadoTexture.LoadTextureA();
	logofiTexture = Texture("Textures/escudo_fi_color.tga");
	logofiTexture.LoadTextureA();
	LlantaIzqAtras = Texture("Textures/LlantaIzqAtras.png");
	LlantaIzqAtras.LoadTextureA();
	LlantaDerAtras = Texture("Textures/LlantaDerAtras.png");
	LlantaDerAtras.LoadTextureA();
	LlantaIzqAd = Texture("Textures/LlantaIzqAd.png");
	LlantaIzqAd.LoadTextureA();
	LlantaDerAd = Texture("Textures/LlantaDerAd.png");
	LlantaDerAd.LoadTextureA();
	

	
	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/llanta_optimizada.obj");
	LlantaIzqAtras1 = Model();
	LlantaIzqAtras1.LoadModel("Models/LlantaIzqAtras.obj");
	LlantaDerAtras1 = Model();
	LlantaDerAtras1.LoadModel("Models/LlantaDerAtras.obj");
	LlantaIzqAd1 = Model();
	LlantaIzqAd1.LoadModel("Models/LlantaIzqAd.obj");
	LlantaDerAd1 = Model();
	LlantaDerAd1.LoadModel("Models/LlantaDerAd.obj");
	CarAcomodado = Model();
	CarAcomodado.LoadModel("Models/CarAcomodado.obj");
	CarroSeparado = Model();
	CarroSeparado.LoadModel("Models/CarroSeparado2.obj");
	hotdog = Model();
	hotdog.LoadModel("Models/hotdog.obj");
	Lampara = Model();
	Lampara.LoadModel("Models/lampara.obj");

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;

	//Luz Lampara
	pointLights[0] = PointLight(1.0f, 1.0f, 1.0f,  // color blanco
		0.1f, 0.5f,        // intensidad ambiental baja, difusa alta
		0.0f, 8.0f, -2.6f, // posición más alta
		0.7f, 0.05f, 0.01f);
	pointLightCount++;

	unsigned int spotLightCount = 0;

	//Luz Hotdog
	pointLights[1] = PointLight(1.0f, 0.5f, 0.1f,  // Color cálido
		0.1f, 0.7f,                               // Intensidad ambiental y difusa
		0.0f, 2.0f, -14.0f,                       // Posición (ligada al modelo hotdog)
		0.7f, 0.2f, 0.1f);                        // atenuación (constant, linear, exponent)
	pointLightCount++;


	//LUCES CARRO
	//Luz Faro
	spotLights[0] = SpotLight(0.05f, 0.53f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		-1.0f, -0.2f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;


	//Luz Faro
	spotLights2[0] = SpotLight(0.05f, 0.53f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, -0.2f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	//Luz coche
	spotLights2[1] = SpotLight(1.0f, 1.0f, 0.0f, //Color
		0.1f, 2.0f,	//Intensidad
		0.0f, 0.0f, 0.0, //Coordenadas
		0.0f, -5.0f, 0.0f,	//Direccion
		0.99f, 0.5f, 0.2f,	//Valores con lin exp
		30.0f);
	spotLightCount++;

	//se crean mas luces puntuales y spotlight 
	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		mainWindow.getlampara();
		mainWindow.getfaro();
		mainWindow.getmuevex();

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		//sirve para que en tiempo de ejecución (dentro del while) se cambien propiedades de la luz
		//glm::vec3 lowerLight = camera.getCameraPosition();
		//lowerLight.y -= 0.3f;
		//spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);




		PointLight activePointLights[MAX_POINT_LIGHTS];
		unsigned int activePointCount = 0;

		if (mainWindow.getlampara()) {
			if (activePointCount < MAX_POINT_LIGHTS)
				activePointLights[activePointCount++] = pointLights[0];
		}

		if (mainWindow.getLuzHotdog()) {
			if (activePointCount < MAX_POINT_LIGHTS)
				activePointLights[activePointCount++] = pointLights[1];
		}

		shaderList[0].SetPointLights(activePointLights, activePointCount);




		
		//SWITCH CARRO
		if (mainWindow.getfaro() == true)
		{
			shaderList[0].SetSpotLights(spotLights, spotLightCount);
			shaderList[0].SetSpotLights(spotLights2, spotLightCount - 2);
		}
		//Se apaga lampara
		else if (mainWindow.getfaro() == false)
		{
			//No se manda al shader
			shaderList[0].SetSpotLights(spotLights2, spotLightCount);
			shaderList[0].SetSpotLights(spotLights, spotLightCount - 2);
		}




		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Instancia de lampara
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Lampara.RenderModel();

		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));

		//Carro
		model = glm::mat4(1.0);
		glm::vec3 cochePos = glm::vec3(2.0f + mainWindow.getmuevex(), 0.8f, 0.0f);
		model = glm::translate(model, cochePos);
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CarroSeparado.RenderModel();

		//LUZ LIGADA AL CARRO
		cochePos = glm::vec3(mainWindow.getmuevex(), 0.0f, 0.0f);
		spotLights[0].SetFlash(cochePos, glm::vec3(-1.0f, -0.2f, 0.0f));
		cochePos.x += 3.6f;
		spotLights2[0].SetFlash(cochePos, glm::vec3(1.0f, -0.2f, 0.0f));
		model = modelaux;



		//LLANTAS
		// color = glm::vec3(0.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		//Llanta Delantera Izquierda 
		modelaux = model;
		model = glm::translate(model, glm::vec3(1.65f, -1.0f, 2.23f));
		model = glm::rotate(model, glm::radians(90.0f - (mainWindow.getmuevex()) * 100), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaIzqAd1.RenderModel();
		model = modelaux;

		//Llanta Delantera Derecha
		modelaux = model;
		model = glm::translate(model, glm::vec3(-1.8f, -1.0f, 2.23f));
		model = glm::rotate(model, glm::radians(90.0f - (mainWindow.getmuevex()) * 100), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaDerAd1.RenderModel();
		model = modelaux;

		//Llanta Trasera Izquierda
		modelaux = model;
		model = glm::translate(model, glm::vec3(1.60f, -1.0f, -2.8f));
		model = glm::rotate(model, glm::radians(90.0f - (mainWindow.getmuevex()) * 100), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaIzqAtras1.RenderModel();
		model = modelaux;

		//Llanta Trasera Derecha
		modelaux = model;
		model = glm::translate(model, glm::vec3(-1.9f, -1.0f, -2.8f));
		model = glm::rotate(model, glm::radians(90.0f - (mainWindow.getmuevex()) * 100), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaDerAtras1.RenderModel();
		model = modelaux;

	


		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -4.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));


		//HotDog
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -14.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hotdog.RenderModel();

		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		glDisable(GL_BLEND);


		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
};