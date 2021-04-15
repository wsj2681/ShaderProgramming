#include "stdafx.h"
#include "Renderer.h"
#include "LoadPng.h"
#include <Windows.h>
#include <cstdlib>
#include <cassert>

Renderer::Renderer(int windowSizeX, int windowSizeY)
{
	//default settings
	glClearDepth(1.f);

	Initialize(windowSizeX, windowSizeY);
}


Renderer::~Renderer()
{
}

void Renderer::Initialize(int windowSizeX, int windowSizeY)
{
	//Set window size
	m_WindowSizeX = windowSizeX;
	m_WindowSizeY = windowSizeY;

	//Load shaders
	m_SolidRectShader = CompileShaders("./Shaders/SolidRect.vs", "./Shaders/SolidRect.fs");
	m_FSSandBoxShader = CompileShaders("./Shaders/FSSandBox.vs", "./Shaders/FSSandBox.fs");

	//Create VBOs
	CreateVertexBufferObjects();

	//Initialize camera settings
	m_v3Camera_Position = glm::vec3(0.f, 0.f, 1000.f);
	m_v3Camera_Lookat = glm::vec3(0.f, 0.f, 0.f);
	m_v3Camera_Up = glm::vec3(0.f, 1.f, 0.f);
	m_m4View = glm::lookAt(
		m_v3Camera_Position,
		m_v3Camera_Lookat,
		m_v3Camera_Up
	);

	//Initialize projection matrix
	m_m4OrthoProj = glm::ortho(
		-(float)windowSizeX / 2.f, (float)windowSizeX / 2.f,
		-(float)windowSizeY / 2.f, (float)windowSizeY / 2.f,
		0.0001f, 10000.f);
	m_m4PersProj = glm::perspectiveRH(45.f, 1.f, 1.f, 1000.f);

	//Initialize projection-view matrix
	m_m4ProjView = m_m4OrthoProj * m_m4View; //use ortho at this time
	//m_m4ProjView = m_m4PersProj * m_m4View;

	//Initialize model transform matrix :; used for rotating quad normal to parallel to camera direction
	m_m4Model = glm::rotate(glm::mat4(1.0f), glm::radians(0.f), glm::vec3(1.f, 0.f, 0.f));

	float3 rect[] = { float3(0.f, 0.f, 0.f), float3(1.f, 1.f, 0.f), float3(1.f, 0.f, 0.f) };

	glGenBuffers(1, &m_VBOTri);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOTri);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rect), rect, GL_STATIC_DRAW);


	float3 rect2[] = { float3(0.f, 0.f, 0.f), float3(-1.f, 0.f, 0.f), float3(-1.f, 1.f, 0.f) };
	glGenBuffers(1, &m_VBOTri2);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOTri2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rect2), rect2, GL_STATIC_DRAW);

	float rectSize = 1.f;
	float3 rect3[] = 
	{ 
		float3(-rectSize, -rectSize, 0.f), float3(-rectSize, rectSize, 0.f), float3(rectSize, rectSize, 0.f),
		float3(-rectSize, -rectSize, 0.f), float3(rectSize, rectSize, 0.f), float3(rectSize, -rectSize, 0.f)
	};

	glGenBuffers(1, &m_VBOFSSandBox);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOFSSandBox);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rect3), rect3, GL_STATIC_DRAW);

	//Particle
	CreateParticle(10000);
}

void Renderer::CreateVertexBufferObjects()
{
	float3 rect[] = { 
		float3(0.25f, 0.25f, 0.f), float3(-0.25f, -0.25f, 0.f), float3(0.25, -0.25, 0.f), 
		float3(0.25f, 0.25f, 0.f), float3(-0.25f, -0.25f, 0.f), float3(-0.25f, 0.25f, 0.f) };

	glGenBuffers(1, &m_VBORect);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBORect);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rect), rect, GL_STATIC_DRAW);

	//float3 rect2[] = { float3(1.f, 0.f, 0.f), float3(1.f, 1.f, 0.f), float3(0.f, 1.f, 0.f) };

	//glGenBuffers(2, &m_VBORect);
	//glBindBuffer(GL_ARRAY_BUFFER, m_VBORect);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(rect2), rect, GL_STATIC_DRAW);

}

