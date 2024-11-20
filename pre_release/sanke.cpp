// Copyright (c) 2024 Tritium_cyanide
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
//my first little project on opengl .awa



#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <SDL.h>
#include <SDL_opengl.h>
#pragma comment(lib, "SDL2main.lib") 
#pragma comment(lib, "SDL2.lib") 
#define SDL_MAIN_HANDLED

#include <cstdlib>
#include <cstdio>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <random>
#include <vector>
#include <mutex>
#include <time.h>
#include <string>

using namespace std;

//

enum Direction {
    UP,
    DOWN,
    NORTH,
    SOUTH,
    EAST,
    WEST,
};

struct Point1 {
    int x;
    int y;
    int z;
    Direction PD;

    bool operator==(const Point1& other) const {
        return x == other.x && y == other.y && z == other.z;
    }

    bool operator!=(const Point1& other) const {
        return !(*this == other);
    }
};

bool quit;
std::mutex quitMutex;

float campos[3];
glm::mat4 view;
glm::mat4 projection;

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

std::chrono::time_point<std::chrono::system_clock> *timePtr;
int* movespeedp;

SDL_Window* window;
SDL_Event e;

Direction headDirection;
Direction headDirectionl;

Point1 pos;
Point1 fruitpo;

float yaw = 0.0f;
float pitch = 0.0f;
float cameraDistance = 1.0f;

std::vector<Point1> bodypo;
Point1 bodypos;
glm::vec3 lookat;

GLfloat vertices[][3] = {
    {-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, -0.5f}, {-0.5f, 0.5f, -0.5f},
    {-0.5f, -0.5f, 0.5f}, {0.5f, -0.5f, 0.5f}, {0.5f, 0.5f, 0.5f}, {-0.5f, 0.5f, 0.5f}
};

GLuint edges[] = {
    0,1, 1,2, 2,3, 3,0,
    4,5, 5,6, 6,7, 7,4,
    0,4, 1,5, 2,6, 3,7
};

GLfloat vertices1[][3] = {
    {-0.025f, -0.025f, -0.025f},
    {0.025f, -0.025f, -0.025f},
    {0.025f, 0.025f, -0.025f},
    {-0.025f, 0.025f, -0.025f},
    {-0.025f, -0.025f, 0.025f},
    {0.025f, -0.025f, 0.025f},
    {0.025f, 0.025f, 0.025f},
    {-0.025f, 0.025f, 0.025f}
};

GLuint edges1[] = {
    0, 1, 2, 2, 3, 0, 4, 5, 6,
    6, 7, 4, 4, 0, 3, 3, 7, 4,
    1, 5, 6, 6, 2, 1, 2, 6, 7,
    7, 3, 2, 0, 4, 5, 5, 1, 0
};

//

struct EventLoopData {
    int lastX, lastY;
    float yaw, pitch;
    float cameraDistance;
};

EventLoopData eventdata;

void eventLoopFunction(EventLoopData *data) {

    window = SDL_CreateWindow("3D snake game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!window) {
        //std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
    }

    SDL_GLContext glContext1 = SDL_GL_CreateContext(window);
    if (!glContext1) {
        //std::cerr << "SDL_GL_CreateContext Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        SDL_GL_DeleteContext(glContext1);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    while (!quit) {
        std::this_thread::sleep_for(std::chrono::milliseconds(8));
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quitMutex.lock();
                quit = true;
                quitMutex.unlock();
            }
            else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                case SDLK_w:
                    if (headDirectionl != SOUTH) headDirection = NORTH;
                    break;
                case SDLK_a:
                    if (headDirectionl != EAST) headDirection = WEST;
                    break;
                case SDLK_s:
                    if (headDirectionl != NORTH) headDirection = SOUTH;
                    break;
                case SDLK_d:
                    if (headDirectionl != WEST) headDirection = EAST;
                    break;
                case SDLK_LSHIFT:
                    if (headDirectionl != UP) headDirection = DOWN;
                    break;
                case SDLK_SPACE:
                    if (headDirectionl != DOWN) headDirection = UP;
                    break;
                case SDLK_ESCAPE:
                    quitMutex.lock();
                    quit = true;
                    quitMutex.unlock();
                }
            }

            else if (e.type == SDL_MOUSEMOTION) {
                int mouseX = e.motion.x;
                int mouseY = e.motion.y;
                int deltaX = mouseX - data->lastX;
                int deltaY = mouseY - data->lastY;
                data->lastX = mouseX;
                data->lastY = mouseY;
                data->yaw += deltaX * 0.01f;
                data->pitch -= deltaY * 0.01f;

                if (data->pitch > 89.8f) data->pitch = 89.8f;
                if (data->pitch < -89.8f) data->pitch = -89.8f;

            }

            else if (e.type == SDL_MOUSEWHEEL) {
                data->cameraDistance -= e.wheel.y * 0.04f;
            }
        }
    }
    SDL_GL_DeleteContext(glContext1);
}

