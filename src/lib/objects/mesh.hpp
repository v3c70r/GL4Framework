#pragma once
#include <objects/object.h>
#include <vector>
#include <glm/glm.hpp>

// A base mesh class contains no rendering info

namespace GL_CORE
{
class Mesh: Object
{
protectd:
    // Mesh information on the host side
    std::vector<size_t> vertices_;
    std::vector<glm::vec3> normals_;
    std::vector<glm::vec3> positions_;
    std::vector<glm::vec2> textureCoords_;
};
}
