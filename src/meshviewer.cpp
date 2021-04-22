// Bryn Mawr College, alinen, 2020
// Shaili Regmi cs312 - Assignment 4
// NOTE: Refer to readme for details and notes

#include "AGL.h"
#include "AGLM.h"
#include <cmath>
#include <fstream>
#include <sstream>
#include <vector>
#include "mesh.h"
#include "osutils.h"
#include <glm/gtx/string_cast.hpp>

using namespace std;
using namespace glm;
using namespace agl;

// globals
Mesh theModel;
int theCurrentModel = 0;
vector<string> theModelNames;

mat4 transformation(1.0); // initialize to identity
mat4 projection = perspective(radians(30.0f), 1.0f, 0.1f, 10.0f);
mat4 camera;
mat4 mvp, mv;
mat3 nmv;

double xpos, ypos;
float change_x, change_y;

float Dist = 5.0f;
float Azimuth = 0.0f;
float Elevation = 0.0f;

// OpenGL IDs
GLuint theVboPosId;
GLuint theVboNormalId;
GLuint theElementbuffer;

static void LoadModel(int modelId)
{
   assert(modelId >= 0 && modelId < theModelNames.size());
   theModel.loadPLY(theModelNames[theCurrentModel]);

   glBindBuffer(GL_ARRAY_BUFFER, theVboPosId);
   glBufferData(GL_ARRAY_BUFFER, theModel.numVertices() * 3 * sizeof(float), theModel.positions(), GL_DYNAMIC_DRAW);

   glBindBuffer(GL_ARRAY_BUFFER, theVboNormalId);
   glBufferData(GL_ARRAY_BUFFER, theModel.numVertices() * 3 * sizeof(float), theModel.normals(), GL_DYNAMIC_DRAW);

   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, theElementbuffer);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, theModel.numTriangles() * 3 * sizeof(unsigned int), theModel.indices(), GL_DYNAMIC_DRAW);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
   if (action != GLFW_PRESS) return;

   if (key == GLFW_KEY_ESCAPE)
   {
      glfwSetWindowShouldClose(window, GLFW_TRUE);
   }
   else if (key == 'P')
   {
      if (--theCurrentModel < 0)
      {
         theCurrentModel = theModelNames.size() - 1;
      }
      cout << "Current file: " << theModelNames[theCurrentModel] << endl;
      LoadModel(theCurrentModel);
   }
   else if (key == 'N')
   {
      theCurrentModel = (theCurrentModel + 1) % theModelNames.size(); 
      cout << "Current file: " << theModelNames[theCurrentModel] << endl;
      LoadModel(theCurrentModel);
   }
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
   // Prevent a divide by zero
   if(height == 0) height = 1;
	
   // Set Viewport to window dimensions
   glViewport(0, 0, width, height);
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    float temp_Dist = 0.0f;
    if (state == GLFW_PRESS)
    {
        glfwGetCursorPos(window, &xpos, &ypos);

        int keyPress = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT); // Zooms in if you go towards the left and out if right.
        if (keyPress == GLFW_PRESS) {
            if (xpos >= 0 && ypos >= 0)
            {
                temp_Dist += 0.1 * abs(change_x);
            }
            else
            {
                temp_Dist -= 0.1 * abs(change_y);
            }
            
            if (temp_Dist == 0.0f) temp_Dist = 1.0f;
            if (temp_Dist < -10.0f) temp_Dist = -10.0f;
            if (temp_Dist > 10.0f) temp_Dist = 10.0f;
            Dist = temp_Dist;
        }
    }
    else if (state == GLFW_RELEASE)
    {
        
    }
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
   // TODO: Camera controls

    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    if (state != GLFW_PRESS) return;

    float x, y, z;
    double old_xpos = xpos;
    double old_ypos = ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    if (xpos > radians(180.0)) xpos = radians(180.0);
    if (xpos < radians(-180.0)) xpos = radians(-180.0);
    if (ypos > radians(90.0)) ypos = radians(90.0);
    if (ypos < radians(-90.0)) ypos = radians(-90.0);

    change_x = xpos - old_xpos;
    change_y = ypos - old_ypos;

    Azimuth = 0.01 * change_x;
    Elevation = 0.01 * change_y;

    x = Dist * sin(Azimuth) * cos(Elevation);
    y = Dist * sin(Elevation);
    z = Dist * cos(Azimuth) * cos(Elevation);
    
    // Scaling and Translating each model to fit the screen
    vec3 min_bound = theModel.getMinBounds();
    vec3 max_bound = theModel.getMaxBounds();
    vec3 max_dimension_vec = max_bound - min_bound;
    float max_dimension = max_dimension_vec.x;
    if (max_dimension_vec.y > max_dimension) max_dimension = max_dimension_vec.y;
    if (max_dimension_vec.z > max_dimension) max_dimension = max_dimension_vec.z;
    vec3 midpoint = 0.5f * (min_bound + max_bound);

    mat4 trans_matrix = translate(mat4(1.0f), -midpoint);
    mat4 scale_matrix = scale(mat4(1.0), vec3((1.0f / max_dimension), (1.0f / max_dimension), (1.0f / max_dimension)));
    transformation = scale_matrix * trans_matrix;
    
    camera = lookAt(vec3(x, y, z), vec3(0), vec3(0, 1, 0));

    mvp = projection * camera * transformation;
    mv = camera * transformation;
    nmv = mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2]));
}

