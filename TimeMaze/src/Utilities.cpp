#include "Utilities.h"

#include <iostream>

using namespace std;
using namespace glm;
using namespace GLLib;

unique_ptr<Renderer> Renderer::instance(nullptr);
unique_ptr<Director> Director::instance(nullptr);
unique_ptr<Keyboard> Keyboard::instance(nullptr);
unique_ptr<Animation> Animation::instance(nullptr);

int Renderer::screenWidth = 1024;
int Renderer::screenHeight = 768;

void Renderer::init(GLFWwindow* window)
{
	if (!instance.get()) {
		const size_t OITBufferSize = 3 * screenWidth * screenHeight;
		
		instance.reset(new Renderer);

		instance->m_window = window;

		/********************** GLSL Shader Programs Creating Begin **********************/

		string info;

		GLShader genVshader(GL_VERTEX_SHADER);
		genVshader.loadFile("res/shader/general.vert");
		genVshader.compile(info);
		cout << info.c_str() << endl;

		GLShader genGshader(GL_GEOMETRY_SHADER);
		genGshader.loadFile("res/shader/general.geom");
		genGshader.compile(info);
		cout << info.c_str() << endl;

		GLShader lightFshader(GL_FRAGMENT_SHADER);
		lightFshader.loadFile("res/shader/lighting.frag");
		lightFshader.compile(info);
		cout << info.c_str() << endl;

		GLShader shadowVshader(GL_VERTEX_SHADER);
		shadowVshader.loadFile("res/shader/shadowMap.vert");
		shadowVshader.compile(info);
		cout << info.c_str() << endl;

		GLShader shadowFshader(GL_FRAGMENT_SHADER);
		shadowFshader.loadFile("res/shader/shadowMap.frag");
		shadowFshader.compile(info);
		cout << info.c_str() << endl;

		GLShader blendCtrFshader(GL_FRAGMENT_SHADER);
		blendCtrFshader.loadFile("res/shader/blendCtr.frag");
		blendCtrFshader.compile(info);
		cout << info.c_str() << endl;

		GLShader screenVshader(GL_VERTEX_SHADER);
		screenVshader.loadFile("res/shader/screenQuad.vert");
		screenVshader.compile(info);
		cout << info.c_str() << endl;

		GLShader blendFshader(GL_FRAGMENT_SHADER);
		blendFshader.loadFile("res/shader/blend.frag");
		blendFshader.compile(info);
		cout << info.c_str() << endl;

		GLShader splitFshader(GL_FRAGMENT_SHADER);
		splitFshader.loadFile("res/shader/bloomSplit.frag");
		splitFshader.compile(info);
		cout << info.c_str() << endl;

		GLShader mergeFshader(GL_FRAGMENT_SHADER);
		mergeFshader.loadFile("res/shader/bloomMerge.frag");
		mergeFshader.compile(info);
		cout << info.c_str() << endl;

		GLShader blurFshader(GL_FRAGMENT_SHADER);
		blurFshader.loadFile("res/shader/gaussianBlur.frag");
		blurFshader.compile(info);
		cout << info.c_str() << endl;

		shared_ptr<GLShaderProgram> program(new GLShaderProgram);
		program->attachShader(genVshader);
		program->attachShader(genGshader);
		program->attachShader(lightFshader);
		program->build(info);
		cout << info.c_str() << endl;

		shared_ptr<GLShaderProgram> shadowProgram(new GLShaderProgram);
		shadowProgram->attachShader(shadowVshader);
		shadowProgram->attachShader(shadowFshader);
		shadowProgram->build(info);
		cout << info.c_str() << endl;

		shared_ptr<GLShaderProgram> blendCtrProgram(new GLShaderProgram);
		blendCtrProgram->attachShader(genVshader);
		blendCtrProgram->attachShader(genGshader);
		blendCtrProgram->attachShader(blendCtrFshader);
		blendCtrProgram->build(info);
		cout << info.c_str() << endl;

		shared_ptr<GLShaderProgram> blendProgram(new GLShaderProgram);
		blendProgram->attachShader(screenVshader);
		blendProgram->attachShader(blendFshader);
		blendProgram->build(info);
		cout << info.c_str() << endl;

		shared_ptr<GLShaderProgram> splitProgram(new GLShaderProgram);
		splitProgram->attachShader(screenVshader);
		splitProgram->attachShader(splitFshader);
		splitProgram->build(info);
		cout << info.c_str() << endl;

		shared_ptr<GLShaderProgram> blurProgram(new GLShaderProgram);
		blurProgram->attachShader(screenVshader);
		blurProgram->attachShader(blurFshader);
		blurProgram->build(info);
		cout << info.c_str() << endl;

		shared_ptr<GLShaderProgram> mergeProgram(new GLShaderProgram);
		mergeProgram->attachShader(screenVshader);
		mergeProgram->attachShader(mergeFshader);
		mergeProgram->build(info);
		cout << info.c_str() << endl;

		/********************** GLSL Shader Programs Creating End **********************/

		{
			vec2 posArr[] = {
				{ -1, -1 },
				{ 1, -1 },
				{ 1, 1 },
				{ -1, 1 }
			};
			vec2 texCoordArr[] = {
				{ 0, 0 },
				{ 1, 0 },
				{ 1, 1 },
				{ 0, 1 }
			};

			VertexArrayFactory geometryFactory;
			geometryFactory.pushAttribute(AttrDef(0, AttrDef::TWO, GL_FLOAT, 0, 0));
			geometryFactory.pushAttribute(AttrDef(1, AttrDef::TWO, GL_FLOAT, sizeof(posArr), 0));
			geometryFactory.allocData(sizeof(posArr) + sizeof(texCoordArr));
			geometryFactory.setData(posArr, 0, sizeof(posArr));
			geometryFactory.setData(texCoordArr, sizeof(posArr), sizeof(texCoordArr));

			unsigned int indices[] = {
				0, 1, 2,
				2, 3, 0
			};

			instance->m_screenQuadVAO = geometryFactory.createVAO(indices, sizeof(indices) / sizeof(unsigned int));
		}

		/********************** RenderProcess Creating Begin **********************/

		/* Shadow map */
		shared_ptr<Camera> shadowCamera(new Camera);
		shared_ptr<GLFramebuffer> shadow_fbo(new GLFramebuffer);
		shadowCamera->perspective(0.5f * pi<float>(), 1.0f, shadowZNear, shadowZFar);
		instance->m_shadowMapProcess.reset(new RenderProcess(shadowCamera, shadowProgram));
		instance->m_shadowMapProcess->setFramebuffer(shadow_fbo);
		instance->m_shadowMapProcess->setModelMatUniformLoc(shadowProgram->getUniformLocation("model"));
		instance->m_shadowMapProcess->setViewMatUniformLoc(shadowProgram->getUniformLocation("view"));
		instance->m_shadowMapProcess->setProjMatUniformLoc(shadowProgram->getUniformLocation("projection"));

		/* Screen Space */
		shared_ptr<GLFramebuffer> ss_fbo(new GLFramebuffer);
		instance->m_renderProcess.reset(new RenderProcess(instance->m_camera, program));
		instance->m_renderProcess->setFramebuffer(ss_fbo);
		instance->m_renderProcess->setModelMatUniformLoc(program->getUniformLocation("model"));
		instance->m_renderProcess->setViewMatUniformLoc(program->getUniformLocation("view"));
		instance->m_renderProcess->setProjMatUniformLoc(program->getUniformLocation("projection"));

		/* OIT Blending */
		instance->m_blendCtrProcess.reset(new RenderProcess(instance->m_camera, blendCtrProgram));
		instance->m_blendCtrProcess->setModelMatUniformLoc(blendCtrProgram->getUniformLocation("model"));
		instance->m_blendCtrProcess->setViewMatUniformLoc(blendCtrProgram->getUniformLocation("view"));
		instance->m_blendCtrProcess->setProjMatUniformLoc(blendCtrProgram->getUniformLocation("projection"));

		shared_ptr<GLFramebuffer> blend_fbo(new GLFramebuffer);
		instance->m_blendProcess.reset(new RenderProcess(nullptr, blendProgram));
		instance->m_blendProcess->setFramebuffer(blend_fbo);
		instance->m_blendProcess->addObject(instance->m_screenQuadVAO.vao);

		/* Bloom Effect */
		shared_ptr<GLFramebuffer> split_fbo(new GLFramebuffer);
		instance->m_bloomSplitProcess.reset(new RenderProcess(nullptr, splitProgram));
		instance->m_bloomSplitProcess->setFramebuffer(split_fbo);
		instance->m_bloomSplitProcess->addObject(instance->m_screenQuadVAO.vao);

		shared_ptr<GLFramebuffer> merge_fbo(new GLFramebuffer);
		instance->m_bloomMergeProcess.reset(new RenderProcess(nullptr, mergeProgram));
		instance->m_bloomMergeProcess->setFramebuffer(merge_fbo);
		instance->m_bloomMergeProcess->addObject(instance->m_screenQuadVAO.vao);

		shared_ptr<GLFramebuffer> blur_fbo(new GLFramebuffer);
		instance->m_blurProcess.reset(new RenderProcess(nullptr, blurProgram));
		instance->m_blurProcess->setFramebuffer(blur_fbo);
		instance->m_blurProcess->addObject(instance->m_screenQuadVAO.vao);

		/********************** RenderProcess Creating End **********************/

		/********************** General GLBuffer & GLTexture Creating Begin **********************/

		/* Shadow map */
		for (int i = 0; i < 16; ++i) {
			instance->m_cubeShadowMap[i].reset(new GLTexture);
			instance->m_cubeShadowMap[i]->bind(0, GL_TEXTURE_CUBE_MAP);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_DEPTH_COMPONENT16, shadowmapSize, shadowmapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_DEPTH_COMPONENT16, shadowmapSize, shadowmapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_DEPTH_COMPONENT16, shadowmapSize, shadowmapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_DEPTH_COMPONENT16, shadowmapSize, shadowmapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_DEPTH_COMPONENT16, shadowmapSize, shadowmapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_DEPTH_COMPONENT16, shadowmapSize, shadowmapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		}

		shadow_fbo->bind();
		glDrawBuffer(GL_NONE);

		/* Screen Space */
		instance->m_ssColorTexture.reset(new GLTexture);
		instance->m_ssColorTexture->bind(0, GL_TEXTURE_2D_MULTISAMPLE);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, GL_RGB32F, screenWidth, screenHeight, GL_TRUE);

		instance->m_ssDepthTexture.reset(new GLTexture);
		instance->m_ssDepthTexture->bind(0, GL_TEXTURE_2D_MULTISAMPLE);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, GL_R32F, screenWidth, screenHeight, GL_TRUE);

		instance->m_ssDepthRenderbuffer.reset(new GLRenderbuffer);
		instance->m_ssDepthRenderbuffer->bind();
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 8, GL_DEPTH_COMPONENT16, screenWidth, screenHeight);

		ss_fbo->bind();
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, *instance->m_ssColorTexture, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D_MULTISAMPLE, *instance->m_ssDepthTexture, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, *instance->m_ssDepthRenderbuffer);
		GLenum ssDrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, ssDrawBuffers);

		/* OIT Blending */
		instance->m_atomicCounter.reset(new GLBuffer);
		glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, *instance->m_atomicCounter);
		glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), nullptr, GL_DYNAMIC_COPY);

		instance->m_fragmentStorageBuffer.reset(new GLBuffer);
		instance->m_fragmentStorageBuffer->bind(GL_TEXTURE_BUFFER);
		glBufferData(GL_TEXTURE_BUFFER, OITBufferSize * 4 * sizeof(GLuint), nullptr, GL_DYNAMIC_COPY);

		instance->m_fragmentStorageBufferTexture.reset(new GLTexture);
		instance->m_fragmentStorageBufferTexture->bind(0, GL_TEXTURE_BUFFER);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32UI, *instance->m_fragmentStorageBuffer);
		glBindImageTexture(0, *instance->m_fragmentStorageBufferTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32UI);

		instance->m_ptrInitializer.reset(new GLBuffer);
		instance->m_ptrInitializer->bind(GL_PIXEL_UNPACK_BUFFER);
		glBufferData(GL_PIXEL_UNPACK_BUFFER, screenWidth * screenHeight * sizeof(GLuint), nullptr, GL_STATIC_DRAW);
		void* data = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
		memset(data, 0xFF, screenWidth * screenHeight * sizeof(GLuint));
		glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

		instance->m_headPtrTexture.reset(new GLTexture);
		instance->m_headPtrTexture->bind(0, GL_TEXTURE_2D);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, screenWidth, screenHeight, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glBindImageTexture(1, *instance->m_headPtrTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);

		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

		instance->m_blendedColorTexture.reset(new GLTexture);
		instance->m_blendedColorTexture->bind(0, GL_TEXTURE_2D);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		blend_fbo->bind();
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *instance->m_blendedColorTexture, 0);
		GLenum blDrawBuffers[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, blDrawBuffers);

		/* Bloom Effect */
		instance->m_baseColorTexture.reset(new GLTexture);
		instance->m_baseColorTexture->bind(0, GL_TEXTURE_2D);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		instance->m_highlightColorTexture.reset(new GLTexture);
		instance->m_highlightColorTexture->bind(0, GL_TEXTURE_2D);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		instance->m_bloomedRenderbuffer.reset(new GLRenderbuffer);
		instance->m_bloomedRenderbuffer->bind();
		glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB8, screenWidth, screenHeight);

		instance->m_bluredTexture.reset(new GLTexture);
		instance->m_bluredTexture->bind(0, GL_TEXTURE_2D);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		split_fbo->bind();
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *instance->m_baseColorTexture, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, *instance->m_highlightColorTexture, 0);
		GLenum splitDrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, splitDrawBuffers);

		merge_fbo->bind();
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, *instance->m_bloomedRenderbuffer);
		GLenum mergeDrawBuffers[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, mergeDrawBuffers);

		blur_fbo->bind();
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *instance->m_bluredTexture, 0);
		GLenum blurDrawBuffers[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, blurDrawBuffers);

		/********************** General GLBuffer & GLTexture Creating End **********************/

		/********************** GLSL Shader Programs Configuring Begin **********************/

		for (int i = 0; i < 16; ++i) {
			instance->m_lightColor[i] = vec3(1.0f);
			instance->m_lightStrength[i] = 1.0f;
		}

		int cubeShadowTexUnit[16] = { 4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19 };

		program->use();
		glUniform1iv(program->getUniformLocation("useLight"), 16, instance->m_useLight);
		glUniform3fv(program->getUniformLocation("wsLightPos"), 16, (GLfloat *)instance->m_lightPos);
		glUniform3fv(program->getUniformLocation("lightColor"), 16, (GLfloat *)instance->m_lightColor);
		glUniform1fv(program->getUniformLocation("lightStrength"), 16, (GLfloat *)instance->m_lightStrength);
		glUniform3f(program->getUniformLocation("ambientLightColor"), 0.1f, 0.1f, 0.1f);
		glUniform1iv(program->getUniformLocation("cubeShadowSampler"), 16, cubeShadowTexUnit);
		glUniform1f(program->getUniformLocation("shadowFar"), shadowZFar);
		glUniform1f(program->getUniformLocation("shadowNear"), shadowZNear);

		blendCtrProgram->use();
		glUniform1ui(blendCtrProgram->getUniformLocation("max_length"), OITBufferSize);

		mergeProgram->use();
		glUniform1f(mergeProgram->getUniformLocation("bloom"), 1.0f);

		float gaussianKernel[17];
		float sigma = 8.0;
		float Z = 0.0;
		//create the 1-D kernel
		for (int i = 0; i <= 8; ++i)
			gaussianKernel[8 + i] = gaussianKernel[8 - i] = static_cast<float>(0.39894 * exp(-0.5 * i * i / (sigma * sigma)) / sigma);
		//get the normalization factor (as the gaussian has been clamped)
		for (int i = 0; i < 17; ++i)
			Z += gaussianKernel[i];
		blurProgram->use();
		glUniform1fv(blurProgram->getUniformLocation("kernel"), 17, gaussianKernel);
		glUniform1f(blurProgram->getUniformLocation("Z"), Z);

		/********************** GLSL Shader Programs Configuring End **********************/

		/********************** OpenGL State Initialization Begin **********************/

		glClearColor(0, 0, 0, 1);
		glClearDepth(1);
		glDepthFunc(GL_LEQUAL);
		glPolygonOffset(2.0f, 8.0f);
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_POLYGON_SMOOTH);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

		/********************** OpenGL State Initialization End **********************/
	}
}

