#version 330 core

in vec4 position_world;
in vec4 normal;
in vec4 vertex_color;

// Posição do vértice atual no sistema de coordenadas local do modelo.
in vec4 position_model;

// Coordenadas de textura obtidas do arquivo OBJ (se existirem!)
in vec2 texcoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Identify the object to be rendered
#define COW 0
#define PLANE 1
#define MAZE 2
uniform int object_id;

// Identify the type of interpolation to be used
#define GOURAUD_INTERPOLATION 0
#define PHONG_INTERPOLATION 1
uniform int interpolation_type;

// Parâmetros da axis-aligned bounding box (AABB) do modelo
uniform vec4 b_box_min;
uniform vec4 b_box_max;

// Variável para acesso da imagem de textura
uniform sampler2D GoldTexture;

out vec4 color;

void main()
{
    color.a = 1.0;

    if (interpolation_type == GOURAUD_INTERPOLATION)
    {
        color.rgb = vertex_color.rgb;
    }
    else
    {
        vec4 origin = vec4(0.0,0.0,0.0,1.0);
        vec4 camera_position = inverse(view) * origin;
        vec4 p = position_world;
        vec4 n = normalize(normal);
        vec4 l = normalize(camera_position - p);
        vec4 v = l;
        vec4 half_vector = normalize(l + v);

        vec3 Kd; // Diffuse reflectance
        vec3 Ks; // Specular reflectance
        vec3 Ka; // Ambient reflectance
        float q; // Specular exponent for Blinn-Phong model

        vec3 I = vec3(1.0, 1.0, 1.0);  // Light intensity (white light)
        vec3 Ia = vec3(0.25, 0.15, 0.2); // Ambient light intensity

        // Coordenadas de textura U e V
        float U = 0.0;
        float V = 0.0;

        if ( object_id == COW )
        {
            float minx = b_box_min.x;
            float maxx = b_box_max.x;

            float miny = b_box_min.y;
            float maxy = b_box_max.y;

            float minz = b_box_min.z;
            float maxz = b_box_max.z;

            U = (position_model.x - minx) / (maxx - minx);
            V = (position_model.y - miny) / (maxy - miny);

            Kd = texture(GoldTexture, vec2(U,V)).rgb;
            //Kd = vec3(0.8,0.4,0.08);
            Ks = vec3(0.0,0.0,0.0);
            Ka = vec3(0.0, 0.0, 0.0);
            q = 32.0;
        }
        else if ( object_id == PLANE )
        {   
            Kd = vec3(0.1, 0.6, 0.3);
            Ks = vec3(0.0, 0.0, 0.0);
            Ka = vec3(0.01, 0.01, 0.01);
            q = 1.0;
        }
        else if ( object_id == MAZE )
        {
            Kd = vec3(0.35,0.35,0.35);
            Ks = vec3(0.01,0.01,0.01);
            Ka = vec3(0.1, 0.1, 0.1);
            q = 2.0;
        }
        else // Unknown object
        {
            Kd = vec3(0.0,0.0,0.0);
            Ks = vec3(0.0,0.0,0.0);
            Ka = vec3(0.0,0.0,0.0);
            q = 1.0;
        }
        vec3 lambert_diffuse_term = Kd * I * max(dot(n,l),0.0);
        vec3 ambient_term = Ka * Ia;
        vec3 blinn_phong_specular_term = Ks * I * pow(max(dot(n,half_vector),0.0),q);

        color.rgb = lambert_diffuse_term + ambient_term + blinn_phong_specular_term;

        // Gamma correction
        color.rgb = pow(color.rgb, vec3(1.0,1.0,1.0)/2.2);
    }
} 

