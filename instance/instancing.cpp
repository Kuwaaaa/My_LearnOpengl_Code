// ------------------------------------------------------------------------------------------
// 本项目对 实例化(instancing) 进行了实验
//	
// instancing:
//				instancing 是一种大量渲染单独一个网格的一种优化技术。
//			  与绘制顶点本身相比，调用 glDrawArrays 或 glDrawElements 函数去告诉 GPU 绘制顶点，会消耗更多的性能。因为 OpenGL在绘制顶点数据之前需要做很多准备工作
//			 （比如告诉GPU该从哪个缓冲读取数据，从哪寻找顶点属性，而且这些都是在相对缓慢的CPU到GPU总线(CPU to GPU Bus)上进行的）。所以即使渲染顶点非常快，命令 
//			  GPU 去渲染却未必。而 instancing 这项技术可以将数据一次性发送给 GPU ，然后使用绘制函数让 OpenGL 利用这些数据绘制多个物体，这样就可以节省出很多性能。
//	
//			  使用 instancing 渲染主要使用两个绘制函数
//			  		glDrawArraysInstanced() 
//			  		glDrawElementsInstanced()
//			  这两个绘制函数与普通的绘制函数相比几乎完全一样，但是需要一个额外的参数，叫做实例数量。通过这个参数来设置需要渲染的实例个数。
//	
// 向着色器传递偏移量：
//			  instancing 绘制函数本身没什么用，因为每个物体都相同，且位置也一样，所以绘制同一个物体多少次也只能看到一个物体。
//			  出于这个原因，我们需要向着色器传递一套偏移量来使每次实例化的物体都做出一些变化，使得他们可以有不同的位置或者变换。	
//			  上传偏移量主要有两种方法.
//				
//				1. 使用 GLSL内建变量 gl_InstanceID 和 Uniform 数组:				( 本项目 CUBE_1 中实现 ) (由于 Uniform 数组有着大小限制，所以这个方案传送的偏移量数量不能很多)
//						GLSL内建变量 gl_InstanceID 在使用实例化渲染调用时会从 0 开始，在每个实例被渲染时递增1。因为每个实例都有唯一的 ID ，我们可以建立一个数组，
//						将 ID 与位置值对应起来，将每个实例放置在世界的不同位置。
//				2. 使用顶点属性:												    ( 本项目 CUBE_2 中实现 )
//					    使用顶点属性时，顶点着色器的每次运行都会让GLSL获取新一组适用于当前顶点的属性。而当我们将顶点属性定义为一个实例化数组时，顶点着色器就只需要对每个实例，
//					    而不是每个顶点，更新顶点属性的内容了。这允许我们对逐顶点的数据使用普通的顶点属性，而对逐实例的数据使用实例化数组。
//							通过 glVertexAttribDivisor(@p1, @p2); 设置顶点属性
//							  其中 p1 为要启用顶点属性的 location 值
//							  p2 为设置顶点属性更新频率. p2 值为 0 时，顶点着色器每次迭代时都更新数据。当设置为 1 时，每渲染一个实例更新一次，2 时每两个实例更新一次，以此类推.
//
//
//	----------------------------------------------- 实例 -----------------------------------------------
//												  小行星带
//	（本项目 PLANET_1  PLANET_2 中实现）
//  PLANET_1 中不使用实例化渲染，很快就达到了性能瓶颈
//  PLANET_2 中使用实例化渲染

#define CUBE_2

// 使用 Uniform 数组 上传偏移值的 100 个四边形
#ifdef CUBE_1
#define INSTANCING
#endif // CUBE

// 使用 顶点属性 layout(location = 2) 上传偏移值的 100 个四边形
#ifdef CUBE_2
#define INSTANCING
#define INSTANCING_2
#endif // CUBE_2

// 不用实例化渲染行星带
#ifdef PLANET_1
#define PLANET
#define PLANET_MODEL
#endif // 

// 使用实例化渲染行星带
#ifdef PLANET_2
#define PLANET
#define PLANET_INSTANCING
#endif


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "common.h"
#include "shader.h"
#include "Camera.h"
#include "Model.h"

#include <iostream>

GLOBALVALUE