Renderer::~Renderer()
{
}

void Renderer::addRenderable(const shared_ptr<IAppear>& renderable)
{
	this->m_renderables.push_back(renderable);
}

void Renderer::removeRenderable(const shared_ptr<IAppear>& renderable)
{
	for (auto iterator = this->m_renderables.begin(); iterator != this->m_renderables.end(); ++iterator) {
		if (iterator->get() == renderable.get()) {
			this->m_renderables.erase(iterator);
			return;
		}
	}
}

void Renderer::clearRenderables()
{
	this->m_renderables.clear();
}

void Renderer::render()
{
	double time = glfwGetTime();
	double frameDuring = time - this->m_frameBeginTime;

	if (frameDuring >= frameTime) {
		/************************ Shadowmap Rendering Begin ************************/
		
		this->m_shadowMapProcess->clearObjects();
		for (shared_ptr<IAppear> renderable : this->m_renderables) {
			if (renderable->lightEffect() && !renderable->doBlend()) {
				this->m_shadowMapProcess->addObject(renderable->geometry());
			}
		}
		
		glEnable(GL_POLYGON_OFFSET_FILL);
		glViewport(0, 0, shadowmapSize, shadowmapSize);

		shared_ptr<Camera> shadowCamera = this->m_shadowMapProcess->getCamera();
		for (int i = 0; i < 16; ++i) {
			if (!this->m_useLight[i]) continue;

			shadowCamera->setPosition(this->m_lightPos[i]);

			this->m_shadowMapProcess->getFramebuffer()->bind();
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X, *this->m_cubeShadowMap[i], 0);
			glClear(GL_DEPTH_BUFFER_BIT);
			shadowCamera->lookTo(1, 0, 0);
			shadowCamera->setUpVector(0, -1, 0);
			this->m_shadowMapProcess->render();

			this->m_shadowMapProcess->getFramebuffer()->bind();
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, *this->m_cubeShadowMap[i], 0);
			glClear(GL_DEPTH_BUFFER_BIT);
			shadowCamera->lookTo(0, 1, 0);
			shadowCamera->setUpVector(0, 0, 1);
			this->m_shadowMapProcess->render();

			this->m_shadowMapProcess->getFramebuffer()->bind();
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, *this->m_cubeShadowMap[i], 0);
			glClear(GL_DEPTH_BUFFER_BIT);
			shadowCamera->lookTo(0, 0, 1);
			shadowCamera->setUpVector(0, -1, 0);
			this->m_shadowMapProcess->render();

			this->m_shadowMapProcess->getFramebuffer()->bind();
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, *this->m_cubeShadowMap[i], 0);
			glClear(GL_DEPTH_BUFFER_BIT);
			shadowCamera->lookTo(-1, 0, 0);
			shadowCamera->setUpVector(0, -1, 0);
			this->m_shadowMapProcess->render();

			this->m_shadowMapProcess->getFramebuffer()->bind();
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, *this->m_cubeShadowMap[i], 0);
			glClear(GL_DEPTH_BUFFER_BIT);
			shadowCamera->lookTo(0, -1, 0);
			shadowCamera->setUpVector(0, 0, -1);
			this->m_shadowMapProcess->render();

			this->m_shadowMapProcess->getFramebuffer()->bind();
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, *this->m_cubeShadowMap[i], 0);
			glClear(GL_DEPTH_BUFFER_BIT);
			shadowCamera->lookTo(0, 0, -1);
			shadowCamera->setUpVector(0, -1, 0);
			this->m_shadowMapProcess->render();
		}

		glDisable(GL_POLYGON_OFFSET_FILL);
		glViewport(0, 0, screenWidth, screenHeight);

		/************************ Shadowmap Rendering End ************************/

		/************************ Screen Space Rendering Begin ************************/

		vector< shared_ptr<IAppear> > blendingObjects;
		shared_ptr<GLShaderProgram> program = this->m_renderProcess->getPipeline();

		for (int i = 0; i < 16; ++i) {
			this->m_cubeShadowMap[i]->bind(4 + i, GL_TEXTURE_CUBE_MAP);
		}

		this->m_renderProcess->getFramebuffer()->bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		static const GLfloat depthClearValue[] = { 1.0f, 0.0f, 0.0f, 0.0f };
		glClearBufferfv(GL_COLOR, 1, depthClearValue);

		program->use();
		glUniform1f(program->getUniformLocation("time"), static_cast<float>(time));
		
		for (shared_ptr<IAppear> renderable : this->m_renderables) {
			if (!renderable->doBlend()) {
				vec3 color = renderable->color();
				float shininess = renderable->shininess();

				program->use();
				glUniform3f(program->getUniformLocation("materialColor"), color.r, color.g, color.b);
				glUniform1f(program->getUniformLocation("shininess"), shininess);
				glUniform1i(program->getUniformLocation("useColormap"), false);
				glUniform1i(program->getUniformLocation("useNormalmap"), false);
				glUniform1i(program->getUniformLocation("useEnvmap"), false);
				glUniform1i(program->getUniformLocation("useSpecmap"), false);
				glUniform1i(program->getUniformLocation("useAnimap"), false);
				glUniform1i(program->getUniformLocation("lighting"), renderable->lightEffect());
				glUniform1f(program->getUniformLocation("animationDuring"), renderable->animationDuring());
				glUniform1i(program->getUniformLocation("texColor_inverse"), renderable->texColorInverse());
				glUniform1ui(program->getUniformLocation("texChannel_link"), renderable->textureChannelLink());

				vector<IAppear::TexFlag> flags;
				renderable->textures(flags);
				for (IAppear::TexFlag flag : flags) {
					switch (flag.type)
					{
					case IAppear::TexFlag::COLORMAP:
						glUniform1i(program->getUniformLocation("useColormap"), true);
						flag.texture->bind(0, GL_TEXTURE_2D);
						break;
					case IAppear::TexFlag::NORMALMAP:
						glUniform1i(program->getUniformLocation("useNormalmap"), true);
						flag.texture->bind(1, GL_TEXTURE_2D);
						break;
					case IAppear::TexFlag::ENVMAP:
						glUniform1i(program->getUniformLocation("useEnvmap"), true);
						flag.texture->bind(2, GL_TEXTURE_CUBE_MAP);
						break;
					case IAppear::TexFlag::SPECMAP:
						glUniform1i(program->getUniformLocation("useSpecmap"), true);
						flag.texture->bind(3, GL_TEXTURE_2D);
						break;
					case IAppear::TexFlag::ANIMATED_COLORMAP:
						glUniform1i(program->getUniformLocation("useAnimap"), true);
						flag.texture->bind(20, GL_TEXTURE_3D);
						break;
					default:
						break;
					}
				}

				this->m_renderProcess->clearObjects();
				this->m_renderProcess->addObject(renderable->geometry());

				this->m_renderProcess->render();
			} else {
				blendingObjects.push_back(renderable);
			}
		}

		/************************ Screen Space Rendering End ************************/

		glDisable(GL_DEPTH_TEST);

		/************************ Blender Rendering Begin ************************/

		this->m_atomicCounter->bind(GL_ATOMIC_COUNTER_BUFFER);
		const GLuint zero = 0;
		glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(zero), &zero);
		
		this->m_ptrInitializer->bind(GL_PIXEL_UNPACK_BUFFER);
		this->m_headPtrTexture->bind(0, GL_TEXTURE_2D);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, screenWidth, screenHeight, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, nullptr);

		this->m_ssDepthTexture->bind(5, GL_TEXTURE_2D_MULTISAMPLE);

		shared_ptr<GLShaderProgram> blendCtrProgram = this->m_blendCtrProcess->getPipeline();
		
		blendCtrProgram->use();
		glUniform1f(blendCtrProgram->getUniformLocation("time"), static_cast<float>(time));

		for (shared_ptr<IAppear> renderable : blendingObjects) {
			vec3 color = renderable->color();
			
			GLuint blend_param = (renderable->blendParameter() << 8) | renderable->textureChannelLink();

			blendCtrProgram->use();
			glUniform3f(blendCtrProgram->getUniformLocation("materialColor"), color.r, color.g, color.b);
			glUniform1f(blendCtrProgram->getUniformLocation("alpha"), renderable->alpha());
			glUniform1i(blendCtrProgram->getUniformLocation("useColormap"), false);
			glUniform1i(blendCtrProgram->getUniformLocation("useNormalmap"), false);
			glUniform1i(blendCtrProgram->getUniformLocation("useEnvmap"), false);
			glUniform1i(blendCtrProgram->getUniformLocation("useSpecmap"), false);
			glUniform1i(blendCtrProgram->getUniformLocation("useAnimap"), false);
			glUniform1f(blendCtrProgram->getUniformLocation("animationDuring"), renderable->animationDuring());
			glUniform1i(blendCtrProgram->getUniformLocation("texColor_inverse"), renderable->texColorInverse());
			glUniform1i(blendCtrProgram->getUniformLocation("texAlpha_inverse"), renderable->texAlphaInverse());
			glUniform1ui(blendCtrProgram->getUniformLocation("blend_param"), blend_param);
			
			vector<IAppear::TexFlag> flags;
			renderable->textures(flags);
			for (IAppear::TexFlag flag : flags) {
				switch (flag.type)
				{
				case IAppear::TexFlag::COLORMAP:
					glUniform1i(blendCtrProgram->getUniformLocation("useColormap"), true);
					flag.texture->bind(0, GL_TEXTURE_2D);
					break;
				case IAppear::TexFlag::NORMALMAP:
					glUniform1i(blendCtrProgram->getUniformLocation("useNormalmap"), true);
					flag.texture->bind(1, GL_TEXTURE_2D);
					break;
				case IAppear::TexFlag::ENVMAP:
					glUniform1i(blendCtrProgram->getUniformLocation("useEnvmap"), true);
					flag.texture->bind(2, GL_TEXTURE_CUBE_MAP);
					break;
				case IAppear::TexFlag::SPECMAP:
					glUniform1i(blendCtrProgram->getUniformLocation("useSpecmap"), true);
					flag.texture->bind(3, GL_TEXTURE_2D);
					break;
				case IAppear::TexFlag::ANIMATED_COLORMAP:
					glUniform1i(blendCtrProgram->getUniformLocation("useAnimap"), true);
					flag.texture->bind(4, GL_TEXTURE_3D);
					break;
				default:
					break;
				}
			}

			if (renderable->doCullface()) {
				glEnable(GL_CULL_FACE);
			} else {
				glDisable(GL_CULL_FACE);
			}

			this->m_blendCtrProcess->clearObjects();
			this->m_blendCtrProcess->addObject(renderable->geometry());

			this->m_blendCtrProcess->render();
		}
		glEnable(GL_CULL_FACE);

		this->m_ssColorTexture->bind(0, GL_TEXTURE_2D_MULTISAMPLE);
		this->m_blendProcess->render();
		
		/************************ Blender Rendering End ************************/

		/************************ Bloom Effect Begin ************************/

		this->m_blendedColorTexture->bind(0, GL_TEXTURE_2D);
		this->m_bloomSplitProcess->render();
		this->m_highlightColorTexture->bind(0, GL_TEXTURE_2D);
		this->m_blurProcess->render();
		this->m_baseColorTexture->bind(0, GL_TEXTURE_2D);
		this->m_bluredTexture->bind(1, GL_TEXTURE_2D);
		this->m_bloomMergeProcess->render();

		/************************ Bloom Effect End ************************/

		/* Copy it to screen */
		this->m_bloomMergeProcess->getFramebuffer()->bind(GL_READ_FRAMEBUFFER);
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		GLFramebuffer::getScreen()->bind(GL_DRAW_FRAMEBUFFER);
		glDrawBuffer(GL_BACK);
		glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		glEnable(GL_DEPTH_TEST);

		/* Swap front and back buffers */
		glfwSwapBuffers(this->m_window);

		this->m_frameBeginTime = time;
	}
}

