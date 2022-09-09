#version 330 core
out vec4 color;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
struct Material{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};
struct Light {
    vec3 position;
    vec3 direction;
    float innerCutOff;
    float outerCutOff;
    float intensityVal;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

uniform Light light;
uniform vec3 viewPos;
uniform vec4 ourColour;

uniform Material material;
void main()
{
    
    // Diffuse
    vec3 lightDir = normalize(light.position-FragPos);
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon   = (light.innerCutOff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    vec3 norm = normalize(Normal);
    // Ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
        
    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * light.specular*vec3(texture(material.specular, TexCoords));
    diffuse*=intensity*light.intensityVal;
    specular*=intensity*light.intensityVal;
    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    vec3 result = (ambient + diffuse + specular);
    color = vec4(result, 1.0f)* ourColour;
    
        
}
    




