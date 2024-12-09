//#define TESTING_RPN
#ifdef TESTING_RPN
#include <iostream>
#include "ShuntingYard.h"

int main() {
    std::cout << "Hello World!";
    std::cout << ReversePolishNotation::evaluate(ShuntingYard::convertToRPN("exp(x^2)+exp(y^2)"), 1.01, 1) << std::endl;
    std::vector<float> points;
    // ----- BEGIN TO EVALUATE RPNS ----- //
    points.clear();
    points.reserve(1 * 3);
    auto expression = ShuntingYard::convertToRPN("exp(x^2)+exp(y^2)");
    for (double x = 0.0; x < 100.0; x += 0.1) {
        for (double y = 0.0; y < 100.0; y += 0.1) {
            points.push_back(x);
            points.push_back(y);
            points.push_back(ReversePolishNotation::evaluate(expression, x, y));
        }
    }
    std::cout << points.size();
    // ----- END EVALUATING RPNS ----- //
}

#else
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <regex> // For replacing x and y with a number
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ShuntingYard.h"
#include <Windows.h>

constexpr int INITIAL_WINDOW_WIDTH = 800, INITIAL_WINDOW_HEIGHT = 800;

const char* vertexShaderSource = R"(
#version 440 core
layout(location = 0) in vec3 aPos;

uniform mat4 model, view, proj;

void main() {
    gl_Position = proj * view * model * vec4(aPos, 1.0f);
}
)";

const char* fragmentShaderSource = R"(
#version 440 core
out vec4 FragColor;
void main() {
    FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f); // red color
}
)";

/*
std::vector<GLfloat> points = {
    -0.8f, 0.5f, 0.0f,  // Point 1
    -0.6f, 0.7f, 0.0f,  // Point 2
    -0.4f, 0.9f, 0.0f,  // Point 3
     0.0f, 0.0f, 0.0f,  // Point 4 (Origin)
     0.4f, -0.7f, 0.0f, // Point 5
     0.6f, -0.5f, 0.0f  // Point 6
};
*/

// Define vertices for a line
constexpr GLfloat vertices[] = {
    -1.0f, 0.0f, 0,  // Bottom-left
     1.0f,  0.0f, 0   // Top-right
};
//std::string replaceXWith(std::string expression, )

int windowWidth = INITIAL_WINDOW_HEIGHT, windowHeight = INITIAL_WINDOW_HEIGHT;

constexpr glm::vec3 UP{ 0.0f, 1.0f, 0.0f };

// https://stackoverflow.com/questions/27927913/find-index-of-first-match-using-c-regex
std::string parseExpressionFromCommandLine(std::string s) {
    std::smatch m; // <-- need a match object

    long i = 0;

    while (regex_search(s, m, std::regex{"\\s+"})) { // <-- use it here to get the match
        if (i == 2) {
            return s;
        }
        s = s.substr(m.position() + m.length()); // <-- also, skip the whole match
        i++;
    }

    if (!s.empty()) {
        if (i == 2) {
            return s;
        }
    }

    std::cerr << "Failed to parse command line. Ask me in real life about it." << std::endl;
    exit(EXIT_FAILURE);
}