void Renderer::setAmbientLightColor(float r, float g, float b)
{
	shared_ptr<GLShaderProgram> program = this->m_renderProcess->getPipeline();
	program->use();
	glUniform3f(program->getUniformLocation("ambientLightColor"), r, g, b);
}

void Renderer::setLightPos(int index, float x, float y, float z)
{
	this->m_lightPos[index] = vec3(x, y, z);
	shared_ptr<GLShaderProgram> program = this->m_renderProcess->getPipeline();
	program->use();
	glUniform3fv(program->getUniformLocation("wsLightPos"), 16, (GLfloat *)instance->m_lightPos);
}

void Renderer::setLightColor(int index, float r, float g, float b)
{
	this->m_lightColor[index] = vec3(r, g, b);
	shared_ptr<GLShaderProgram> program = this->m_renderProcess->getPipeline();
	program->use();
	glUniform3fv(program->getUniformLocation("lightColor"), 16, (GLfloat *)instance->m_lightColor);
}

void Renderer::setLightStrength(int index, float s)
{
	this->m_lightStrength[index] = s;
	shared_ptr<GLShaderProgram> program = this->m_renderProcess->getPipeline();
	program->use();
	glUniform1fv(program->getUniformLocation("lightStrength"), 16, (GLfloat *)instance->m_lightStrength);
}