class render
{
    unsigned int compileShader(const char* shaderCode, GLenum type) {
        GLint success;
        unsigned int shader;
        shader = glCreateShader(type);
        glShaderSource(shader, 1, &shaderCode, NULL);
        glCompileShader(shader);
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            //GLchar infoLog[512];
            //glGetShaderInfoLog(shader, 512, NULL, infoLog);
            //std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
        return shader;
    }

    unsigned int createShader(const char* vertexShaderCode, const char* fragmentShaderCode) {
        unsigned int program;
        unsigned int vertex, fragment;

        vertex = compileShader(vertexShaderCode, GL_VERTEX_SHADER);
        fragment = compileShader(fragmentShaderCode, GL_FRAGMENT_SHADER);

        program = glCreateProgram();
        glAttachShader(program, vertex);
        glAttachShader(program, fragment);
        glLinkProgram(program);

        glDeleteShader(vertex);
        glDeleteShader(fragment);

        return program;
    }

public:
    static int n;
    unsigned int shaderProgram;
    unsigned int vao, vbo, ebo;
    int modellocation, viewlocation, projectionlocation, colorslocation;
    std::string shaderSource;
    render(){
        n++;
    }
    void elementset(bool isline){
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        if (isline) {
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(edges), edges, GL_STATIC_DRAW);
            glVertexAttribPointer(n, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(n);

            
        }
        else {
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(edges1), edges1, GL_STATIC_DRAW);
            glVertexAttribPointer(n, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(n);
            
            
    
        }
        /*
            shaderSource =
                "#version 330 core\n"
                "layout (location = " + std::to_string(n) + ") in vec3 aPos; \n"
                "out vec3 colors\n"
                "uniform mat4 model;\n"
                "uniform mat4 view;\n"
                "uniform mat4 projection;\n"
                "void main() {\n"
                "    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
                "int instanceIndex = gl_InstanceID;\n"
                "switch(instanceIndex){\n"
                "case 1: colors = vec3(0.0,1.0,0.0)\n"
                "break;\n"
                "case 2: colors = vec3(1.0,0.0,0.0)\n"
                "break;\n"
                "case 3: colors = vec3(1.0,0.0,0.0)\n"
                "break;\n"
                "default: colors = vec3(1.0,1.0,1.0)\n"
                "break;\n"
                "}\n";
            const char* vertexShaderSource = shaderSource.c_str();
            const char* fragmentShaderSource = R"(
        #version 330 core
        in vec3 colors;
        out vec4 FragColor;
        void main() {
            FragColor = vec4(colors, 1); 
        }
    )";
            shaderProgram = createShader(vertexShaderSource, fragmentShaderSource);
        */
            shaderSource =
                "#version 330 core\n"
                "layout (location = " + std::to_string(n) + ") in vec3 aPos; \n"
                "uniform mat4 model;\n"
                "uniform mat4 view;\n"
                "uniform mat4 projection;\n"
                "void main() {\n"
                "    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
                "}\n";
            const char* vertexShaderSource = shaderSource.c_str();
            const char* fragmentShaderSource = R"(
        #version 330 core
        uniform vec3 colors;
        out vec4 FragColor;
        void main() {
            FragColor = vec4(colors, 1); 
        }
    )";
            shaderProgram = createShader(vertexShaderSource, fragmentShaderSource);
        
        

