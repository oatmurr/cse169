#include "Skin.h"

Skin::Skin() {
    skeleton = nullptr;

    model = glm::mat4(1.0f);

    color = glm::vec3(0.8f, 0.8f, 0.8f);
}

Skin::~Skin() {
    delete skeleton;
}

bool Skin::Load(const char* filename, Skeleton* skeleton) {
    
    if (!filename) {
        return true;
    }
    
    printf("Skin::Load - loading skin from file: %s\n", filename);
    this->skeleton = skeleton;

    // open .skin file
    Tokenizer token;
    token.Open(filename);

    while (true) {
        char temp[256];
        token.GetToken(temp);

        if (strcmp(temp, "positions") == 0) {
            int numPositions = token.GetInt();
            token.FindToken("{");
            // direct indexing is faster than repeated resizing with push_back()
            positions.resize(numPositions);

            for (int i = 0; i < numPositions; i++) {

                glm::vec3 position;
                position.x = token.GetFloat();
                position.y = token.GetFloat();
                position.z = token.GetFloat();
                positions[i] = position;
                printf("Skin::Load - position %d: %f, %f, %f\n", i, position.x, position.y, position.z);
            }
            token.FindToken("}");

        } else if (strcmp(temp, "normals") == 0) {
            int numNormals = token.GetInt();
            token.FindToken("{");
            normals.resize(numNormals);

            for (int i = 0; i < numNormals; i++) {
                
                glm::vec3 normal;
                normal.x = token.GetFloat();
                normal.y = token.GetFloat();
                normal.z = token.GetFloat();
                normals[i] = normal;
                printf("Skin::Load - normal %d: %f, %f, %f\n", i, normal.x, normal.y, normal.z);
            }
            token.FindToken("}");

        } else if (strcmp(temp, "skinweights") == 0) {
            int numSkinweights = token.GetInt();
            token.FindToken("{");
            // theoretically, positions, normals, vertices should all be the same size
            vertices.resize(numSkinweights);

            for (int i = 0; i < numSkinweights; i++) {

                int numAttachments = token.GetInt();
                std::vector<int> jointIndices;
                std::vector<float> weights;

                for (int j = 0; j < numAttachments; j++) {
                    jointIndices.push_back(token.GetInt());
                    weights.push_back(token.GetFloat());
                }
                vertices[i] = Vertex(positions[i], normals[i], numAttachments, jointIndices, weights);
                printf("Skin::Load - vertex %d: %f, %f, %f with %d attachments\n", i, positions[i].x, positions[i].y, positions[i].z, numAttachments);
            }
            token.FindToken("}");

        } else if (strcmp(temp, "triangles") == 0) {
            int numTriangles = token.GetInt();
            token.FindToken("{");
            triangles.resize(numTriangles);
            
            for (int i = 0; i < numTriangles; i++) {
                int index0 = token.GetInt();
                int index1 = token.GetInt();
                int index2 = token.GetInt();
                triangles[i] = Triangle(index0, index1, index2);
                printf("Skin::Load - triangle %d: %d, %d, %d\n", i, index0, index1, index2);
            }
            token.FindToken("}");

        } else if (strcmp(temp, "bindings") == 0) {
            int numBindings = token.GetInt();
            token.FindToken("{");
            bindings.resize(numBindings);

            for (int i = 0; i < numBindings; i++) {
                token.FindToken("matrix");
                token.FindToken("{");
                glm::mat4 matrix;
                // example binding matrix:
                // matrix {
                //     1.000    0.000    0.000
                //     0.000    1.000    0.000
                //     0.000    0.000    1.000
                //     0.000    0.000    0.000
                // }
                for (int row = 0; row < 3; row++) {
                    for (int col = 0; col < 4; col++) {
                        matrix[row][col] = token.GetFloat();
                    }
                }
                // last column is always 0, 0, 0, 1
                matrix[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
                bindings[i] = matrix;
                printf("Skin::Load - loaded binding matrix %d\n", i);
                token.FindToken("}");
            }
            token.FindToken("}");
            break;

        } else {
            printf("Skin::Load - unrecognised token: %s\n", temp);
            token.SkipLine();
        }
    }


    // char temp[256];
    // token.GetToken(temp);

    // // Joint::Load is a while-loop because there are an arbitrary amount of joints
    // // for Skin::Load, the structure of the file is known (only need for-loops for each part)
    // // p.s. what i implemented Joint::Load, should probably mostly be in Skeleton::Load

    // // load positions
    // token.FindToken("positions");
    // int numPositions = token.GetInt();
    // token.FindToken("{");
    // // direct indexing is faster than repeated resizing with push_back()
    // positions.resize(numPositions);

    // for (int i = 0; i < numPositions; i++) {
        
    //     glm::vec3 position;
    //     position.x = token.GetFloat();
    //     position.y = token.GetFloat();
    //     position.z = token.GetFloat();
    //     positions[i] = position;
    //     printf("Skin::Load - position %d: %f, %f, %f\n", i, position.x, position.y, position.z);
    // }
    // token.FindToken("}");

    // // load normals
    // token.FindToken("normals");
    // int numNormals = token.GetInt();
    // token.FindToken("{");
    // normals.resize(numNormals);

    // for (int i = 0; i < numNormals; i++) {
        
    //     glm::vec3 normal;
    //     normal.x = token.GetFloat();
    //     normal.y = token.GetFloat();
    //     normal.z = token.GetFloat();
    //     normals[i] = normal;
    //     printf("Skin::Load - normal %d: %f, %f, %f\n", i, normal.x, normal.y, normal.z);
    // }
    // token.FindToken("}");

    // // load skinweights
    // token.FindToken("skinweights");
    // int numSkinweights = token.GetInt();
    // token.FindToken("{");
    // // theoretically, positions, normals, vertices should all be the same size
    // vertices.resize(numSkinweights);

    // for (int i = 0; i < numSkinweights; i++) {
        
    //     int numAttachments = token.GetInt();
    //     std::vector<int> jointIndices;
    //     std::vector<float> weights;

    //     for (int j = 0; j < numAttachments; j++) {
    //         jointIndices.push_back(token.GetInt());
    //         weights.push_back(token.GetFloat());
    //     }
    //     vertices[i] = Vertex(positions[i], normals[i], numAttachments, jointIndices, weights);
    //     printf("Skin::Load - vertex %d: %f, %f, %f with %d attachments\n", i, positions[i].x, positions[i].y, positions[i].z, numAttachments);
    // }
    // token.FindToken("}");

    // // load triangles
    // token.FindToken("triangles");
    // int numTriangles = token.GetInt();
    // token.FindToken("{");
    // triangles.resize(numTriangles);
    
    // for (int i = 0; i < numTriangles; i++) {
    //     int index0 = token.GetInt();
    //     int index1 = token.GetInt();
    //     int index2 = token.GetInt();
    //     triangles[i] = Triangle(index0, index1, index2);
    //     printf("Skin::Load - triangle %d: %d, %d, %d\n", i, index0, index1, index2);
    // }
    // token.FindToken("}");

    // // load bindings
    // token.FindToken("bindings");
    // int numBindings = token.GetInt();
    // token.FindToken("{");
    // bindings.resize(numBindings);

    // for (int i = 0; i < numBindings; i++) {
    //     token.FindToken("matrix");
    //     token.FindToken("{");
    //     glm::mat4 matrix;
    //     // example binding matrix:
    //     // matrix {
	// 	//     1.000    0.000    0.000
	// 	//     0.000    1.000    0.000
	// 	//     0.000    0.000    1.000
	// 	//     0.000    0.000    0.000
	//     // }
    //     for (int row = 0; row < 3; row++) {
    //         for (int col = 0; col < 4; col++) {
    //             matrix[row][col] = token.GetFloat();
    //         }
    //     }
    //     // last column is always 0, 0, 0, 1
    //     matrix[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    //     bindings[i] = matrix;
    //     printf("Skin::Load - loaded binding matrix %d\n", i);
    //     token.FindToken("}");
    // }
    // token.FindToken("}");

    // working copies that get modified during skinning
    transformedPositions = positions;
    transformedNormals = normals;

    // allocate skinning matrices if skeleton is provided
    if (this->skeleton) {
        skinningMatrices.resize(bindings.size());
    }

    this->SetupBuffers();

    token.Close();
    printf("Skin::Load - finished loading skin\n");
    return true;
}
void Skin::Update() {
    // if no skeleton, mesh stays in binding pose
    if (!skeleton) {
        printf("Skin::Update - no skeleton, staying in binding pose\n");
        return;
    }

    // printf("Skin::Update - computing skinning matrices\n");
    // compute skinning matrix for each joint Mi = Wi * Bi^(-1)
    // Mi = skinning matrix of joint i
    // Wi = world matrix of joint i
    // Bi = binding matrix for joint i
    for (int i = 0; i < bindings.size(); i++) {
        skinningMatrices[i] = skeleton->GetWorldMatrix(i) * glm::inverse(bindings[i]);
        // printf("Skin::Update - computed skinning matrix %d\n", i);
    }

    // printf("Skin::Update - computing blended positions and normals\n");
    // compute blended world space positions and normals
    // transform each vertex position and normal
    for (int i = 0; i < positions.size(); i++) {
        glm::vec4 position = glm::vec4(positions[i], 1.0f);
        // use 0 as the 4th coordinate
        glm::vec4 normal = glm::vec4(normals[i], 0.0f);

        glm::vec4 blendedPosition = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
        glm::vec4 blendedNormal = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

        // apply weighted transformation from each attached joint
        for (int j = 0; j < vertices[i].GetNumAttachments(); j++) {
            // conversion from byte to float handled in Vertex::GetWeight()
            float weight = vertices[i].GetWeight(j);
            int jointIndex = vertices[i].GetJointIndex(j);

            // v' = Σ wi * Wi * Bi^(-1) * v
            blendedPosition += weight * (skinningMatrices[jointIndex] * position);
            // n' = Σ wi * (Wi * Bi^(-1))^(-1T) * n (normalisation done during storage) <- use this one "if there will be a scale and/or a shear in the transformations"
            blendedNormal += weight * (glm::transpose(glm::inverse(skinningMatrices[jointIndex])) * normal);
            // n' = Σ wi * Wi * Bi^(-1) * n
            // blendedNormal += weight * (skinningMatrices[jointIndex] * normal);
        }

        // storage
        transformedPositions[i] = glm::vec3(blendedPosition);
        transformedNormals[i] = glm::normalize(glm::vec3(blendedNormal));
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * transformedPositions.size(), transformedPositions.data());

    glBindBuffer(GL_ARRAY_BUFFER, VBO_normals); 
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * transformedNormals.size(), transformedNormals.data());

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // printf("Skin::Update - finished updating skin\n");
}