void Renderer::openLight(int index, bool enabled)
{
	this->m_useLight[index] = enabled;
	shared_ptr<GLShaderProgram> program = this->m_renderProcess->getPipeline();
	program->use();
	glUniform1iv(program->getUniformLocation("useLight"), 16, instance->m_useLight);
}

shared_ptr<Camera> Renderer::getCamera()
{
	return this->m_camera;
}

Renderer::Renderer() :
	m_frameBeginTime(0.0), m_camera(new Camera), m_useLight{}, m_lightPos{}, m_lightColor{}, m_lightStrength{}
{
}

Director::~Director()
{
}

void Director::useScene(const shared_ptr<Scene>& scene)
{
	if (!this->m_sceneStack.empty()) {
		this->m_sceneStack.top()->finished();
		this->m_sceneStack.pop();
	}
	this->m_sceneStack.push(scene);
	scene->init();
}

void Director::pushScene(const shared_ptr<Scene>& scene)
{
	if (!this->m_sceneStack.empty()) {
		this->m_sceneStack.top()->paused();
	}
	this->m_sceneStack.push(scene);
	scene->init();
}

void Director::popScene()
{
	if (this->m_sceneStack.empty()) return;
	this->m_sceneStack.top()->finished();
	this->m_sceneStack.pop();
	this->m_sceneStack.top()->resume();
}

