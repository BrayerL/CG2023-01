#include "Object.h"

#include "Object.h"

void Object::initialize(
	string filePath,
	Shader* shader,
	glm::vec3 position,
	glm::vec3 scale,
	float angle,
	glm::vec3 axis)
{
	this->shader = shader;
	this->position = position;
	this->originalPosition = position;
	this->scale = scale;
	this->angle = angle;
	this->axis = axis;

	loadObj(filePath);
}

void Object::update()
{
	glm::mat4 model = glm::mat4(1); //matriz identidade
	model = glm::translate(model, position);
	model = glm::rotate(model, glm::radians(angle), axis);
	model = glm::scale(model, scale);
	shader->setMat4("model", glm::value_ptr(model));
}

void Object::rotateX(float angle) {
	this->angle = angle;
	this->axis = glm::vec3(1.0f, 0.0f, 0.0f);
	update();
}

void Object::rotateY(float angle) {
	this->angle = angle;
	this->axis = glm::vec3(0.0f, 1.0f, 0.0f);
	update();
}

void Object::rotateZ(float angle) {
	this->angle = angle;
	this->axis = glm::vec3(0.0f, 0.0f, 1.0f);
	update();
}

void Object::translate(glm::vec3 position)
{
	this->position = position;
	update();
}

void Object::increaseScale() {
	scale.x += 0.05;
	scale.y += 0.05;
	scale.z += 0.05;
	update();
}

void Object::decreaseScale() {
	scale.x -= 0.05;
	scale.y -= 0.05;
	scale.z -= 0.05;
	update();
}

void Object::moveUp()
{
	this->position.y = this->position.y + movementSpeed;
	update();
}

void Object::moveDown()
{
	this->position.y = this->position.y - movementSpeed;
	update();
}

void Object::moveLeft()
{
	this->position.x = this->position.x - movementSpeed;
	update();
}

void Object::moveRight()
{
	this->position.x = this->position.x + movementSpeed;
	update();
}

void Object::moveFront()
{
	this->position.z = this->position.z + movementSpeed;
	update();
}

void Object::moveBack()
{
	this->position.z = this->position.z - movementSpeed;
	update();
}

void Object::reset() {
	this->position = originalPosition;
	this->scale = glm::vec3(1);
	this->angle = 0.0;
	update();
}

void Object::draw()
{
	for (int i = 0; i < meshes.size(); i++)
	{
		meshes[i].draw();
	}
}

void Object::deleteVAO() {
	glDeleteVertexArrays(1, &VAO);
}