void Renderer::CreateParticle(int count)
{
	int floatCount = count * 
		(3/*vertexPosition*/ + 3/*vertexSpeed*/ + 
		1/*EmitTime*/ + 1/*LifeTime*/ + 
		2/*vibration*/ + 1/*randomValue*/ + 4 /*RGBA*/) * 3 * 2;

	float* particleVertices = new float[floatCount];
	int vertexCount = count * 3 * 2;

	int index = 0;
	float particleSize = 0.01f;

	for (int i = 0; i < count; ++i)
	{
		float randomValueX = 0.f, randomValueY = 0.f, randomValueZ = 0.f;
		float randomValueVX = 1.f, randomValueVY = 0.f, randomValueVZ = 0.f;
		float randomEmitTime = 0.f, randomLifeTime = 2.f;
		float randomPeriod = 1.f, randomAmp = 1.f;	
		float randomValue = 0.f;
		float randomRed = 0.f, randomGreen = 0.f, randomBlue = 0.f, randomAlpha = 0.f;

		//randomValueX = ((float)rand() / (float)RAND_MAX - 0.5f) * 2.f;
		//randomValueY = ((float)rand() / (float)RAND_MAX - 0.5f) * 2.f;
		//randomValueZ = 0.f;
		
		randomValueVX = ((float)rand() / (float)RAND_MAX - 0.5f) * 0.1f;
		randomValueVY = ((float)rand() / (float)RAND_MAX - 0.5f) * 0.1f;
		randomValueVZ = 0.f;
		
		randomEmitTime = ((float)rand() / (float)RAND_MAX) * 10.f;
		randomLifeTime = ((float)rand() / (float)RAND_MAX) * 2.f;
		randomPeriod = ((float)rand() / (float)RAND_MAX) * 10.f + 1.f;
		randomAmp = ((float)rand() / (float)RAND_MAX) * 0.02f - 0.01f;

		randomValue = ((float)rand() / (float)RAND_MAX);

		randomRed = ((float)rand() / (float)RAND_MAX);
		randomGreen = ((float)rand() / (float)RAND_MAX);
		randomBlue = ((float)rand() / (float)RAND_MAX);
		randomAlpha = ((float)rand() / (float)RAND_MAX);

		//v0	position
		particleVertices[index] = -particleSize / 2.f + randomValueX;
		index++;
		particleVertices[index] = -particleSize / 2.f + randomValueY;
		index++;
		particleVertices[index] = 0.f;
		index++;

		//v0	speed;
		particleVertices[index] = randomValueVX;
		index++;
		particleVertices[index] = randomValueVY;
		index++;
		particleVertices[index] = 0.f;
		index++;

		//randomEmitTime
		particleVertices[index] = randomEmitTime;
		index++;

		//randomLifeTime
		particleVertices[index] = randomLifeTime;
		index++;

		//randomPeriod
		particleVertices[index] = randomPeriod;
		index++;
		//randomAmp
		particleVertices[index] = randomAmp;
		index++;
		//randomValue
		particleVertices[index] = randomValue;
		index++;

		//randomRGBA
		particleVertices[index] = randomRed;
		index++;
		particleVertices[index] = randomGreen;
		index++;
		particleVertices[index] = randomBlue;
		index++;
		particleVertices[index] = randomAlpha;
		index++;
		//v1
		particleVertices[index] = particleSize / 2.f + randomValueX;
		index++;
		particleVertices[index] = -particleSize / 2.f + randomValueY;
		index++;
		particleVertices[index] = 0.f;
		index++;

		particleVertices[index] = randomValueVX;
		index++;
		particleVertices[index] = randomValueVY;
		index++;
		particleVertices[index] = 0.f;
		index++;

		particleVertices[index] = randomEmitTime;
		index++;
		particleVertices[index] = randomLifeTime;
		index++;

		particleVertices[index] = randomPeriod;
		index++;
		particleVertices[index] = randomAmp;
		index++;
		particleVertices[index] = randomValue;
		index++;
		//randomRGBA
		particleVertices[index] = randomRed;
		index++;
		particleVertices[index] = randomGreen;
		index++;
		particleVertices[index] = randomBlue;
		index++;
		particleVertices[index] = randomAlpha;
		index++;
		//v2
		particleVertices[index] = particleSize / 2.f + randomValueX;
		index++;
		particleVertices[index] = particleSize / 2.f + randomValueY;
		index++;
		particleVertices[index] = 0.f;
		index++;

		particleVertices[index] = randomValueVX;
		index++;
		particleVertices[index] = randomValueVY;
		index++;
		particleVertices[index] = 0.f;
		index++;

		particleVertices[index] = randomEmitTime;
		index++;
		particleVertices[index] = randomLifeTime;
		index++;

		particleVertices[index] = randomPeriod;
		index++;
		particleVertices[index] = randomAmp;
		index++;
		particleVertices[index] = randomValue;
		index++;
		//randomRGBA
		particleVertices[index] = randomRed;
		index++;
		particleVertices[index] = randomGreen;
		index++;
		particleVertices[index] = randomBlue;
		index++;
		particleVertices[index] = randomAlpha;
		index++;
		//v3
		particleVertices[index] = -particleSize / 2.f + randomValueX;
		index++;
		particleVertices[index] = -particleSize / 2.f + randomValueY;
		index++;
		particleVertices[index] = 0.f;
		index++;

		particleVertices[index] = randomValueVX;
		index++;
		particleVertices[index] = randomValueVY;
		index++;
		particleVertices[index] = 0.f;
		index++;

		particleVertices[index] = randomEmitTime;
		index++;
		particleVertices[index] = randomLifeTime;
		index++;

		particleVertices[index] = randomPeriod;
		index++;
		particleVertices[index] = randomAmp;
		index++;
		particleVertices[index] = randomValue;
		index++;
		//randomRGBA
		particleVertices[index] = randomRed;
		index++;
		particleVertices[index] = randomGreen;
		index++;
		particleVertices[index] = randomBlue;
		index++;
		particleVertices[index] = randomAlpha;
		index++;

		//v4
		particleVertices[index] = particleSize / 2.f + randomValueX;
		index++;
		particleVertices[index] = particleSize / 2.f + randomValueY;
		index++;
		particleVertices[index] = 0.f;
		index++;

		particleVertices[index] = randomValueVX;
		index++;
		particleVertices[index] = randomValueVY;
		index++;
		particleVertices[index] = 0.f;
		index++;

		particleVertices[index] = randomEmitTime;
		index++;
		particleVertices[index] = randomLifeTime;
		index++;

		particleVertices[index] = randomPeriod;
		index++;
		particleVertices[index] = randomAmp;
		index++;
		particleVertices[index] = randomValue;
		index++;
		//randomRGBA
		particleVertices[index] = randomRed;
		index++;
		particleVertices[index] = randomGreen;
		index++;
		particleVertices[index] = randomBlue;
		index++;
		particleVertices[index] = randomAlpha;
		index++;
		//v5
		particleVertices[index] = -particleSize / 2.f + randomValueX;
		index++;
		particleVertices[index] = particleSize / 2.f + randomValueY;
		index++;
		particleVertices[index] = 0.f;
		index++;

		particleVertices[index] = randomValueVX;
		index++;
		particleVertices[index] = randomValueVY;
		index++;
		particleVertices[index] = 0.f;
		index++;

		particleVertices[index] = randomEmitTime;
		index++;
		particleVertices[index] = randomLifeTime;
		index++;

		particleVertices[index] = randomPeriod;
		index++;
		particleVertices[index] = randomAmp;
		index++;
		particleVertices[index] = randomValue;
		index++;
		//randomRGBA
		particleVertices[index] = randomRed;
		index++;
		particleVertices[index] = randomGreen;
		index++;
		particleVertices[index] = randomBlue;
		index++;
		particleVertices[index] = randomAlpha;
		index++;
	}

	glGenBuffers(1, &m_VBOManyParticle);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOManyParticle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * floatCount, particleVertices, GL_STATIC_DRAW);
	m_VBOManyParticleCount = vertexCount;
}