void Skin::Draw(const glm::mat4& viewProjMtx, GLuint shader) {
    // draw triangles using transformed positions and normals
    // printf("Skin::Draw - starting to draw skin\n");

    color = glm::vec3(0.8f, 0.8f, 0.8f);
    model = glm::mat4(1.0f);

    // activate the shader program
    glUseProgram(shader);
    // printf("Skin::Draw - shader program activated\n");

    // check uniform locations
    GLint viewProjLoc = glGetUniformLocation(shader, "viewProj");
    if (viewProjLoc == -1) {
        printf("Failed to find uniform: viewProj\n");
    }

    GLint modelLoc = glGetUniformLocation(shader, "model");
    if (modelLoc == -1) {
        printf("Failed to find uniform: model\n");
    }

    GLint colorLoc = glGetUniformLocation(shader, "DiffuseColor");
    if (colorLoc == -1) {
        printf("Failed to find uniform: DiffuseColor\n");
    }

    glUniformMatrix4fv(viewProjLoc, 1, false, (float*)&viewProjMtx);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float*)&model);
    glUniform3fv(colorLoc, 1, &color[0]);

    // get the locations and send the uniforms to the shader
    // glUniformMatrix4fv(glGetUniformLocation(shader, "viewProj"), 1, false, (float*)&viewProjMtx);
    // glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (float*)&model);
    // glUniform3fv(glGetUniformLocation(shader, "DiffuseColor"), 1, &color[0]);
    // printf("Skin::Draw - uniforms sent to shader\n");

    // bind the VAO
    glBindVertexArray(VAO);
    // printf("Skin::Draw - VAO bound\n");

    // draw the points using triangles, indexed with the EBO
    glDrawElements(GL_TRIANGLES, triangleIndices.size(), GL_UNSIGNED_INT, 0);
    // printf("Skin::Draw - triangles drawn\n");

    // unbind the VAO and shader program
    glBindVertexArray(0);
    glUseProgram(0);
    // printf("Skin::Draw - finished drawing skin\n");
}

