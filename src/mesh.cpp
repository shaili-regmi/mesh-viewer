
#include "mesh.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include <cmath>
#include <iostream>

using namespace std;
using namespace glm;
using namespace agl;

Mesh::Mesh() 
{
    vertices = 0;
    faces = 0;
    min_bound = glm::vec3(0);
    max_bound = glm::vec3(0);
}

Mesh::~Mesh()
{
    delete[] vertex_array;
    delete[] normal_array; 
    delete[] index_array;
}

bool Mesh::loadPLY(const std::string& filename)
{
    ifstream file(filename);

    if (!file)
    {
        return false;
    }

    string current_string;
    file >> current_string;
    if (current_string.compare("ply") != 0)
    {
        return false;
    }

    float x, y, z, nx, ny, nz;
    unsigned int i0, i1, i2;
    float min_x = 0;
    float min_y = 0;
    float min_z = 0;
    float max_x = 0;
    float max_y = 0;
    float max_z = 0;
    int properties = 0;

    while (current_string.compare("end_header") != 0)
    {
        file >> current_string;
        if (current_string.compare("property") == 0)
        {
            file >> current_string;
            if (current_string.compare("float") == 0)
            {
                properties++;
            }
        }
        if (current_string.compare("element") == 0)
        {
            file >> current_string;
            if (current_string.compare("vertex") == 0)
            {
                file >> current_string;
                vertices = stoi(current_string, nullptr, 10);
            }
            if (current_string.compare("face") == 0)
            {
                file >> current_string;
                faces = stoi(current_string, nullptr, 10);
            }
        }
    }

    vertex_array = new float[vertices * 3];
    normal_array = new float[vertices * 3];
    index_array = new unsigned int[faces * 3];

    float current_value;

    for (int i = 0; i < vertices; i++)
    {
        vector <float> properties_list;
        
        for (int j = 0; j < properties; j++)
        {
            file >> current_value;
            properties_list.push_back(current_value);
        }
        x = properties_list[0];
        y = properties_list[1];
        z = properties_list[2];
        nx = properties_list[3];
        ny = properties_list[4];
        nz = properties_list[5];

        properties_list.clear();

        if (i == 0)
        {
            min_x = x;
            min_y = y;
            min_z = z;
            max_x = x;
            max_y = y;
            max_z = z;
        }

        if (x < min_x) min_x = x;
        if (y < min_y) min_y = y;
        if (z < min_z) min_z = z;
        if (x > max_x) max_x = x;
        if (y > max_y) max_y = y;
        if (z > max_z) max_z = z;

        vertex_array[i*3+0] = x;
        vertex_array[i*3+1] = y; 
        vertex_array[i*3+2] = z;
        normal_array[i*3+0] = nx;
        normal_array[i*3+1] = ny;
        normal_array[i*3+2] = nz;
    }

    min_bound = glm::vec3(min_x, min_y, min_z);
    max_bound = glm::vec3(max_x, max_y, max_z);

    for (int i = 0; i < faces; i++)
    {
        file >> current_string;
        if (current_string.compare("3") == 0)
        {
            file >> i0 >> i1 >> i2;
            index_array[i*3+0] = i0;
            index_array[i*3+1] = i1;
            index_array[i*3+2] = i2;
        }
    }

    file.close();
    return true;
}

glm::vec3 Mesh::getMinBounds() const
{
  return min_bound;
}

glm::vec3 Mesh::getMaxBounds() const
{
  return max_bound;
}

int Mesh::numVertices() const
{
    return vertices;
}

int Mesh::numTriangles() const
{
   return faces;
}

float* Mesh::positions() const
{
   return vertex_array;
}

float* Mesh::normals() const
{
   return normal_array;
}

unsigned int* Mesh::indices() const
{
   return index_array;
}