void Renderer::AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	//쉐이더 오브젝트 생성
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
	}

	const GLchar* p[1];
	p[0] = pShaderText;
	GLint Lengths[1];
	Lengths[0] = (GLint)strlen(pShaderText);
	//쉐이더 코드를 쉐이더 오브젝트에 할당
	glShaderSource(ShaderObj, 1, p, Lengths);

	//할당된 쉐이더 코드를 컴파일
	glCompileShader(ShaderObj);

	GLint success;
	// ShaderObj 가 성공적으로 컴파일 되었는지 확인
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];

		//OpenGL 의 shader log 데이터를 가져옴
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		printf("%s \n", pShaderText);
	}

	// ShaderProgram 에 attach!!
	glAttachShader(ShaderProgram, ShaderObj);
}

bool Renderer::ReadFile(char* filename, std::string *target)
{
	std::ifstream file(filename);
	if (file.fail())
	{
		std::cout << filename << " file loading failed.. \n";
		file.close();
		return false;
	}
	std::string line;
	while (getline(file, line)) {
		target->append(line.c_str());
		target->append("\n");
	}
	return true;
}

GLuint Renderer::CompileShaders(char* filenameVS, char* filenameFS)
{
	GLuint ShaderProgram = glCreateProgram(); //빈 쉐이더 프로그램 생성

	if (ShaderProgram == 0) { //쉐이더 프로그램이 만들어졌는지 확인
		fprintf(stderr, "Error creating shader program\n");
	}

	std::string vs, fs;

	//shader.vs 가 vs 안으로 로딩됨
	if (!ReadFile(filenameVS, &vs)) {
		printf("Error compiling vertex shader\n");
		return -1;
	};

	//shader.fs 가 fs 안으로 로딩됨
	if (!ReadFile(filenameFS, &fs)) {
		printf("Error compiling fragment shader\n");
		return -1;
	};

	// ShaderProgram 에 vs.c_str() 버텍스 쉐이더를 컴파일한 결과를 attach함
	AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);

	// ShaderProgram 에 fs.c_str() 프레그먼트 쉐이더를 컴파일한 결과를 attach함
	AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	//Attach 완료된 shaderProgram 을 링킹함
	glLinkProgram(ShaderProgram);

	//링크가 성공했는지 확인
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);

	if (Success == 0) {
		// shader program 로그를 받아옴
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << filenameVS << ", " << filenameFS << " Error linking shader program\n" << ErrorLog;
		return -1;
	}

	glValidateProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << filenameVS << ", " << filenameFS << " Error validating shader program\n" << ErrorLog;
		return -1;
	}

	glUseProgram(ShaderProgram);
	std::cout << filenameVS << ", " << filenameFS << " Shader compiling is done.\n";

	return ShaderProgram;
}
unsigned char * Renderer::loadBMPRaw(const char * imagepath, unsigned int& outWidth, unsigned int& outHeight)
{
	std::cout << "Loading bmp file " << imagepath << " ... " << std::endl;
	outWidth = -1;
	outHeight = -1;
	// Data read from the header of the BMP file
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	// Actual RGB data
	unsigned char * data;

	// Open the file
	FILE * file = NULL;
	fopen_s(&file, imagepath, "rb");
	if (!file)
	{
		std::cout << "Image could not be opened, " << imagepath << " is missing. " << std::endl;
		return NULL;
	}

	if (fread(header, 1, 54, file) != 54)
	{
		std::cout << imagepath << " is not a correct BMP file. " << std::endl;
		return NULL;
	}

	if (header[0] != 'B' || header[1] != 'M')
	{
		std::cout << imagepath << " is not a correct BMP file. " << std::endl;
		return NULL;
	}

	if (*(int*)&(header[0x1E]) != 0)
	{
		std::cout << imagepath << " is not a correct BMP file. " << std::endl;
		return NULL;
	}

	if (*(int*)&(header[0x1C]) != 24)
	{
		std::cout << imagepath << " is not a correct BMP file. " << std::endl;
		return NULL;
	}

	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	outWidth = *(int*)&(header[0x12]);
	outHeight = *(int*)&(header[0x16]);

	if (imageSize == 0)
		imageSize = outWidth * outHeight * 3;

	if (dataPos == 0)
		dataPos = 54;

	data = new unsigned char[imageSize];

	fread(data, 1, imageSize, file);

	fclose(file);

	std::cout << imagepath << " is succesfully loaded. " << std::endl;

	return data;
}

