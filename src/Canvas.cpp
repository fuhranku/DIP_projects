#include "Canvas.h"

Canvas::Canvas(){
    VAO = 0;   
    VBO = 0;
    width = 0;
    height = 0;
}

void Canvas::Build(int width, int height, glm::vec2 arrhalfWidth, glm::vec2 arrhalfHeight) {
    this->width = width;
    this->height = height;
    this->arrhalfWidth = arrhalfWidth;
    this->arrhalfHeight = arrhalfHeight;

    vertices = 
    {
        // positions                              // texture Coords
        -arrhalfWidth[0],  arrhalfHeight[1], 0.0f, 0.0f, 1.0f,
        -arrhalfWidth[0], -arrhalfHeight[0], 0.0f, 0.0f, 0.0f,
         arrhalfWidth[1],  arrhalfHeight[1], 0.0f, 1.0f, 1.0f,
         arrhalfWidth[1], -arrhalfHeight[0], 0.0f, 1.0f, 0.0f,
    };

    // Setup plane VAO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices.front(), GL_STATIC_DRAW);

    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    // Texture Coords
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    // Compute Grid
    grid = Grid(arrhalfWidth, arrhalfHeight);
}

void Canvas::Delete() {
    // Deletes the vertex array from the GPU
    glDeleteVertexArrays(1, &VAO);
    // Deletes the vertex object from the GPU
    glDeleteBuffers(1, &VBO);
}

void Canvas::Update(int width, int height, glm::vec2 arrhalfWidth, glm::vec2 arrhalfHeight) {
    Delete();
    Build(width, height, arrhalfWidth, arrhalfHeight);
}

Grid::Grid() {

}

Grid::Grid(glm::vec2 halfWidth, glm::vec2 halfHeight){ 
    // Load Grid Shader
    gridShader = new Shader("assets/shaders/gridShader.vert", "assets/shaders/gridShader.frag");

    int width = halfWidth[0] + halfWidth[1];
    int height = halfHeight[0] + halfHeight[1];

    std::vector<glm::vec3> vertices;
    std::vector<glm::uvec4> indices;

    for (int j = -halfHeight[0]; j <= halfHeight[1]; ++j) {
        for (int i = -halfWidth[0]; i <= halfWidth[1]; ++i) {
            float x = (float)i;
            float y = (float)j;
            vertices.push_back(glm::vec3(x, y, 0.014));
        }
    }

    for (int j = 0; j < height; ++j) {  
        for (int i = 0; i < width; ++i) {

            int row1 = j * (width + 1);
            int row2 = (j + 1) * (width + 1);

            indices.push_back(glm::uvec4(row1 + i, row1 + i + 1, row1 + i + 1, row2 + i + 1));
            indices.push_back(glm::uvec4(row2 + i + 1, row2 + i, row2 + i, row1 + i));

        }
    }

    glGenVertexArrays(1, &gridVAO);
    glBindVertexArray(gridVAO);
    glGenBuffers(1, &gridVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glGenBuffers(1, &gridIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gridIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(glm::uvec4), &indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    gridLength = (GLuint)indices.size() * 4;
}

void Grid::Draw(glm::mat4 viewMatrix, glm::mat4 orthoMatrix){
    gridShader->use();
    gridShader->setMat4("model", glm::mat4(1.0f));
    gridShader->setMat4("view", viewMatrix);
    gridShader->setMat4("projection", orthoMatrix);

    glBindVertexArray(gridVAO);

    glDrawElements(GL_LINES, gridLength, GL_UNSIGNED_INT, NULL);

    glBindVertexArray(0);
}