static void PrintShaderErrors(GLuint id, const std::string label)
{
   std::cerr << label << " failed\n";
   GLint logLen;
   glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLen);
   if (logLen > 0)
   {
      char* log = (char*)malloc(logLen);
      GLsizei written;
      glGetShaderInfoLog(id, logLen, &written, log);
      std::cerr << "Shader log: " << log << std::endl;
      free(log);
   }
}

static std::string LoadShaderFromFile(const std::string& fileName)
{
   std::ifstream file(fileName);
   if (!file)
   {
      std::cout << "Cannot load file: " << fileName << std::endl;
      return "";
   }

   std::stringstream code;
   code << file.rdbuf();
   file.close();

   return code.str();
}

static void LoadModels(const std::string& dir)
{
   std::vector<std::string> filenames = GetFilenamesInDir(dir, "ply");
   for (int i = 0; i < filenames.size(); i++)
   {
      std::string filename = filenames[i];
      theModelNames.push_back(dir+filename);
   }
}

static GLuint LoadShader(const std::string& vertex, const std::string& fragment)
{
   GLint result;
   std::string vertexShader = LoadShaderFromFile(vertex);
   const char* vertexShaderRaw = vertexShader.c_str();
   GLuint vshaderId = glCreateShader(GL_VERTEX_SHADER);
   glShaderSource(vshaderId, 1, &vertexShaderRaw, NULL);
   glCompileShader(vshaderId);
   glGetShaderiv(vshaderId, GL_COMPILE_STATUS, &result);
   if (result == GL_FALSE)
   {
      PrintShaderErrors(vshaderId, "Vertex shader");
      return -1;
   }

   std::string fragmentShader = LoadShaderFromFile(fragment);
   const char* fragmentShaderRaw = fragmentShader.c_str();
   GLuint fshaderId = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(fshaderId, 1, &fragmentShaderRaw, NULL);
   glCompileShader(fshaderId);
   glGetShaderiv(fshaderId, GL_COMPILE_STATUS, &result);
   if (result == GL_FALSE)
   {
      PrintShaderErrors(fshaderId, "Fragment shader");
      return -1;
   }

   GLuint shaderId = glCreateProgram();
   glAttachShader(shaderId, vshaderId);
   glAttachShader(shaderId, fshaderId);
   glLinkProgram(shaderId);
   glGetShaderiv(shaderId, GL_LINK_STATUS, &result);
   if (result == GL_FALSE)
   {
      PrintShaderErrors(shaderId, "Shader link");
      return -1;
   }
   return shaderId;
}