        modellocation = glGetUniformLocation(shaderProgram, "model");
        viewlocation = glGetUniformLocation(shaderProgram, "view");
        projectionlocation = glGetUniformLocation(shaderProgram, "projection");
        colorslocation = glGetUniformLocation(shaderProgram, "colors");
    }
    void elementdelete() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
        glDeleteProgram(shaderProgram);
    }
    
};
render cube, fruit, body;
int render::n = 0;

//
void author(SDL_Window* window) {

    SDL_Surface* image1 = SDL_LoadBMP("text3d.bmp");

    if (image1 == nullptr) {
        //SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load %s", "text3d.bmp");
    }
    else {
        SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        SDL_Texture* texture1 = SDL_CreateTextureFromSurface(renderer, image1);
        if (texture1 == nullptr) {
            //SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture from image1");
            SDL_FreeSurface(image1);
        }

        SDL_FreeSurface(image1);
        SDL_Rect rect = { 160, 180, 0, 0 };
        SDL_QueryTexture(texture1, NULL, NULL, &rect.w, &rect.h);
        SDL_RenderClear(renderer);

        Uint32 startTime = SDL_GetTicks();
        Uint32 delayTime = 100;

        while (!quit) {
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) {
                    quitMutex.lock();
                    quit = true;
                    quitMutex.unlock();
                }
            }
            if (SDL_GetTicks() - startTime > delayTime) {
                SDL_RenderClear(renderer);
                break;
            }
            SDL_RenderCopy(renderer, texture1, NULL, &rect);
            SDL_RenderPresent(renderer);
        }
        SDL_DestroyTexture(texture1);
        SDL_DestroyRenderer(renderer);
    }
}


void setupCube(unsigned int shaderProgram, GLint modellocation, GLint viewlocation, GLint projectionlocation, GLint colorslocation) {
    

    glUseProgram(shaderProgram);

    glm::mat4 model = glm::mat4(1.0f);

    GLfloat colors[3] = { 1.0f,1.0f,1.0f };
    glUniform3fv(colorslocation, 1, colors);
    glUniformMatrix4fv(modellocation, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewlocation, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionlocation, 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(1);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);

    
}


