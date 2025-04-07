#include "write_ply.h"
#include <fstream>
#include <iostream>
#include <sstream>

void writePLY(const std::vector<float>& vertices, const std::vector<float>& normals, const std::string& fileName) {
    std::ofstream outFile(fileName);
    if (!outFile.is_open()){
        std::cerr << "Error: Unable to open file " << fileName << " for writing." << std::endl;
        return;
    }
    int vertexCount = vertices.size() / 3;
    int faceCount = vertices.size() / 9;
    
    // Write header
    outFile << "ply\n";
    outFile << "format ascii 1.0\n";
    outFile << "element vertex " << vertexCount << "\n";
    outFile << "property float x\n";
    outFile << "property float y\n";
    outFile << "property float z\n";
    outFile << "property float nx\n";
    outFile << "property float ny\n";
    outFile << "property float nz\n";
    outFile << "element face " << faceCount << "\n";
    outFile << "property list uchar int vertex_indices\n";
    outFile << "end_header\n";
    
    // Write vertices and normals
    for (size_t i = 0; i < vertices.size(); i += 3) {
        outFile << vertices[i] << " " << vertices[i+1] << " " << vertices[i+2] << " "
                << normals[i] << " " << normals[i+1] << " " << normals[i+2] << "\n";
    }
    
    // Write face indices (each face uses 3 consecutive vertices)
    for (int i = 0; i < faceCount; ++i) {
        outFile << "3 " << i*3 << " " << i*3 + 1 << " " << i*3 + 2 << "\n";
    }
    outFile.close();
}

// A very basic readPLY function (for starter purposes)
void readPLY(const std::string& filename, std::vector<Vertex>& vertices, std::vector<int>& indices) {
    std::ifstream file(filename);
    if (!file.is_open()){
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return;
    }
    std::string line;
    // Skip header until "end_header"
    while (std::getline(file, line)) {
        if (line == "end_header") break;
    }
    // Read vertices (this starter assumes a fixed number or until failure)
    Vertex v;
    while (file >> v.x >> v.y >> v.z >> v.nx >> v.ny >> v.nz) {
        vertices.push_back(v);
    }
    // (For indices, you would parse the face definitions similarly.)
    file.close();
}
