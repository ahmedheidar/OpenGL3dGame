#include <string>
#include <vector>
#include <stdio.h>

struct Vertex {
	float x, y, z;
	float nx, ny, nz;
	float s, t;
};

class Model_OBJ {
public:
	bool loadModelOBJ(const std::string& filename, std::vector<Vertex>& vertices, std::vector<GLuint>& indices);
};
