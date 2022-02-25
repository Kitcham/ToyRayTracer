#ifndef TRIMESH_H
#define TRIMESH_H


#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <sys/types.h>

#include "intersect_list.h"
#include "TriangleMesh.h"

using namespace std;

class TriMesh {
public:
	TriMesh() {}
	TriMesh(std::string name, vec3 kdi, vec3 ksi) {
		fileName = name;
		kd = kdi;
		ks = ksi;
	}

	/*	Loads an obj file */
	
	/*void loadFromOBJ() {
		FILE* stream;
		size_t len = 0;
		long int nread = 0;
		std::ifstream in;
		in.open(fileName, std::ifstream::in);
		std::vector<vec3> vertices;
		std::vector<std::vector<int>> faces;
		if (in.fail()) return;

		std::string line;

		
		while (!in.eof()) {
			std::getline(in, line);
			std::istringstream iss(line.c_str());
			char trash;
			if (!line.compare(0, 2, "v ")) {
				iss >> trash;
				vec3 v;
				for (int i = 0; i < 3; i++) iss >> v[i];
				vertices.push_back(v);
			}
			else if (!line.compare(0, 2, "f ")) {
				std::vector<int> f;
				int itrash, idx;
				iss >> trash;
				while (iss >> idx >> trash >> itrash >> trash >> itrash) {
					idx--; // in wavefront obj all indices start at 1, not zero
					f.push_back(idx);
				}
				faces.push_back(f);
			}
		}
		std::cerr << "# v# " << vertices.size() << " f# " << faces.size() << std::endl;
		
		numVertices = vertices.size();
		numFaces = faces.size();
		
		for (int i = 0; i < numVertices; i++) {
			normals.push_back(vec3(0, 0, 0));
		}

		//cout << numVertices << endl;
		//cout << numFaces << endl;
		//printVertices();
		//printFaces();
	}*/
	
	
	void loadFromOBJ() {
		FILE* stream;
		size_t len = 0;
		long int nread = 0;
		char filechar[300];
		std::string line;
		std::string linetemp;
		std::ifstream in;
		in.open(fileName, std::ifstream::in);

	
		//char* line = NULL;
		/*
		if (stream == NULL) {
			perror("fopen");
			exit(EXIT_FAILURE);
		}*/

		//while ((nread = getline(&line, &len, stream)) != -1) 
		while (!in.eof())
		{
			std::getline(in, linetemp);
			std::istringstream iss(line.c_str());
			//char* line = linetemp.c_str();
			if (line[0] == '#') 
			{
				continue;
			}
			else 
			{
				//fwrite(line, nread, 1, stdout);
				if (line[0] == 'v') {
					stringstream ss(line);
					string m = line;
					int axis = 0;
					vec3 vec;
					while (getline(ss, m, ' ')) {
						if (m[0] == 'v' || m[0] == '\n') continue;
						if (axis == 2) m[m.size() - 1] = '\0';
						if (axis >= 4) break;
						//std::cerr << "axis = " << axis << endl;
						//std::cerr << "here = " << m << endl; // m might be \n
						vec[axis] = stod(m);
						axis += 1;
					}
					vertices.push_back(vec);
				}
				else if (line[0] == 'f') {
					stringstream ss(line);
					string m = line;
					int axis = 0;
					vec3 vec;
					while (getline(ss, m, ' ')) {
						if (m[0] == 'f' || m[0] == '\n') continue;
						if (axis == 2) m[m.size() - 1] = '\0';
						if (axis >= 4) break;
						//std::cerr << "axis = " << axis << endl;
						//std::cerr << "here = " << m << endl; // m might be \n
						vec[axis] = stoi(m);
						axis += 1;
					}
					faces.push_back(vec);
				}
				else {
					continue;
				}
			}
		}
		numVertices = vertices.size();
		numFaces = faces.size();

		for (int i = 0; i < numVertices; i++) {
			normals.push_back(vec3(0, 0, 0));
		}

		//cout << numVertices << endl;
		//cout << numFaces << endl;
		//printVertices();
		//printFaces();
	}
	

	/*	
	*   Generates the triangular mesh
	*	returns a hittable_list of triangles
	*/
	intersectList generateTriangles() {
		// faces stores a vector of 3 vertices
		// vertices stores a vector of 3 doubles
		intersectList triangles;
		for (int i = 0; i < numFaces; i++) {
			vec3 face = faces[i];
			int index1 = face[0];
			int index2 = face[1];
			int index3 = face[2];

			vec3 v1 = vertices[index1 - 1];
			vec3 v2 = vertices[index2 - 1];
			vec3 v3 = vertices[index3 - 1];

			/*v1 *= 200;
			v2 *= 200;
			v3 *= 200;
			*/
			v1 = v1 * 50;
			v2 = v2 * 50;
			v3 = v3 * 50;
			/*v1 = rotateAboutPoint(v1, 90, 1);
			v2 = rotateAboutPoint(v2, 90, 1);
			v3 = rotateAboutPoint(v3, 90, 1);*/
			triangles.add(make_shared<TriangleMesh>(v1, v2, v3, kd, ks, index1, index2, index3));

			vec3 per_face_normal = cross(v2 - v1, v3 - v1);
			//per_face_normal = normalize(per_face_normal);

			normals[index1 - 1] += per_face_normal;
			normals[index2 - 1] += per_face_normal;
			normals[index3 - 1] += per_face_normal;
		}

		for (int i = 0; i < normals.size(); i++) {
			normals[i] = unit_vector(normals[i]);
		}
		/*normals[0] = vec3(1,0,0);
		normals[1] = vec3(0,1,0);
		normals[2] = vec3(0,0,1);*/
		//printNormals();
		return triangles;
	}

	/*	
	    输出顶点数
	*/
	void printVertices() {
		for (int i = 0; i < vertices.size(); i++) {
			cout << vertices[i] << endl;
		}
	}

	/*	
	   输出面数
	*/
	void printFaces() {
		for (int i = 0; i < faces.size(); i++) {
			cout << faces[i] << endl;
		}
	}

	/*	
	   法向量数组
	*/
	void printNormals() {
		for (int i = 0; i < normals.size(); i++) {
			cout << normals[i] << endl;
		}
	}

	/* 
	   获取顶点数量
	*/
	int getVerticesSize() {
		return vertices.size();
	}

	/*	
	   获取面数
	*/
	int getFacesSize() {
		return faces.size();
	}

	vector<vec3> normals;
	vector<vec3> vertices;
private:
	vector<vec3> faces;	// length = # of triangles
	int numVertices;
	int numFaces;
	vec3 kd;
	vec3 ks;
	std::string fileName;
};

#endif