GLuint Renderer::CreatePngTexture(char * filePath)
{
	//Load Pngs: Load file and decode image.
	std::vector<unsigned char> image;
	unsigned width, height;
	unsigned error = lodepng::decode(image, width, height, filePath);
	if (error != 0)
	{
		lodepng_error_text(error);
		assert(error == 0);
		return -1;
	}

	GLuint temp;
	glGenTextures(1, &temp);

	glBindTexture(GL_TEXTURE_2D, temp);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);

	return temp;
}

GLuint Renderer::CreateBmpTexture(char * filePath)
{
	//Load Bmp: Load file and decode image.
	unsigned int width, height;
	unsigned char * bmp
		= loadBMPRaw(filePath, width, height);

	if (bmp == NULL)
	{
		std::cout << "Error while loading bmp file : " << filePath << std::endl;
		assert(bmp != NULL);
		return -1;
	}

	GLuint temp;
	glGenTextures(1, &temp);

	glBindTexture(GL_TEXTURE_2D, temp);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bmp);

	return temp;
}

void Renderer::Test()
{
	glUseProgram(m_SolidRectShader);

	GLint triLocation = glGetAttribLocation(m_SolidRectShader, "a_Position");
	glEnableVertexAttribArray(triLocation); // (Location, 다렉에 register set이랑 비슷);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOTri);
	glVertexAttribPointer(triLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	GLint triLocation2 = glGetAttribLocation(m_SolidRectShader, "a_Position1");
	glEnableVertexAttribArray(triLocation2);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOTri2);
	glVertexAttribPointer(triLocation2, 3, GL_FLOAT, GL_FALSE, 0, 0);

	static float gScale = 0.f;
	
	GLint scaleUniform = glGetUniformLocation(m_SolidRectShader, "scale");
	glUniform1f(scaleUniform, gScale);

	GLint colorUniform = glGetUniformLocation(m_SolidRectShader, "color");
	glUniform4f(colorUniform, gScale, 1, 1, 1);

	GLint positionUniform = glGetUniformLocation(m_SolidRectShader, "position");
	glUniform3f(positionUniform, gScale, gScale, 0);

	// (Primitive Type, firstIndex, VertexCount)
	glDrawArrays(GL_TRIANGLES, 0, 3);

	gScale += 0.01f;

	if (gScale > 1.f)
	{
		gScale = 0.f;
	}
	glDisableVertexAttribArray(triLocation);
	glDisableVertexAttribArray(triLocation2);
}

