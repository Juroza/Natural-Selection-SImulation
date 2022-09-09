#version 330 core
out vec4 color;

in vec3 FragPos;
in vec3 Normal;

struct Material{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
struct Light {
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;
uniform vec3 viewPos;

uniform Material material;
void main()
{
    // Ambient
    vec3 ambient = material.ambient * light.ambient;
    
    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = (diff*material.diffuse) * light.diffuse;
    
    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = (material.specular) * spec * light.specular;
    
    vec3 result = (ambient + diffuse + specular);
    color = vec4(result, 1.0f);
}