void Director::update()
{
	double time = glfwGetTime();
	double timeDuring = time - this->m_lastTime;

	if (timeDuring >= this->m_updateDelta) {
		if (!this->m_sceneStack.empty()) {
			shared_ptr<Scene> scene = this->m_sceneStack.top();
			scene->update(timeDuring);

			Renderer* renderer = Renderer::getInstance();

			renderer->clearRenderables();
			for (shared_ptr<GameObject> gameObject = scene->m_rootObject; gameObject.get(); gameObject = gameObject->m_sibling) {
				if (gameObject->m_appear.get()) {
					renderer->addRenderable(gameObject->m_appear);
				}
				this->__addChildrenMeshToRenderer(gameObject);
			}

			*renderer->getCamera() = *scene->m_camera;

			for (int i = 0; i < 16; ++i) {
				renderer->openLight(i, false);
			}

			int lightIndex = 0;
			for (shared_ptr<Light> light : scene->m_lights) {
				if (lightIndex >= 16) break;
				vec3 lightPos = light->getWorldPosition();
				vec4 v4clipPos = scene->m_camera->getProjectionMatrix() * scene->m_camera->getViewMatrix() * vec4(lightPos, 1.0f);
				vec3 clipPos = vec3(v4clipPos.x / v4clipPos.w, v4clipPos.y / v4clipPos.w, v4clipPos.z / v4clipPos.w);
				if (clipPos.x <= 1.0f && clipPos.x >= -1.0f && clipPos.y <= 1.0f && clipPos.y >= -1.0f && clipPos.z <= 1.0f && clipPos.z >= 0.0f
					&& light->getStrenth() / length(lightPos - scene->m_camera->getPosition()) >= 0.1f) {
					vec3 color = light->getColor();
					renderer->openLight(lightIndex, true);
					renderer->setLightPos(lightIndex, lightPos.x, lightPos.y, lightPos.z);
					renderer->setLightColor(lightIndex, color.r, color.g, color.b);
					renderer->setLightStrength(lightIndex, light->getStrenth());
					++lightIndex;
				}
			}
		}
		
		this->m_lastTime = time;
	}
}

