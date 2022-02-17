// ------------------------------------------------------------------------------------------
// ����Ŀ�� ʵ����(instancing) ������ʵ��
//	
// instancing:
//				instancing ��һ�ִ�����Ⱦ����һ�������һ���Ż�������
//			  ����ƶ��㱾����ȣ����� glDrawArrays �� glDrawElements ����ȥ���� GPU ���ƶ��㣬�����ĸ�������ܡ���Ϊ OpenGL�ڻ��ƶ�������֮ǰ��Ҫ���ܶ�׼������
//			 ���������GPU�ô��ĸ������ȡ���ݣ�����Ѱ�Ҷ������ԣ�������Щ��������Ի�����CPU��GPU����(CPU to GPU Bus)�Ͻ��еģ������Լ�ʹ��Ⱦ����ǳ��죬���� 
//			  GPU ȥ��Ⱦȴδ�ء��� instancing ��������Խ�����һ���Է��͸� GPU ��Ȼ��ʹ�û��ƺ����� OpenGL ������Щ���ݻ��ƶ�����壬�����Ϳ��Խ�ʡ���ܶ����ܡ�
//	
//			  ʹ�� instancing ��Ⱦ��Ҫʹ���������ƺ���
//			  		glDrawArraysInstanced() 
//			  		glDrawElementsInstanced()
//			  ���������ƺ�������ͨ�Ļ��ƺ�����ȼ�����ȫһ����������Ҫһ������Ĳ���������ʵ��������ͨ�����������������Ҫ��Ⱦ��ʵ��������
//	
// ����ɫ������ƫ������
//			  instancing ���ƺ�������ûʲô�ã���Ϊÿ�����嶼��ͬ����λ��Ҳһ�������Ի���ͬһ��������ٴ�Ҳֻ�ܿ���һ�����塣
//			  �������ԭ��������Ҫ����ɫ������һ��ƫ������ʹÿ��ʵ���������嶼����һЩ�仯��ʹ�����ǿ����в�ͬ��λ�û��߱任��	
//			  �ϴ�ƫ������Ҫ�����ַ���.
//				
//				1. ʹ�� GLSL�ڽ����� gl_InstanceID �� Uniform ����:				( ����Ŀ CUBE_1 ��ʵ�� ) (���� Uniform �������Ŵ�С���ƣ���������������͵�ƫ�����������ܺܶ�)
//						GLSL�ڽ����� gl_InstanceID ��ʹ��ʵ������Ⱦ����ʱ��� 0 ��ʼ����ÿ��ʵ������Ⱦʱ����1����Ϊÿ��ʵ������Ψһ�� ID �����ǿ��Խ���һ�����飬
//						�� ID ��λ��ֵ��Ӧ��������ÿ��ʵ������������Ĳ�ͬλ�á�
//				2. ʹ�ö�������:												    ( ����Ŀ CUBE_2 ��ʵ�� )
//					    ʹ�ö�������ʱ��������ɫ����ÿ�����ж�����GLSL��ȡ��һ�������ڵ�ǰ��������ԡ��������ǽ��������Զ���Ϊһ��ʵ��������ʱ��������ɫ����ֻ��Ҫ��ÿ��ʵ����
//					    ������ÿ�����㣬���¶������Ե������ˡ����������Ƕ��𶥵������ʹ����ͨ�Ķ������ԣ�������ʵ��������ʹ��ʵ�������顣
//							ͨ�� glVertexAttribDivisor(@p1, @p2); ���ö�������
//							  ���� p1 ΪҪ���ö������Ե� location ֵ
//							  p2 Ϊ���ö������Ը���Ƶ��. p2 ֵΪ 0 ʱ��������ɫ��ÿ�ε���ʱ���������ݡ�������Ϊ 1 ʱ��ÿ��Ⱦһ��ʵ������һ�Σ�2 ʱÿ����ʵ������һ�Σ��Դ�����.
//
//
//	----------------------------------------------- ʵ�� -----------------------------------------------
//												  С���Ǵ�
//	������Ŀ PLANET_1  PLANET_2 ��ʵ�֣�
//  PLANET_1 �в�ʹ��ʵ������Ⱦ���ܿ�ʹﵽ������ƿ��
//  PLANET_2 ��ʹ��ʵ������Ⱦ

#define CUBE_2

// ʹ�� Uniform ���� �ϴ�ƫ��ֵ�� 100 ���ı���
#ifdef CUBE_1
#define INSTANCING
#endif // CUBE

// ʹ�� �������� layout(location = 2) �ϴ�ƫ��ֵ�� 100 ���ı���
#ifdef CUBE_2
#define INSTANCING
#define INSTANCING_2
#endif // CUBE_2

// ����ʵ������Ⱦ���Ǵ�
#ifdef PLANET_1
#define PLANET
#define PLANET_MODEL
#endif // 

// ʹ��ʵ������Ⱦ���Ǵ�
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
	// ���ı���
	// ---------------------------------------
#ifdef INSTANCING
	// ʹ�� �������� �ϴ�ƫ��ֵ
#ifdef INSTANCING_2
	Shader shader("instancing_2.vs", "instancing.fs");
	// ʹ�� uniform ���� �ϴ�ƫ��ֵ
#else  
	Shader shader("instancing_1.vs", "instancing.fs");
#endif // ���ı���
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
		// λ��          // ��ɫ
		-0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
		 0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
		-0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

		-0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
		 0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
		 0.05f,  0.05f,  0.0f, 1.0f, 1.0f
	};

	// ���� uniform �������
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
															
#ifdef INSTANCING_2							   // �˴�ʹ�ö�������
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
	srand(glfwGetTime()); // ��ʼ���������
	float radius = 150.0;	// �뾶
	float offset = 25.0f;	// ÿ��ʵ����ƫ��ֵ
	for (unsigned int i = 0; i < amount; i++)
	{
		glm::mat4 model(1.0f);
		// 1. λ��: �ֲ��ڰ뾶Ϊ 'radius' ��Բ���ϣ�ƫ�Ƶķ�Χ�� [-offset,offset]
		float angle = (float)i / (float)amount * 360.0f;
		float displacement;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float x = sin(angle) * radius + displacement;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float y = displacement * 0.4f; // �����Ǵ��ĸ߶ȱ� x �� z �Ŀ��ҪС
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float z = cos(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, y, z));

		// 2. ���ţ� �� 0.05 �� 0.25f ֮������
		float scale = (rand() % 20) / 100.0f + 1;
		model = glm::scale(model, glm::vec3(scale));

		// 3. ��ת: ����һ�� (��) ���ѡ�����ת�����������������ת
		float rotAngle = (rand() % 360);
		model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

		// 4. ����ھ����������
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
		// ��������
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
		instanceShader.setFloat("det",det);							// �ϴ���Ҫ�仯�ĽǶ�

		// draw meteorites
		// �����Ƕ�ÿ���������ʵ������Ⱦ
		// ����ֻ��Ⱦһ��С���ǣ����Ը�ѭ��ֻ����һ��

		for (unsigned int i = 0; i < rock.getMeshes().size(); i++)
		{
			glBindVertexArray(rock.getMeshes()[i].VAO);
			// �ܹ�ʵ������Ⱦ�� amount ��
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