int main()
{
	GLFWwindow* window = glfw_gladInit("Geometry");

	// build and compile shaders
	// -------------------------
	// 画四边形
	// ---------------------------------------
#ifdef INSTANCING
	// 使用 顶点属性 上传偏移值
#ifdef INSTANCING_2
	Shader shader("instancing_2.vs", "instancing.fs");
	// 使用 uniform 数组 上传偏移值
#else  
	Shader shader("instancing_1.vs", "instancing.fs");
#endif // 画四边形
#endif // INSTANCING
	// ---------------------------------------

#ifdef PLANET
	Shader shader("planet_1.vs", "planet.fs");
#endif // PLANET_MODEL
	
#ifdef PLANET_INSTANCING
	Shader instanceShader("planet_2.vs", "planet.fs");
#endif

	glEnable(GL_DEPTH_TEST);
	
#ifdef INSTANCING
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float quadVertices[] = {
		// 位置          // 颜色
		-0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
		 0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
		-0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

		-0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
		 0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
		 0.05f,  0.05f,  0.0f, 1.0f, 1.0f
	};

	// 创建 uniform 缓冲对象
	glm::vec2 translations[100];
	int index = 0;
	float offset = 0.1f;
	for (int y = -10; y < 10; y += 2)
	{
		for (int x = -10; x < 10; x += 2)
		{
			glm::vec2 translation;
			translation.x = (float)x / 10.0f + offset;
			translation.y = (float)y / 10.0f + offset;
			translations[index++] = translation;
		}
	}

	// instanceVBO
	unsigned instanceVBO;
	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 100, &translations[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// quadVAO
	unsigned quadVAO, quadVBO;
	//makeVO(quadVertices, sizeof(quadVertices), { 2, 3 }, quadVAO, quadVBO);
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) ( 2 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER,instanceVBO); // this attribute comes from a different vertex buffer
															
#ifdef INSTANCING_2							   // 此处使用顶点属性
											   // ----------------
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribDivisor(2, 1);			   // tell OpenGL this is an instanced vertex attribute.
#endif // INSTANCING_2

	
	

	shader.use();
	for (unsigned int i = 0; i < 100; i++)
	{
		std::stringstream ss;
		std::string index;
		ss << i;
		index = ss.str();
		shader.setVec2(("offsets[" + index + "]").c_str(), translations[i]);
	}

#endif // INSTANCING

#ifdef PLANET
	//Model planet("E:/resource/Blender_OBJ/pot/pot.obj");
	Model planet("D:/VsProjects/Advanceopengl/mesh/planet.obj");
	Model rock("D:/VsProjects/Advanceopengl/mesh/rock.obj");

	unsigned int amount = 5000;
	glm::mat4* modelMatrices;
	modelMatrices = new glm::mat4[amount];
	srand(glfwGetTime()); // 初始化随机种子
	float radius = 150.0;	// 半径
	float offset = 25.0f;	// 每个实例的偏移值
	for (unsigned int i = 0; i < amount; i++)
	{
		glm::mat4 model(1.0f);
		// 1. 位移: 分布在半径为 'radius' 的圆形上，偏移的范围是 [-offset,offset]
		float angle = (float)i / (float)amount * 360.0f;
		float displacement;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float x = sin(angle) * radius + displacement;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float y = displacement * 0.4f; // 让行星带的高度比 x 和 z 的宽度要小
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float z = cos(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, y, z));

		// 2. 缩放： 在 0.05 和 0.25f 之间缩放
		float scale = (rand() % 20) / 100.0f + 1;
		model = glm::scale(model, glm::vec3(scale));

		// 3. 旋转: 绕着一个 (半) 随机选择的旋转轴向量进行随机的旋转
		float rotAngle = (rand() % 360);
		model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

		// 4. 添加在矩阵的数组中
		modelMatrices[i] = model;
	}
#endif // PLANET_MODEL


#ifdef PLANET_INSTANCING
	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

	for (unsigned int i = 0; i < rock.getMeshes().size(); i++)
	{
		unsigned int VAO = rock.getMeshes()[i].VAO;
		glBindVertexArray(VAO);
		// 顶点属性
		GLsizei vec4Size = sizeof(glm::vec4);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(vec4Size));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2*vec4Size));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3*vec4Size));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
	}
#endif
	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;


		// input
		// -----
		processInput(window);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#ifdef INSTANCING
// --------------
		shader.use();
		glBindVertexArray(quadVAO);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);
#endif // INSTANCING

#ifdef PLANET
		float rotateSpeed = 5.0f;
		float det = glm::radians(glfwGetTime()) * rotateSpeed;

		// configure transformation matrices
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100000.0f);
		glm::mat4 view = camera.GetViewMatrix();;
		shader.use();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		shader.setVec3("carame_position", camera.Front);

		shader.use();
		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 3.0f, 0.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		model = glm::rotate(model, det, glm::vec3(1.0f, 3.0f, 0.0f));
		shader.setMat4("model", model);
		planet.Draw(shader);
#endif

#ifdef PLANET_MODEL
		shader.use()

		// draw meteorites
		for(unsigned int i = 0; i < amount; i++)
		{
			shader.setMat4("model", modelMatrices[i]);
			rock.Draw(shader);
		}
#endif
		
#ifdef PLANET_INSTANCING

		std::cout << det << endl;
		instanceShader.use();
		instanceShader.setMat4("projection", projection);
		instanceShader.setMat4("view", view);
		instanceShader.setFloat("det",det);							// 上传需要变化的角度

		// draw meteorites
		// 这里是对每个网格进行实例化渲染
		// 由于只渲染一个小行星，所以该循环只迭代一次

		for (unsigned int i = 0; i < rock.getMeshes().size(); i++)
		{
			glBindVertexArray(rock.getMeshes()[i].VAO);
			// 总共实例化渲染了 amount 个
			glDrawElementsInstanced(GL_TRIANGLES, rock.getMeshes()[i].indices.size(), GL_UNSIGNED_INT, 0, amount);
		}
#endif

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
#ifdef INSTANCING
	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadVBO);
#endif // INSTANCING

	glfwTerminate();
	return 0;
}