float g_time = 0.f;

void Renderer::Particle()
{
	GLuint shader = m_SolidRectShader;
	glUseProgram(shader); // ShaderProgram;


	GLint VBOLocation = glGetAttribLocation(shader, "a_Position");
	glEnableVertexAttribArray(VBOLocation);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOManyParticle);
	glVertexAttribPointer(VBOLocation, 3, GL_FLOAT, GL_FALSE,
		/*Stride*/sizeof(float) * 15, /*Offset*/(GLvoid*)(sizeof(float) * 0));

	GLint VBOValocityLocation = glGetAttribLocation(shader, "a_Valocity");
	glEnableVertexAttribArray(VBOValocityLocation);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOManyParticle);
	glVertexAttribPointer(VBOValocityLocation, 3, GL_FLOAT, GL_FALSE,
		/*Stride*/sizeof(float) * 15, /*Offset*/(GLvoid*)(sizeof(float) * 3));

	GLint VBOEmitLocation = glGetAttribLocation(shader, "a_EmitTime");
	glEnableVertexAttribArray(VBOEmitLocation);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOManyParticle);
	glVertexAttribPointer(VBOEmitLocation, 1, GL_FLOAT, GL_FALSE,
		/*Stride*/sizeof(float) * 15, /*Offset*/(GLvoid*)(sizeof(float) * 6));

	GLint VBOLifeLocation = glGetAttribLocation(shader, "a_LifeTime");
	glEnableVertexAttribArray(VBOLifeLocation);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOManyParticle);
	glVertexAttribPointer(VBOLifeLocation, 1, GL_FLOAT, GL_FALSE,
		/*Stride*/sizeof(float) * 15, /*Offset*/(GLvoid*)(sizeof(float) * 7));

	GLint VBOPeriodLocation = glGetAttribLocation(shader, "a_Period");
	glEnableVertexAttribArray(VBOPeriodLocation);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOManyParticle);
	glVertexAttribPointer(VBOPeriodLocation, 1, GL_FLOAT, GL_FALSE,
		/*Stride*/sizeof(float) * 15, /*Offset*/(GLvoid*)(sizeof(float) * 8));

	GLint VBOAmpLocation = glGetAttribLocation(shader, "a_Amp");
	glEnableVertexAttribArray(VBOAmpLocation);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOManyParticle);
	glVertexAttribPointer(VBOAmpLocation, 1, GL_FLOAT, GL_FALSE,
		/*Stride*/sizeof(float) * 15, /*Offset*/(GLvoid*)(sizeof(float) * 9));

	GLint VBORandLocation = glGetAttribLocation(shader, "a_RandValue");
	glEnableVertexAttribArray(VBORandLocation);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOManyParticle);
	glVertexAttribPointer(VBORandLocation, 1, GL_FLOAT, GL_FALSE,
		/*Stride*/sizeof(float) * 15, /*Offset*/(GLvoid*)(sizeof(float) * 10));

	GLint VBOColorLocation = glGetAttribLocation(shader, "a_Color");
	glEnableVertexAttribArray(VBOColorLocation);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOManyParticle);
	glVertexAttribPointer(VBOColorLocation, 3, GL_FLOAT, GL_FALSE,
		/*Stride*/sizeof(float) * 15, /*Offset*/(GLvoid*)(sizeof(float) * 11));

	GLint forceUniform = glGetUniformLocation(shader, "u_ExForce");
	glUniform3f(forceUniform, sin(g_time), cos(g_time), 0);

	GLint timeUniform = glGetUniformLocation(shader, "u_time");
	glUniform1f(timeUniform, g_time);

	glDrawArrays(GL_TRIANGLES, 0, m_VBOManyParticleCount);

	g_time = g_time + 0.016;

}