void Skin::SetupBuffers() {
    printf("Skin::SetupBuffers - starting to setup buffers\n");

    // generate a vertex array (VAO) and two vertex buffer objects (VBO).
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO_positions);
    glGenBuffers(1, &VBO_normals);
    printf("Skin::SetupBuffers - VAO and VBOs generated\n");

    // bind to the VAO.
    glBindVertexArray(VAO);
    printf("Skin::SetupBuffers - VAO bound\n");

    // bind to the first VBO - we will use it to store the vertices
    glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positions.size(), positions.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    printf("Skin::SetupBuffers - positions VBO setup complete\n");

    // bind to the second VBO - We will use it to store the normals
    glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    printf("Skin::SetupBuffers - normals VBO setup complete\n");

    // new for skinning - fill triangle indices buffer
    for (int i = 0; i < triangles.size(); i++) {
        triangleIndices.push_back(triangles[i].GetVertexIndex1());
        triangleIndices.push_back(triangles[i].GetVertexIndex2());
        triangleIndices.push_back(triangles[i].GetVertexIndex3());
    }
    printf("Skin::SetupBuffers - triangle indices filled\n");

    // generate EBO, bind the EBO to the bound VAO and send the data
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * triangleIndices.size(), triangleIndices.data(), GL_STATIC_DRAW);
    printf("Skin::SetupBuffers - EBO setup complete\n");

    // unbind the VBOs.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    printf("Skin::SetupBuffers - finished setting up buffers\n");
}