void Object::loadObj(string filePath)
{
		string texNomes[] = { "../models/Pokemon/textures/PikachuMouthDh.png",
							 "../models/Pokemon/textures/PikachuDh.png",
							 "../models/Pokemon/textures/PikachuHohoDh.png",
							 "../models/Pokemon/textures/PikachuEyeDh.png",
							 "../models/Pokemon/textures/PikachuDh.png" };

	int i = 0;

	glm::vec3 color;
	color.r = 1.0;  color.g = 0.0;  color.b = 0.0;


	vector <glm::vec3> vertices;
	vector <glm::vec3> colors;
	vector <GLuint> indices;
	vector <glm::vec2> texCoords;
	vector <glm::vec3> normals;
	vector <GLfloat> vbuffer;

	ifstream inputFile;
	inputFile.open(filePath.c_str());
	if (inputFile.is_open())
	{
		char line[100];
		string sline;

		bool inicioGrupo = true;

		while (!inputFile.eof())
		{
			inputFile.getline(line, 100);
			sline = line;

			string word;

			istringstream ssline(line);
			ssline >> word;

			//cout << word << " ";
			if (word == "v" || inputFile.eof())
			{
				if (inicioGrupo)
				{
					if (vbuffer.size())
					{
						inicioGrupo = false;

						Mesh grupo;
						GLuint texID = loadTexture(texNomes[i]);
						i++;
						int nVerts;
						GLuint VAO = generateVAO(vbuffer, nVerts);
						grupo.initialize(VAO, nVerts, shader, texID);

						meshes.push_back(grupo);

						//Limpar o array auxiliar do buffer de geometria
						vbuffer.clear();
					}
				}

				glm::vec3 v;
				ssline >> v.x >> v.y >> v.z;
				vertices.push_back(v);
				colors.push_back(color);
			}
			if (word == "vt")
			{
				glm::vec2 vt;

				ssline >> vt.s >> vt.t;

				texCoords.push_back(vt);
			}
			if (word == "vn")
			{
				glm::vec3 vn;

				ssline >> vn.x >> vn.y >> vn.z;

				normals.push_back(vn);
			}

			if (word == "g")
			{
				inicioGrupo = true;
			}

			if (word == "f")
			{
				string tokens[3];

				ssline >> tokens[0] >> tokens[1] >> tokens[2];

				for (int i = 0; i < 3; i++)
				{
					//Recuperando os indices de v
					int pos = tokens[i].find("/");
					string token = tokens[i].substr(0, pos);
					int index = atoi(token.c_str()) - 1;
					indices.push_back(index);

					vbuffer.push_back(vertices[index].x);
					vbuffer.push_back(vertices[index].y);
					vbuffer.push_back(vertices[index].z);
					vbuffer.push_back(colors[index].r);
					vbuffer.push_back(colors[index].g);
					vbuffer.push_back(colors[index].b);

					//Recuperando os indices de vts
					tokens[i] = tokens[i].substr(pos + 1);
					pos = tokens[i].find("/");
					token = tokens[i].substr(0, pos);
					index = atoi(token.c_str()) - 1;

					vbuffer.push_back(texCoords[index].s);
					vbuffer.push_back(texCoords[index].t);

					//Recuperando os indices de vns
					tokens[i] = tokens[i].substr(pos + 1);
					index = atoi(tokens[i].c_str()) - 1;

					vbuffer.push_back(normals[index].x);
					vbuffer.push_back(normals[index].y);
					vbuffer.push_back(normals[index].z);
				}
			}

		}

	}
	else
	{
		cout << "Problema ao encontrar o arquivo " << filePath << endl;
	}
	inputFile.close();

}

