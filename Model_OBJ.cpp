#include "Model_OBJ.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <glew.h>

struct Vertex {
	float x, y, z;
	float nx, ny, nz;
	float s, t;
};

bool  Model_OBJ::loadModelOBJ(const std::string& filename, std::vector<Vertex>& vertices, std::vector<GLuint>& indices) {
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cerr << "Failed to open file " << filename << std::endl;
		return false;
	}

	std::vector<float> positions;
	std::vector<float> normals;
	std::vector<float> texcoords;
	std::vector<GLuint> positionIndices;
	std::vector<GLuint> normalIndices;
	std::vector<GLuint> texcoordIndices;

	std::string line;
	while (std::getline(file, line)) {
		std::istringstream linestream(line);
		std::string keyword;
		linestream >> keyword;
		if (keyword == "v") {
			// Vertex position
			float x, y, z;
			linestream >> x >> y >> z;
			positions.push_back(x);
			positions.push_back(y);
			positions.push_back(z);
		}
		else if (keyword == "vt") {
			// Vertex texcoord
			float s, t;
			linestream >> s >> t;
			texcoords.push_back(s);
			texcoords.push_back(t);
		}
		else if (keyword == "vn") {
			// Vertex normal
			float x, y, z;
			linestream >> x >> y >> z;
			normals.push_back(x);
			normals.push_back(y);
			normals.push_back(z);
		}
		else if (keyword == "f") {
			// Face
			for (int i = 0; i < 3; i++) {
				std::string indexString;
				linestream >> indexString;
				std::istringstream indexStream(indexString);
				GLuint positionIndex, texcoordIndex, normalIndex;
				char slash;
				indexStream >> positionIndex >> slash >> texcoordIndex >> slash >> normalIndex;
				positionIndices.push_back(positionIndex - 1);
				texcoordIndices.push_back(texcoordIndex - 1);
				normalIndices.push_back(normalIndex - 1);
			}
		}
	}

	for (int i = 0; i < positionIndices.size(); i++) {
		Vertex vertex;
		vertex.x = positions[positionIndices[i] * 3];
		vertex.y = positions[positionIndices[i] * 3 + 1];
		vertex.z = positions[positionIndices[i] * 3 + 2];
		vertex.s = texcoords[texcoordIndices[i] * 2];
		vertex.t = texcoords[texcoordIndices[i] * 2 + 1];
		vertex.nx = normals[normalIndices[i] * 3];
		vertex.ny = normals[normalIndices[i] * 3 + 1];
		vertex.nz = normals[normalIndices[i] * 3 + 2];
		vertices.push_back(vertex);
		indices.push_back(i);
	}

	return true;


}
