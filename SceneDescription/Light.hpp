#include <glm/glm.hpp>
#include <variant>

enum class light_type
{
    point,
    spot,
    directional,
};

struct PointLight
{
    float constant;   // 衰减常数
    float linear;     // 线性衰减因子
    float quadratic;  // 平方衰减因子
};

struct SpotLight
{
    float cut_off;        // 内圆锥角（用于计算光滑边缘）
    float outer_cut_off;  // 外圆锥角（用于计算光滑边缘）
    glm::vec3 direction;
};

struct DirectionalLight
{
    glm::vec3 direction;
};

struct Light
{
    light_type type;
    float intensity;
    glm::vec3 color;
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;  
    std::variant<std::monostate, PointLight, SpotLight, DirectionalLight> detail;
};