//void Object::loadObj(string filePath)
//{
//	string texNomes[] = { "../models/Pokemon/textures/PikachuMouthDh.png",
//						 "../models/Pokemon/textures/PikachuDh.png",
//						 "../models/Pokemon/textures/PikachuHohoDh.png",
//						 "../models/Pokemon/textures/PikachuEyeDh.png",
//						 "../models/Pokemon/textures/PikachuDh.png" };
//
//	/*vector <string> texNomes;
//
//	texNomes.push_back("../models/Pokemon/textures/PikachuMouthDh.png");
//	texNomes.push_back("../models/Pokemon/textures/PikachuDh.png");
//	texNomes.push_back("../models/Pokemon/textures/PikachuHohoDh.png");
//	texNomes.push_back("../models/Pokemon/textures/PikachuEyeDh.png");
//	texNomes.push_back("../models/Pokemon/textures/PikachuDh.png");*/
//
//	int i = 0;
//
//	glm::vec3 color;
//	color.r = 1.0;  
//	color.g = 0.0;  
//	color.b = 0.0;
//
//	vector <glm::vec3> vertices;
//	vector <glm::vec3> colors;
//	vector <GLuint> indices;
//	vector <glm::vec2> texCoords;
//	vector <glm::vec3> normals;
//	vector <GLfloat> vbuffer;
//
//	ifstream inputFile;
//	inputFile.open(filePath.c_str());
//
//	if (inputFile.is_open())
//	{
//		char line[100];
//		string sline;
//
//		bool inicioGrupo = true;
//
//		while (!inputFile.eof())
//		{
//			inputFile.getline(line, 100);
//			sline = line;
//
//			string word;
//
//			istringstream ssline(sline);
//			ssline >> word;
//
//			if (word == "v" || inputFile.eof())
//			{
//				if (inicioGrupo)
//				{
//					if (vbuffer.size())
//					{
//						inicioGrupo = false;
//
//						Mesh mesh;
//
//						//if (!texNomes.empty()) {
//							GLuint texID = loadTexture(texNomes[i]);
//							i++;
//						//}
//
//						int nVerts;
//						GLuint VAO = generateVAO(vbuffer, nVerts);
//						mesh.initialize(VAO, nVerts, shader, texID);
//
//						meshes.push_back(mesh);
//
//						//Limpar o array auxiliar do buffer de geometria
//						vbuffer.clear();
//					}
//				}
//
//				glm::vec3 v;
//				ssline >> v.x >> v.y >> v.z;
//				vertices.push_back(v);
//				colors.push_back(color);
//			}
//			if (word == "vt")
//			{
//				glm::vec2 vt;
//				ssline >> vt.s >> vt.t;
//				texCoords.push_back(vt);
//			}
//			if (word == "vn")
//			{
//				glm::vec3 vn;
//				ssline >> vn.x >> vn.y >> vn.z;
//				normals.push_back(vn);
//			}
//			if (word == "g")
//			{
//				inicioGrupo = true;
//			}
//			if (word == "f")
//			{
//				string tokens[3];
//
//				ssline >> tokens[0] >> tokens[1] >> tokens[2];
//
//				for (int i = 0; i < 3; i++)
//				{
//					int pos = tokens[i].find("/");
//					string token = tokens[i].substr(0, pos);
//					int index = atoi(token.c_str()) - 1;
//					indices.push_back(index);
//
//					vbuffer.push_back(vertices[index].x);
//					vbuffer.push_back(vertices[index].y);
//					vbuffer.push_back(vertices[index].z);
//					vbuffer.push_back(colors[index].r);
//					vbuffer.push_back(colors[index].g);
//					vbuffer.push_back(colors[index].b);
//
//					//Recuperando os indices de vts
//					tokens[i] = tokens[i].substr(pos + 1);
//					pos = tokens[i].find("/");
//
//					if (pos == 0) {
//						tokens[i] = tokens[i].substr(pos + 1);
//					}
//
//					pos = tokens[i].find("/");
//					token = tokens[i].substr(0, pos);
//					int indexT = atoi(token.c_str()) - 1;
//
//					if (indexT < 0) {
//						indexT = 0;
//					}
//
//					if (texCoords.size() == 0) {
//						vbuffer.push_back(0);
//						vbuffer.push_back(0);
//					}
//					else {
//						vbuffer.push_back(texCoords[indexT].s);
//						vbuffer.push_back(texCoords[indexT].t);
//					}
//
//					tokens[i] = tokens[i].substr(pos + 1);
//					token = tokens[i].substr(0, pos);
//					index = atoi(token.c_str()) - 1;
//
//					if (index < 0) {
//						index = 0;
//					}
//
//					vbuffer.push_back(normals[index].x);
//					vbuffer.push_back(normals[index].y);
//					vbuffer.push_back(normals[index].z);
//
//				}
//			}
//		}
//	}
//	else
//	{
//		cout << "N�o foi possivel abrir o arquivo " << filePath << endl;
//	}
//	inputFile.close();
//}

GLuint Object::generateVAO(vector<GLfloat> vbuffer, int& nVerts)
{
	GLuint VBO, VAO;

	nVerts = vbuffer.size() / 11;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vbuffer.size() * sizeof(GLfloat), vbuffer.data(), GL_STATIC_DRAW);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Atributo cor (r, g, b)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	//Atributo coordenada de textura (s, t)
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	//Atributo normal do v�rtice (x, y, z)
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	return VAO;
}

GLuint Object::loadTexture(string filePath)
{
	GLuint texID;

	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);

	if (data)
	{
		if (nrChannels == 3) //jpg, bmp
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE,
				data);
		}
		else //png
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
				data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);

	return texID;
}