void Director::__addChildrenMeshToRenderer(const shared_ptr<GameObject>& parent)
{
	Renderer* renderer = Renderer::getInstance();
	for (shared_ptr<GameObject> child = parent->m_child; child.get(); child = child->m_sibling) {
		if (child->m_appear.get()) {
			renderer->addRenderable(child->m_appear);
		}
		this->__addChildrenMeshToRenderer(child);
	}
}

Director::Director() :
	m_updateDelta(1.0 / 60.0), m_lastTime(0.0)
{
}

Scene::Scene() :
	m_camera(new Camera)
{
	b2Vec2 gravity(0.0f, -9.8f);
	this->m_camera->setPosition(0, 0, 1);
	this->m_camera->focusOn(0, 0, 0);
	this->m_camera->perspective(45.0f * pi<float>() / 180.0f, static_cast<float>(Renderer::screenWidth) / Renderer::screenHeight, 0.1f, 100.1f);
	this->m_physicsWorld.reset(new b2World(gravity));
	this->m_physicsWorld->SetContactListener(this);
}

Scene::~Scene()
{
}

void Scene::init()
{
}

void Scene::resume()
{
}

void Scene::update(double delta)
{
	this->m_physicsWorld->Step(static_cast<float32>(delta), velocityIterations, positionIterations);
	for (shared_ptr<GameObject> ptr = this->m_rootObject; ptr.get(); ptr = ptr->m_sibling) {
		ptr->update(delta);
		this->__updateChild(ptr, delta);
	}
}

void Scene::paused()
{
}

void Scene::finished()
{
}

void Scene::addChild(const shared_ptr<GameObject>& child)
{
	shared_ptr<GameObject> secondChild = this->m_rootObject;
	this->m_rootObject = child;
	child->m_sibling = secondChild;
	child->m_parent = nullptr;
	child->m_belongScene = this;
	child->__setChildrenBelongScene(this);

	if (child->m_physicsDef2D.get()) {
		child->m_body2d = this->m_physicsWorld->CreateBody(&child->m_physicsDef2D->bodyDef);
		child->m_body2d->CreateFixture(&child->m_physicsDef2D->fixtureDef);
	}
	child->__createChildren2DPhysicsBody();
}

void Scene::removeChild(const shared_ptr<GameObject>& child)
{
	shared_ptr<GameObject> prev(nullptr);
	for (shared_ptr<GameObject> ptr = this->m_rootObject; ptr.get(); prev = ptr, ptr = ptr->m_sibling) {
		if (ptr.get() == child.get()) {
			if (ptr->m_body2d) {
				this->m_physicsWorld->DestroyBody(ptr->m_body2d);
				ptr->m_body2d = nullptr;
			}
			ptr->__removeChildren2DPhysicsBody();

			if (prev.get()) {
				prev->m_sibling = ptr->m_sibling;
			} else {
				this->m_rootObject = ptr->m_sibling;
			}
			ptr->m_parent = nullptr;
			ptr->m_belongScene = nullptr;
			ptr->__setChildrenBelongScene(nullptr);
			break;
		}
	}
}

