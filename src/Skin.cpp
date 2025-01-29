#include "Skin.h"

Skin::Skin() {
    skeleton = nullptr;
}

Skin::~Skin() {
    delete skeleton;
}

bool Skin::Load(const char* filename, Skeleton* skeleton = nullptr) {
    
    if (!filename) {
        return true;
    }
    
    printf("Skin::Load - loading skin from file: %s\n", filename);
    this->skeleton = skeleton;

    // open .skin file
    Tokenizer token;
    token.Open(filename);
    char temp[256];
    token.GetToken(temp);

    // Joint::Load is a while-loop because there are an arbitrary amount of joints
    // for Skin::Load, the structure of the file is known (only need for-loops for each part)
    // p.s. what i implemented Joint::Load, should probably mostly be in Skeleton::Load

    // load positions
    token.FindToken("positions");
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

    // load normals
    token.FindToken("normals");
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

    // load skinweights
    token.FindToken("skinweights");
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

    // load triangles
    token.FindToken("triangles");
    int numTriangles = token.GetInt();
    token.FindToken("{");
    triangles.resize(numTriangles);
    
    for (int i = 0; i < numTriangles; i++) {
        int index0 = token.GetInt();
        int index1 = token.GetInt();
        int index2 = token.GetInt();
        triangles[i] = Triangle(vertices[index0], vertices[index1], vertices[index2]);
        printf("Skin::Load - triangle %d: %d, %d, %d\n", i, index0, index1, index2);
    }
    token.FindToken("}");

    // load bindings
    token.FindToken("bindings");
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

    // working copies that get modified during skinning
    transformedPositions = positions;
    transformedNormals = normals;

    // allocate skinning matrices if skeleton is provided
    if (this->skeleton) {
        skinningMatrices.resize(bindings.size());
    }

    token.Close();
    printf("Skin::Load - finished loading skin\n");
}

void Skin::Update() {

}

void Skin::Draw() {

}