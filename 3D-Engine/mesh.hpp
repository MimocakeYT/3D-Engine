#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include <strstream>
#include "mathematics.hpp"

struct triangle
{
    std::vector<vec3> p;
    vec3 normal;
    triangle() { p = std::vector<vec3>(3); }
    triangle(std::vector<vec3> P) { p = P; }
    triangle(vec3 p1, vec3 p2, vec3 p3) { p = std::vector<vec3>(3); p[0] = p1; p[1] = p2; p[2] = p3; };
    std::vector<triangle> clip_fun(vec3 plane_p, vec3 plane_n)
    {
        plane_n.norm();

        auto dist = [&](vec3& p)
            {
                vec3 n = p;
                n.norm();
                return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - dot_prod(plane_n, plane_p));
            };

        vec3 inside[3];  int inside_count = 0;
        vec3 outside[3]; int outside_count = 0;

        for (int i = 0; i < 3; i++)
        {
            if (dist(this->p[i]) >= 0)
            {
                inside[inside_count++] = this->p[i];
            }
            else
            {
                outside[outside_count++] = this->p[i];
            }
        }

        if (inside_count == 0) return std::vector<triangle>(0);
        if (inside_count == 3) return std::vector<triangle>(1, *this);
        if (inside_count == 1 && outside_count == 2)
        {
            float t;
            triangle out;
            out.p[0] = inside[0];

            out.p[1] = intersectPlane(plane_p, plane_n, inside[0], outside[0], t);

            out.p[2] = intersectPlane(plane_p, plane_n, inside[0], outside[1], t);

            out.normal = normal;

            return std::vector<triangle>{ out };
        }
        if (inside_count == 2 && outside_count == 1)
        {
            triangle out1, out2;
            float t;

            out1.p[0] = inside[0];
            out1.p[1] = inside[1];
            out1.p[2] = intersectPlane(plane_p, plane_n, inside[0], outside[0], t);
            out1.normal = normal;

            out2.p[0] = inside[1];
            out2.p[1] = out1.p[2];
            out2.p[2] = intersectPlane(plane_p, plane_n, inside[1], outside[0], t);
            out2.normal = normal;

            return std::vector<triangle>{ out1, out2 };
        }
    }
};

class mesh
{
public:
    std::vector<triangle> tris;
    mesh() { tris = {}; }
    mesh(std::vector<triangle>& TRIS) : tris(TRIS) {}
    bool load_from_file(char* file_name)
    {
        this->tris = std::vector<triangle>();
        std::ifstream file(file_name);
        if (!file.is_open()) return 0;
        std::vector<vec3> verts;

        while (!file.eof())
        {
            char line[128];
            file.getline(line, 128);
            std::strstream s;
            s << line;

            char j = 0;
            if (line[0] == 'v')
            {
                vec3 v;
                s >> j >> v.x >> v.y >> v.z;
                verts.push_back(v);
            }
            if (line[0] == 'f')
            {
                std::vector<int> f(3);
                s >> j >> f[0] >> f[1] >> f[2];
                triangle t(verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1]);
                this->tris.push_back(t);
            }
        }
        return true;
    }
    void define_as_cube()
    {
        this->tris = {

            // SOUTH
            triangle{ vec3(0.0f, 0.0f, 0.0f),    vec3(0.0f, 1.0f, 0.0f),    vec3(1.0f, 1.0f, 0.0f) },
            triangle{ vec3(0.0f, 0.0f, 0.0f),    vec3(1.0f, 1.0f, 0.0f),    vec3(1.0f, 0.0f, 0.0f) },
            // EAST                                                                         
            triangle(vec3(1.0f, 0.0f, 0.0f),    vec3(1.0f, 1.0f, 0.0f),    vec3(1.0f, 1.0f, 1.0f)),
            triangle(vec3(1.0f, 0.0f, 0.0f),    vec3(1.0f, 1.0f, 1.0f),    vec3(1.0f, 0.0f, 1.0f)),
            // NORTH                                                                        
            triangle(vec3(1.0f, 0.0f, 1.0f),    vec3(1.0f, 1.0f, 1.0f),    vec3(0.0f, 1.0f, 1.0f)),
            triangle(vec3(1.0f, 0.0f, 1.0f),    vec3(0.0f, 1.0f, 1.0f),    vec3(0.0f, 0.0f, 1.0f)),
            // WEST                                                                         
            triangle(vec3(0.0f, 0.0f, 1.0f),    vec3(0.0f, 1.0f, 1.0f),    vec3(0.0f, 1.0f, 0.0f)),
            triangle(vec3(0.0f, 0.0f, 1.0f),    vec3(0.0f, 1.0f, 0.0f),    vec3(0.0f, 0.0f, 0.0f)),
            // TOP                                                                        
            triangle(vec3(0.0f, 1.0f, 0.0f),    vec3(0.0f, 1.0f, 1.0f),    vec3(1.0f, 1.0f, 1.0f)),
            triangle(vec3(0.0f, 1.0f, 0.0f),    vec3(1.0f, 1.0f, 1.0f),    vec3(1.0f, 1.0f, 0.0f)),
            // BOTTOM                                                                     
            triangle(vec3(1.0f, 0.0f, 1.0f),    vec3(0.0f, 0.0f, 1.0f),    vec3(0.0f, 0.0f, 0.0f)),
            triangle(vec3(1.0f, 0.0f, 1.0f),    vec3(0.0f, 0.0f, 0.0f),    vec3(1.0f, 0.0f, 0.0f)),
        };
    }
};