shared_ptr<GameObject> Scene::getFirstChild()
{
	return this->m_rootObject;
}

void Scene::forEach(const function<void(GameObject*)>& func)
{
	for (shared_ptr<GameObject> ptr = this->m_rootObject; ptr.get(); ptr = ptr->m_sibling) {
		ptr->forEach(func);
		func(ptr.get());
	}
}

void Scene::addLight(const shared_ptr<Light>& light)
{
	this->m_lights.push_back(light);
}

void Scene::removeLight(const shared_ptr<Light>& light)
{
	for (auto iterator = this->m_lights.begin(); iterator != this->m_lights.end(); ++iterator) {
		if ((*iterator).get() == light.get()) {
			iterator = this->m_lights.erase(iterator);
			break;
		}
	}
}

void Scene::setAmbientLightColor(float r, float g, float b)
{
	Renderer::getInstance()->setAmbientLightColor(r, g, b);
}

shared_ptr<Camera> Scene::getCamera()
{
	return this->m_camera;
}

void Scene::__updateChild(const shared_ptr<GameObject>& parent, double delta)
{
	for (shared_ptr<GameObject> ptr = parent->m_child; ptr.get(); ptr = ptr->m_sibling) {
		ptr->update(delta);
		this->__updateChild(ptr, delta);
	}
}

GameObject::GameObject() :
	m_belongScene(nullptr), m_parent(nullptr), m_physicsDef2D(), m_body2d(nullptr), m_lastAngle(0.0f),
	m_localPosition(0.0f), m_localRotation(1.0f, 0.0f, 0.0f, 0.0f), m_localScale(1.0f)
{
}

GameObject::~GameObject()
{
}

void GameObject::addChild(const shared_ptr<GameObject>& child)
{
	shared_ptr<GameObject> secondChild = this->m_child;
	this->m_child = child;
	child->m_sibling = secondChild;
	child->m_parent = this;
	child->m_belongScene = this->m_belongScene;
	child->__setChildrenBelongScene(this->m_belongScene);

	if (this->m_belongScene) {
		if (child->m_physicsDef2D.get()) {
			b2World* world = child->m_belongScene->getPhysicsWorld();
			child->m_body2d = world->CreateBody(&child->m_physicsDef2D->bodyDef);
			child->m_body2d->CreateFixture(&child->m_physicsDef2D->fixtureDef);
		}
		child->__createChildren2DPhysicsBody();
	}
}

void GameObject::removeChild(const shared_ptr<GameObject>& child)
{
	shared_ptr<GameObject> prev(nullptr);
	for (shared_ptr<GameObject> ptr = this->m_child; ptr.get(); prev = ptr, ptr = ptr->m_sibling) {
		if (ptr.get() == child.get()) {
			if (ptr->m_body2d) {
				b2World* world = ptr->m_belongScene->getPhysicsWorld();
				world->DestroyBody(ptr->m_body2d);
				ptr->m_body2d = nullptr;
			}
			ptr->__removeChildren2DPhysicsBody();

			if (prev.get()) {
				prev->m_sibling = ptr->m_sibling;
			} else {
				this->m_child = ptr->m_sibling;
			}
			ptr->m_parent = nullptr;
			ptr->m_belongScene = nullptr;
			ptr->__setChildrenBelongScene(nullptr);
			break;
		}
	}
}

void GameObject::forEach(const function<void(GameObject*)>& func)
{
	for (shared_ptr<GameObject> ptr = this->m_child; ptr.get(); ptr = ptr->m_sibling) {
		ptr->forEach(func);
		func(ptr.get());
	}
}

void GameObject::set2DPhysics(const shared_ptr<PhysicsDef2D>& def)
{
	this->m_physicsDef2D = def;
	if (this->m_physicsDef2D.get()) {
		this->m_physicsDef2D->fixtureDef.shape = this->m_physicsDef2D->shapeDef.get();
	}
	if (this->m_belongScene) {
		b2World* world = this->m_belongScene->getPhysicsWorld();
		if (this->m_body2d) {
			world->DestroyBody(this->m_body2d);
			this->m_body2d = nullptr;
		}
		if (this->m_physicsDef2D.get()) {
			this->m_body2d = world->CreateBody(&this->m_physicsDef2D->bodyDef);
			this->m_body2d->CreateFixture(&this->m_physicsDef2D->fixtureDef);
		}
	}
}

b2Body* GameObject::get2DPhysicsBody()
{
	return this->m_body2d;
}

void GameObject::setAppear(const shared_ptr<IAppear>& appear)
{
	this->m_appear = appear;
}

shared_ptr<IAppear> GameObject::getAppear()
{
	return this->m_appear;
}

void GameObject::update(double delta)
{
	vec3 worldPos = this->getWorldPosition();
	quat worldRot = this->getWorldRotation();
	vec3 worldSca = this->getWorldScale();
	if (this->m_body2d) { // update physics data
		b2Vec2 xyPos = this->m_body2d->GetPosition();
		this->setWorldPosition(xyPos.x, xyPos.y, worldPos.z);
		worldPos = vec3(xyPos.x, xyPos.y, worldPos.z);
		float angle = this->m_body2d->GetAngle();
		float delta = angle - this->m_lastAngle;
		quat newRot = normalize(angleAxis(delta, vec3(0, 0, 1)) * worldRot);
		this->setWorldRotation(newRot);
		worldRot = newRot;
		this->m_lastAngle = angle;
	}
	if (this->m_appear.get()) { // update geometry data
		shared_ptr<GLVertexArray> vao = this->m_appear->geometry();
		vao->setPosition(worldPos);
		vao->setRotation(worldRot);
		vao->setScale(worldSca);
	}
}

void GameObject::translate(const vec3& dis)
{
	this->m_localPosition += dis;
}

void GameObject::translate(float x, float y, float z)
{
	this->translate(vec3(x, y, z));
}