void drawbody(unsigned int shaderProgram, GLint modellocation, GLint viewlocation, GLint projectionlocation, GLint colorslocation) {

    glUseProgram(shaderProgram);

    glm::mat4 model = glm::mat4(1.0f);
 
    glm::mat4 translation;
    glm::mat4 rotation;

    GLfloat colors[3];

    float step;
    
    for (GLuint n = 0; n < bodypo.size(); ++n) {
        
        if (n == bodypo.size() - 1) {
            step = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - *timePtr).count()) / static_cast<float>(*movespeedp);

            if (headDirectionl == EAST) {
                rotation = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f ), glm::vec3(0.0f, 1.0f, 0.0f));
                lookat = glm::vec3(bodypo[bodypo.size() - 1].x * 0.05f + step*0.05f, bodypo[bodypo.size() - 1].y * 0.05f, bodypo[bodypo.size() - 1].z * 0.05f);
            }
            else if (headDirectionl == WEST) {
                rotation = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                lookat = glm::vec3(bodypo[bodypo.size() - 1].x * 0.05f - step*0.05f, bodypo[bodypo.size() - 1].y * 0.05f, bodypo[bodypo.size() - 1].z * 0.05f);
            }
            else if (headDirectionl == NORTH) {
                rotation = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f ), glm::vec3(0.0f, 1.0f, 0.0f));
                lookat = glm::vec3(bodypo[bodypo.size() - 1].x * 0.05f, bodypo[bodypo.size() - 1].y * 0.05f , bodypo[bodypo.size() - 1].z * 0.05f - step * 0.05f);
            }
            else if (headDirectionl == SOUTH) {
                rotation = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f ), glm::vec3(0.0f, 1.0f, 0.0f));
                lookat = glm::vec3(bodypo[bodypo.size() - 1].x * 0.05f, bodypo[bodypo.size() - 1].y * 0.05f , bodypo[bodypo.size() - 1].z * 0.05f + step * 0.05f);
            }
            else if (headDirectionl == UP) {
                rotation = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f ), glm::vec3(0.0f, 0.0f, 1.0f));
                lookat = glm::vec3(bodypo[bodypo.size() - 1].x * 0.05f, bodypo[bodypo.size() - 1].y * 0.05f + step * 0.05f, bodypo[bodypo.size() - 1].z * 0.05f );
            }
            else if (headDirectionl == DOWN) {
                lookat = glm::vec3(bodypo[bodypo.size() - 1].x * 0.05f, bodypo[bodypo.size() - 1].y * 0.05f - step * 0.05f, bodypo[bodypo.size() - 1].z * 0.05f);
                rotation = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f ), glm::vec3(0.0f, 0.0f, 1.0f));
            }
            model = glm::mat4(1.0);
            glm::vec3 scale_factor(step, 1.0f, 1.0f);
            model = glm::scale(model, scale_factor);

            translation = glm::translate(glm::mat4(1.0f), glm::vec3((-step+1) * 0.05f, 0.0f , 0.0f));
            model = translation * model;
            translation = glm::translate(glm::mat4(1.0f), glm::vec3(bodypo[n].x * 0.05f , bodypo[n].y * 0.05f, bodypo[n].z * 0.05f));
            model = rotation * model;
            model = translation * model;
        }
        else if ( n == 0) {
            if (bodypo[1].PD == EAST) {
                rotation = glm::rotate(glm::mat4(1.0f), glm::radians(-180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            }
            else if (bodypo[1].PD == WEST) {
                rotation = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            }
            else if (bodypo[1].PD == NORTH) {
                rotation = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            }
            else if (bodypo[1].PD == SOUTH) {
                rotation = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            }
            else if (bodypo[1].PD == UP) {
                rotation = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            }
            else if (bodypo[1].PD == DOWN) {
                rotation = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            }
            step = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - *timePtr).count()) / static_cast<float>(*movespeedp);
            model = glm::mat4(1.0);
            glm::vec3 scale_factor(1-step, 1.0f, 1.0f);
            model = glm::scale(model, scale_factor);

            translation = glm::translate(glm::mat4(1.0f), glm::vec3((-step) * 0.05f, 0.0f, 0.0f));
            model = translation * model;
            translation = glm::translate(glm::mat4(1.0f), glm::vec3(bodypo[n].x * 0.05f, bodypo[n].y * 0.05f, bodypo[n].z * 0.05f));
            model = rotation * model;
            model = translation * model;

        }
        else {
            model = glm::mat4(1.0f);
            glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(bodypo[n].x * 0.05f, bodypo[n].y * 0.05f, bodypo[n].z * 0.05f));
            model = translation * model;
        }
        if (n < 12) {
            colors[0] = static_cast < float>(0.8f - 0.06 * n);
            colors[1] = 1.0f;
            colors[2] = 0.9f;
        }
        else {
            colors[0] = 0.2f;
            colors[1] = 0.9f;
            colors[2] = 1.0f;
        }
        glUniform3fv(colorslocation, 1, colors);
        glUniformMatrix4fv(modellocation, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewlocation, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionlocation, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(3);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }
}

void drawfruit(unsigned int shaderProgram, GLint modellocation, GLint viewlocation, GLint projectionlocation, GLint colorslocation){

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(shaderProgram);

    glm::mat4 model = glm::mat4(1.0f);

    glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(fruitpo.x * 0.05f, fruitpo.y * 0.05f, fruitpo.z * 0.05f));
    model = translation * model;
    glm::vec3 scale_factor(0.8f, 0.8f, 0.8f);
    model = glm::scale(model, scale_factor);
    GLfloat colors[3] = { 1.0f,0.0f,0.0f };

    glUniform3fv(colorslocation, 1, colors);
    glUniformMatrix4fv(modellocation, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewlocation, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionlocation, 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(2);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

}

void setfruit() {

    bool sp = false;
    std::random_device rd;  
    std::mt19937 gen(rd()); 
    int min = -10; 
    int max = 10; 
    std::uniform_int_distribution<> distrib(min, max); 
    fruitpo = { distrib(gen) ,distrib(gen)  ,distrib(gen)};
    do{
        for (GLuint n = 0;n <= bodypo.size() - 1; n++) {
            if (fruitpo == bodypo[n]){
                fruitpo = { distrib(gen) ,distrib(gen)  ,distrib(gen) };
                sp = true;
                n = -1;
            }
        }
        sp = false;
    } while (sp);
}