int main(int argc, const char* argv[]) {
    if (argc <= 2) {
        std::cout << "Usage: \"" << argv[0] << "\" <scale> <expression>" << std::endl;
        return EXIT_SUCCESS;
    }
    const double scale = std::stod(argv[1]);
    /*
    // ----- BEGIN TO EVALUATE RPNS ----- //
    std::vector<GLfloat> points;
    points.reserve(rpns.size() * 3);
    for (std::vector<ReversePolishNotation::Token*> expression : rpns) {
        for (double x = 0.0; x < 100.0; x += increment) {
            std::cout << "x: " << x;
            for (double y = 0.0; y < 100.0; y += increment) {
                std::cout << "x: " << x;
                std::cout << "y: " << y;
                points.push_back(x);
                points.push_back(y);
                points.push_back(ReversePolishNotation::evaluate(expression, x, y));
            }
        }
    }
    // ----- END EVALUATING RPNS ----- //
    */
    std::vector<GLfloat> points;
    std::string expressionString{parseExpressionFromCommandLine(GetCommandLineA())};
    //expressionString = "x"; //"3.1415*(x^2)*y";
    std::cout << "parsed expression (shouldn't have quotes): " << expressionString << "\nCOMPUTING POINT POSITIONS...";
    auto expression = ShuntingYard::convertToRPN(expressionString);
    
    for (double x = -100.0; x < 100.0; x += 0.1) {
        for (double y = -100.0; y < 100.0; y += 0.1) {
            double realX = x / scale, realY = y / scale;
            points.push_back(x);
            points.push_back(y);
            points.push_back(ReversePolishNotation::evaluate(expression, realX, realY) * scale);
        }
    }
    std::cout << " DONE!" << std::endl;

    if (!glfwInit()) {
        std::cerr << "Failed to init GLFW" << std::endl;
        return EXIT_FAILURE;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, "ISU1 (3D Graphing Calculator)", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create window" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window);
    gladLoadGL();
    glViewport(0, 0, INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT);
    glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, windowWidth = width, windowHeight = height);
    });
















    // Create and compile the vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    // Check if the vertex shader compiled successfully
    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "ERROR: Vertex shader compilation failed\n" << infoLog << std::endl;
        return EXIT_FAILURE;
    }

    // Create and compile the fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    // Check if the fragment shader compiled successfully
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "ERROR: Fragment shader compilation failed\n" << infoLog << std::endl;
        return EXIT_FAILURE;
    }

    // Create the shader program and link shaders
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check if the program linked successfully
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "ERROR: Shader program linking failed\n" << infoLog << std::endl;
        return EXIT_FAILURE;
    }

    // Clean up shaders since they're already linked into the program
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Create a VAO and VBO to store the line's vertex data
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Bind VAO and VBO
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Define how the vertex data is laid out (position)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    // Unbind the VBO and VAO (not necessary but good practice)
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);








    GLuint VAO2, VBO2;
    glGenVertexArrays(1, &VAO2);
    glGenBuffers(1, &VBO2);

    glBindVertexArray(VAO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);



    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(GLfloat), points.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);













    glPointSize(2.0f);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    float rotation = 0.0f;
    double prevTime = glfwGetTime();
    std::cout << "Hello World!" << std::endl;
    int error;
    while (!glfwWindowShouldClose(window)) {
        // Handle events
        glfwPollEvents();

        // Clear the screen with a color
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f); // Dark blue background
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use the shader program
        glUseProgram(shaderProgram);

        double crntTime = glfwGetTime();
        if (crntTime - prevTime >= 1.0 / 24.0) {
            rotation += 0.05f;
            prevTime = crntTime;
        }

        glm::mat4 model(glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0.0f, 0.0f, 1.0f))), view(glm::lookAt(glm::vec3(-100, 0, 0), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f))), proj(glm::perspective(glm::radians(45.0f), static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 1000.0f));

        int modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        int viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        int projLoc = glGetUniformLocation(shaderProgram, "proj");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

        printf("\rrotation: %20f, window width: %10d, window height: %10d", rotation, windowWidth, windowHeight);

        //std::cout << "\rrotation: " << rotation << ", windowWidth: " << windowWidth << ", windowHeight: " << windowHeight;

        // Bind VAO and draw the line
        glBindVertexArray(VAO);
        glDrawArrays(GL_LINES, 0, 2);
        glBindVertexArray(GL_NONE);

        glBindVertexArray(VAO2);
        glDrawArrays(GL_POINTS, 0, points.size() / 3); // Render points
        glBindVertexArray(0);

        // Swap buffers
        glfwSwapBuffers(window);

        if (error = glGetError()) {
            std::cerr << "OpenGL error: " << error << std::endl;
            return EXIT_FAILURE;
        }
    }
    // Clean up resources
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // Terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
}
#endif