void GameObject::rotate(float angle, const vec3& axis)
{
	this->m_localRotation = normalize(angleAxis(angle, axis) * this->m_localRotation);
}

void GameObject::rotate(float angle, float x, float y, float z)
{
	this->rotate(angle, vec3(x, y, z));
}

void GameObject::setLocalPosition(const vec3& pos)
{
	this->m_localPosition = pos;
}

void GameObject::setLocalPosition(float x, float y, float z)
{
	this->setLocalPosition(vec3(x, y, z));
}

vec3 GameObject::getLocalPosition()
{
	return this->m_localPosition;
}

void GameObject::setLocalRotation(const quat& rotation)
{
	this->m_localRotation = rotation;
}

quat GameObject::getLocalRotation()
{
	return this->m_localRotation;
}

void GameObject::setLocalScale(const vec3& scale)
{
	this->m_localScale = scale;
}

void GameObject::setLocalScale(float scale)
{
	this->setLocalScale(vec3(scale));
}

void GameObject::setLocalScale(float x, float y, float z)
{
	this->setLocalScale(vec3(x, y, z));
}

vec3 GameObject::getLocalScale()
{
	return this->m_localScale;
}

void GameObject::setWorldPosition(const vec3& pos)
{
	if (!this->m_parent) {
		this->m_localPosition = pos;
	} else {
		this->m_localPosition = pos - this->m_parent->getWorldPosition();
	}
}

void GameObject::setWorldPosition(float x, float y, float z)
{
	this->setWorldPosition(vec3(x, y, z));
}

vec3 GameObject::getWorldPosition()
{
	if (!this->m_parent) {
		return this->m_localPosition;
	} else {
		return this->m_localPosition + this->m_parent->getWorldPosition();
	}
}

void GameObject::setWorldRotation(const quat& rotation)
{
	if (!this->m_parent) {
		this->m_localRotation = rotation;
	} else {
		this->m_localRotation = normalize(inverse(this->m_parent->getWorldRotation()) * rotation);
	}
}

quat GameObject::getWorldRotation()
{
	if (!this->m_parent) {
		return this->m_localRotation;
	} else {
		return normalize(this->m_parent->getWorldRotation() * this->m_localRotation);
	}
}

void GameObject::setWorldScale(const vec3& scale)
{
	if (!this->m_parent) {
		this->m_localScale = scale;
	} else {
		this->m_localScale = scale / this->m_parent->getWorldScale();
	}
}

void GameObject::setWorldScale(float scale)
{
	this->setWorldScale(vec3(scale));
}

void GameObject::setWorldScale(float x, float y, float z)
{
	this->setWorldScale(vec3(x, y, z));
}

vec3 GameObject::getWorldScale()
{
	if (!this->m_parent) {
		return this->m_localScale;
	} else {
		return this->m_localScale * this->m_parent->getWorldScale();
	}
}

shared_ptr<GameObject> GameObject::getFirstChild()
{
	return this->m_child;
}

shared_ptr<GameObject> GameObject::getNextSibling()
{
	return this->m_sibling;
}

Scene* GameObject::getBelongScene()
{
	return this->m_belongScene;
}

void GameObject::__setChildrenBelongScene(Scene* scene)
{
	for (shared_ptr<GameObject> ptr = this->m_child; ptr.get(); ptr = ptr->m_sibling) {
		ptr->m_belongScene = scene;
		ptr->__setChildrenBelongScene(scene);
	}
}

void GameObject::__createChildren2DPhysicsBody()
{
	for (shared_ptr<GameObject> ptr = this->m_child; ptr.get(); ptr = ptr->m_sibling) {
		if (ptr->m_physicsDef2D.get()) {
			b2World* world = ptr->m_belongScene->getPhysicsWorld();
			ptr->m_body2d = world->CreateBody(&ptr->m_physicsDef2D->bodyDef);
			ptr->m_body2d->CreateFixture(&ptr->m_physicsDef2D->fixtureDef);
		}
		ptr->__createChildren2DPhysicsBody();
	}
}

void GameObject::__removeChildren2DPhysicsBody()
{
	for (shared_ptr<GameObject> ptr = this->m_child; ptr.get(); ptr = ptr->m_sibling) {
		if (ptr->m_body2d) {
			b2World* world = ptr->m_belongScene->getPhysicsWorld();
			world->DestroyBody(ptr->m_body2d);
			ptr->m_body2d = nullptr;
		}
		ptr->__removeChildren2DPhysicsBody();
	}
}

Keyboard::~Keyboard()
{
}

void Keyboard::init(GLFWwindow* window)
{
	if (!instance.get()) instance.reset(new Keyboard(window));
}

int Keyboard::getKeyState(int key)
{
	return glfwGetKey(this->m_window, key);
}

Keyboard::Keyboard(GLFWwindow* window) :
	m_window(window)
{
}

Light::Light() :
	m_belongScene(nullptr), m_position(0.0f), m_lightColor(1.0f, 1.0f, 1.0f), m_lightStrength(1.0f)
{
}

Light::~Light()
{
}

void Light::update(double delta)
{
}

void Light::translate(const vec3 & dis)
{
	this->m_position += dis;
}

void Light::translate(float x, float y, float z)
{
	this->translate(vec3(x, y, z));
}

void Light::setWorldPosition(const vec3 & pos)
{
	this->m_position = pos;
}

void Light::setWorldPosition(float x, float y, float z)
{
	this->setWorldPosition(vec3(x, y, z));
}

vec3 Light::getWorldPosition()
{
	return this->m_position;
}

void Light::setColor(glm::vec3 color)
{
	this->m_lightColor = color;
}

void Light::setColor(float r, float g, float b)
{
	this->setColor(vec3(r, g, b));
}

void Light::setStrength(float s)
{
	this->m_lightStrength = s;
}

vec3 Light::getColor()
{
	return this->m_lightColor;
}

float Light::getStrenth()
{
	return this->m_lightStrength;
}

Animation::~Animation()
{
}

void Animation::step(double delta)
{
	
}

Animation::Animation()
{
}