void posmove(int movespeed){
    headDirection = headDirectionl = EAST;
    Point1 pos = { 0, 0, 0 };
    SDL_Delay(50);
    auto start = std::chrono::system_clock::now();
    std::chrono::time_point<std::chrono::system_clock> lastmove;

    bodypo = { {-2,0,0,EAST},{-1,0,0,EAST},{0,0,0,EAST} };

    timePtr = &lastmove;

        while (!quit) {
            auto now = std::chrono::system_clock::now();

            if (now - start > std::chrono::milliseconds(movespeed)) {

                if (headDirection == NORTH) {
                    pos.z += -1;
                }
                else if (headDirection == SOUTH) {
                    pos.z -= -1;
                }
                else if (headDirection == EAST) {
                    pos.x += 1;
                }
                else if (headDirection == WEST) {
                    pos.x -= 1;
                }
                else if (headDirection == UP) {
                    pos.y -= -1;
                }
                else if (headDirection == DOWN) {
                    pos.y += -1;
                }

                lastmove = now;
                bodypos = {pos.x,pos.y,pos.z,headDirection};
                headDirectionl = headDirection;

                if (bodypo[bodypo.size()-1] != fruitpo) {
                    bodypo.erase(bodypo.begin());
                }
                else {
                    setfruit();
                }
                if (bodypos.x > 10 || bodypos.y > 10 || bodypos.z > 10 || bodypos.x < -10 || bodypos.y < -10 || bodypos.z < -10) {
                    quitMutex.lock();
                    quit = true;
                    quitMutex.unlock();
                }
                for (GLuint n = 0; n < bodypo.size() - 1; ++n) {
                    if (bodypos == bodypo[n]) {
                        quitMutex.lock();
                        quit = true;
                        quitMutex.unlock();
                    }
                }
                bodypo.push_back(bodypos);
                start = now;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

void rendering() {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    //author(window);

    SDL_GLContext glContext1 = SDL_GL_CreateContext(window);
    if (!glContext1) {
        //std::cerr << "SDL_GL_CreateContext 1Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        SDL_GL_DeleteContext(glContext1);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    eventdata.lastX = WINDOW_WIDTH / 2;
    eventdata.lastY = WINDOW_WIDTH / 2;
    eventdata.yaw = 0.0f;
    eventdata.pitch = 0.0f;
    eventdata.cameraDistance = 0.4f;

    render cube, fruit, body;
    cube.elementset(true);
    fruit.elementset(false);
    body.elementset(false);

    projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

    while (!quit) {
        std::this_thread::sleep_for(std::chrono::milliseconds(4));

        campos[0] = 5.0f * sin(eventdata.yaw) * cos(eventdata.pitch) * eventdata.cameraDistance;
        campos[1] = 5.0f * sin(eventdata.pitch) * eventdata.cameraDistance;
        campos[2] = 5.0f * cos(eventdata.yaw) * cos(eventdata.pitch) * eventdata.cameraDistance;
        view = glm::lookAt(glm::vec3(campos[0], campos[1], campos[2]), lookat, glm::vec3(0.0f, 1.0f, 0.0f));


        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        setupCube(cube.shaderProgram,cube.modellocation, cube.viewlocation, cube.projectionlocation, cube.colorslocation);

        drawbody(body.shaderProgram, body.modellocation, body.viewlocation, body.projectionlocation, body.colorslocation);

        drawfruit(fruit.shaderProgram, fruit.modellocation, fruit.viewlocation, fruit.projectionlocation, fruit.colorslocation);

        glEnable(GL_DEPTH_TEST);

        SDL_GL_SwapWindow(window);
    }
    SDL_GL_DeleteContext(glContext1);
    cube.elementdelete();
    fruit.elementdelete();
    body.elementdelete();
}

//







int main(int argc, char* args[]){
    // 初始化 SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        //std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    int movespeed = 600;
    movespeedp = &movespeed;
    fruitpo = { 6, 0, 0 };
    campos[0] = 0.0f;
    campos[1] = 0.0f;
    campos[2] = 0.64f;
    quit = false;
    
    std::thread t1(eventLoopFunction, &eventdata);
    std::thread t2(posmove, movespeed);
    std::thread t3(rendering);
    
    t1.join();
    t2.join();
    t3.join();

    SDL_DestroyWindow(window);
    return 0;
}