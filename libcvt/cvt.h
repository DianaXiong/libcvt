#pragma once

#include "lloyd_euclidean_cvd.h"

namespace libcvd{
	inline unsigned RGB(double x) {
		return  (x < 0) ? 0 : (x > 255) ? 255 : unsigned(x);
	}
    struct SimpleMesh{
        std::vector<std::vector<double> > verts;
		std::vector<std::vector<int> > faces;
		std::vector<std::vector<double> > vertColor;
        void addVertex(double x, double y, double z){
            std::vector<double> c(3);
            c[0] = x; c[1] = y; c[2] = z;
            verts.push_back(c);
        }
        void addFace(int v1, int v2, int v3){
            std::vector<int> v(3);
            v[0] = v1; v[1] = v2; v[2] = v3;
            faces.push_back(v);
        }

        // CVT
		void addVertexColor(double x, double y, double z){
			std::vector<double> c(3);
			c[0] = x; c[1] = y; c[2] = z;
			vertColor.push_back(c);
		}

		bool loadOFF(std::string filename) {
			using namespace std;
			FILE* off_file;
			errno_t err = fopen_s(&off_file, filename.c_str(), "r");
			if (err != 0) return false;
			// First line is always OFF
			char header[1000];
			const std::string OFF("OFF");
			const std::string NOFF("NOFF");
			const std::string COFF("COFF");
			std::cout << "hello" << std::endl;
			if (std::fscanf(off_file, "%s\n", header) != 1
				|| !(
					string(header).compare(0, OFF.length(), OFF) == 0 ||
					string(header).compare(0, COFF.length(), COFF) == 0 ||
					string(header).compare(0, NOFF.length(), NOFF) == 0))
			{
				printf("Error: readOFF() first line should be OFF or NOFF or COFF, not %s...", header);
				fclose(off_file);
				return false;
			}
			bool has_normals = string(header).compare(0, NOFF.length(), NOFF) == 0;
			bool has_vertexColors = string(header).compare(0, COFF.length(), COFF) == 0;
			// Second line is #vertices #faces #edges
			int number_of_vertices;
			int number_of_faces;
			int number_of_edges;
			char tic_tac_toe;
			char line[1000];
			bool still_comments = true;
			while (still_comments)
			{
				fgets(line, 1000, off_file);
				still_comments = line[0] == '#';
			}
			sscanf(line, "%d %d %d", &number_of_vertices, &number_of_faces, &number_of_edges);
			verts.resize(number_of_vertices);
			faces.resize(number_of_faces);
			if (has_normals)
				;// N.resize(number_of_vertices);
			if (has_vertexColors)
				vertColor.resize(number_of_vertices);
			
			printf("%s %d %d %d\n",(has_normals ? "NOFF" : "OFF"),number_of_vertices,number_of_faces,number_of_edges);
			// Read vertices
			for (int i = 0; i < number_of_vertices;)
			{
				fgets(line, 1000, off_file);
				double x, y, z, nx, ny, nz;
				if (sscanf(line, "%lg %lg %lg %lg %lg %lg", &x, &y, &z, &nx, &ny, &nz) >= 3)
				{
					std::vector<double> vertex(3);
					vertex[0] = x;
					vertex[1] = y;
					vertex[2] = z;
					verts[i] = vertex;

					if (has_normals)
					{
						std::vector<double> normal(3);
						normal[0] = nx;
						normal[1] = ny;
						normal[2] = nz;
						//N[i] = normal;
					}

					if (has_vertexColors)
					{
						std::vector<double> color(3);
						color[0] = nx / 255.0;
						color[1] = ny / 255.0;
						color[2] = nz / 255.0;
						vertColor[i] = color;
					}
					i++;
				}
				else if (
					fscanf(off_file, "%[#]", &tic_tac_toe) == 1)
				{
					char comment[1000];
					fscanf(off_file, "%[^\n]", comment);
				}
				else
				{
					printf("Error: bad line (%d)\n", i);
					if (feof(off_file))
					{
						fclose(off_file);
						return false;
					}
				}
			}
			
			// Read faces
			for (int i = 0; i < number_of_faces;)
			{
				std::vector<int> face;
				int valence;
				if (fscanf(off_file, "%d", &valence) == 1)
				{
					face.resize(valence);
					for (int j = 0; j < valence; j++)
					{
						int index;
						if (j < valence - 1)
						{
							fscanf(off_file, "%d", &index);
						}
						else {
							fscanf(off_file, "%d%*[^\n]", &index);
						}

						face[j] = index;
					}
					faces[i] = face;
					i++;
				}
				else if (
					fscanf(off_file, "%[#]", &tic_tac_toe) == 1)
				{
					char comment[1000];
					fscanf(off_file, "%[^\n]", comment);
				}
				else
				{
					printf("Error: bad line\n");
					fclose(off_file);
					return false;
				}
			}
			fclose(off_file);
			return true;
		}
		
		bool writeOFF(string filename) {
			std::ofstream s(filename);
			bool hasColor = !vertColor.empty();
			if (!hasColor) {
				s << "OFF\n" << verts.size() << " " << faces.size() << " 0\n";
			}
			else {
				s << "COFF\n" << verts.size() << " " << faces.size() << " 0\n";
			}
			
			for (unsigned i = 0; i < verts.size(); ++i) {
				if (hasColor) {
					s << verts[i][0] << " " << verts[i][1] << " " << verts[i][2] << " ";
					s << RGB(vertColor[i][0] * 255.0) << " " << RGB(vertColor[i][1] * 255.0) << " " << RGB(vertColor[i][2] * 255.0) << std::endl;
				}
				else {
					s << verts[i][0] << " " << verts[i][1] << " " << verts[i][2] << " ";
				}
			}

			for (unsigned i = 0; i < faces.size(); ++i) {
				s << "3 " << faces[i][0] << " " << faces[i][1] << " " << faces[i][2] << std::endl;
			}
			s.close();
			return true;
		}
    };

    void computeCVT(SimpleMesh & m, int regions = 20, int iterations = 200)
    {
        Mesh mesh;

        // insert vertices:
        Vertex** verts = new Vertex*[m.verts.size()];
        for(int i = 0; i < (int)m.verts.size(); i++)
        {
            verts[i] = new Vertex();
            verts[i]->a = Point(m.verts[i][0], m.verts[i][1], m.verts[i][2]);
			verts[i]->index = i;
            mesh.put_vertex(verts[i]);
        }

        auto hedges = new HedgeMap();

        // insert faces:
        for(int i = 0; i < (int)m.faces.size(); i++)
            mesh.put_face(m.faces[i][0], m.faces[i][1], m.faces[i][2], verts, hedges);

		// build adjacency
		mesh.link_mesh();

		// Compute CVT
        LloydCvd cvd(&mesh);
		cvd.lloyd_euclidean_cvd(&mesh, regions, iterations);

		// DEBUG with vertex colors:
		std::map < int, std::vector<double> > vcolors;
		for (auto p : mesh.pc_){
			std::vector<double> c(3);
			c[0] = random1();c[1] = random1();c[2] = random1();
			for (auto f : p->fpc_)
				for (int i = 0; i < 3; i++)
					vcolors[f->vertex(i)->index] = c;
		}
		for (auto keyValue : vcolors){
			auto c = keyValue.second;
			m.addVertexColor(c[0], c[1], c[2]);
		}
    }
}