void Renderer::FSSandBox()
{
	GLuint shader = m_FSSandBoxShader;
	glUseProgram(shader); // ShaderProgram;

	GLuint attribPosLoc = glGetAttribLocation(shader, "a_Position");
	glEnableVertexAttribArray(attribPosLoc);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOFSSandBox);
	glVertexAttribPointer(attribPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(float3) * 1, (GLvoid*)(0));

	GLint resolution = glGetUniformLocation(m_FSSandBoxShader, "iResolution");
	glUniform2f(resolution, 500, 500);

	GLint time = glGetUniformLocation(m_FSSandBoxShader, "time");
	glUniform1f(time, g_time);

	GLint mouse = glGetUniformLocation(m_FSSandBoxShader, "mouse");
	glUniform2f(mouse, g_time, g_time);

	GLint point = glGetUniformLocation(m_FSSandBoxShader, "u_Point");
	glUniform3f(point, 0.5f, 0.5, 0.1f);

	float pointArray[] =
	{
		-0.5f, -0.5f, 0.1f, 
		-0.4f, -0.4f, 0.1f,
		-0.3f, -0.3f, 0.1f,
		-0.2f, -0.2f, 0.1f,
		-0.1f, -0.1f, 0.1f,
		0.5f, 0.5f, 0.1f,
		0.4f, 0.4f, 0.1f,
		0.3f, 0.3f, 0.1f,
		0.2f, 0.2f, 0.1f,
		0.1f, 0.1f, 0.1f
	};

	GLint points = glGetUniformLocation(m_FSSandBoxShader, "u_Points");
	glUniform3fv(points, 10, pointArray);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	g_time = g_time + 0.016;
}