int main(int argc, char** argv)
{
   GLFWwindow* window;

   if (!glfwInit())
   {
      return -1;
   }

   // Explicitly ask for a 4.0 context 
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

   /* Create a windowed mode window and its OpenGL context */
   window = glfwCreateWindow(500, 500, "Mesh Viewer", NULL, NULL);
   if (!window)
   {
      glfwTerminate();
      return -1;
   }

   // Make the window's context current 
   glfwMakeContextCurrent(window);

   glfwSetKeyCallback(window, key_callback);
   glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
   glfwSetMouseButtonCallback(window, mouse_button_callback);
   glfwSetScrollCallback(window, scroll_callback);
   glfwSetCursorPosCallback(window, cursor_position_callback);

#ifndef APPLE
   if (glewInit() != GLEW_OK)
   {
      return -1;
   }
#endif

   glEnable(GL_DEPTH_TEST);
   glEnable(GL_CULL_FACE);
   glClearColor(0, 0, 0, 1);

   glGenBuffers(1, &theVboPosId);
   glGenBuffers(1, &theVboNormalId);
   glGenBuffers(1, &theElementbuffer);

   GLuint vaoId;
   glGenVertexArrays(1, &vaoId);
   glBindVertexArray(vaoId);

   glEnableVertexAttribArray(0); // 0 -> Sending VertexPositions to array #0 in the active shader
   glBindBuffer(GL_ARRAY_BUFFER, theVboPosId); // always bind before setting data
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);

   glEnableVertexAttribArray(1); // 1 -> Sending Normals to array #1 in the active shader
   glBindBuffer(GL_ARRAY_BUFFER, theVboNormalId); // always bind before setting data
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);

   LoadModels("../models/");
   LoadModel(0);

   //GLuint shaderId = LoadShader("../shaders/phong.vs", "../shaders/phong.fs");
   GLuint shaderId = LoadShader("../shaders/phong.vs", "../shaders/phong.fs");
   glUseProgram(shaderId);

   GLuint mvpId = glGetUniformLocation(shaderId, "uMVP");
   GLuint mvId = glGetUniformLocation(shaderId, "uMV");
   GLuint nmvId = glGetUniformLocation(shaderId, "uNMV");

   glUniform3f(glGetUniformLocation(shaderId, "uMaterial.Ks"), 1.0, 1.0, 1.0); 
   glUniform3f(glGetUniformLocation(shaderId, "uMaterial.Kd"), 0.4, 0.6, 1.0); 
   glUniform3f(glGetUniformLocation(shaderId, "uMaterial.Ka"), 0.1, 0.1, 0.1); 
   glUniform1f(glGetUniformLocation(shaderId, "uMaterial.shininess"), 80.0); 
   glUniform3f(glGetUniformLocation(shaderId, "uLight.position"), 100.0, 100.0, 100.0); 
   glUniform3f(glGetUniformLocation(shaderId, "uLight.color"), 1.0, 1.0, 1.0);

   float x, y, z;
   x = Dist * sin(Azimuth) * cos(Elevation);
   y = Dist * sin(Elevation);
   z = Dist * cos(Azimuth) * cos(Elevation);

   // Scaling and Translating each model to fit the screen
   vec3 min_bound = theModel.getMinBounds();
   vec3 max_bound = theModel.getMaxBounds();
   vec3 max_dimension_vec = max_bound - min_bound;
   float max_dimension = max_dimension_vec.x;
   if (max_dimension_vec.y > max_dimension) max_dimension = max_dimension_vec.y;
   if (max_dimension_vec.z > max_dimension) max_dimension = max_dimension_vec.z;
   vec3 midpoint = 0.5f * (min_bound + max_bound);

   mat4 trans_matrix = translate(mat4(1.0f), -midpoint);
   mat4 scale_matrix = scale(mat4(1.0), vec3((1.0f / max_dimension), (1.0f / max_dimension), (1.0f / max_dimension)));
   transformation = scale_matrix * trans_matrix;
   
   camera = lookAt(vec3(x, y, z), vec3(0), vec3(0, 1, 0));
   
   mvp = projection * camera * transformation;
   mv = camera * transformation;
   nmv = mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2]));

   // Loop until the user closes the window 
   while (!glfwWindowShouldClose(window))
   {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the buffers

      // Update transform
      glUniformMatrix3fv(nmvId, 1, GL_FALSE, &nmv[0][0]);
      glUniformMatrix4fv(mvId, 1, GL_FALSE, &mv[0][0]);
      glUniformMatrix4fv(mvpId, 1, GL_FALSE, &mvp[0][0]);

      // Draw primitive
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, theElementbuffer);
      glDrawElements(GL_TRIANGLES, theModel.numTriangles() * 3, GL_UNSIGNED_INT, (void*)0);

      // Swap front and back buffers
      glfwSwapBuffers(window);

      // Poll for and process events
      glfwPollEvents();
   }

   glfwTerminate();
   return 0;
}


