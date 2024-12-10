// Made by Mimocake in 2024
// Used tutorials of javidx9: https://www.youtube.com/watch?v=ih20l3pJoeU

#include <SFML/Graphics.hpp>
#include <algorithm>
#include <iostream>
#include "mesh.hpp"
using namespace std;
using namespace sf;

int main()
{
    vec3 camera_loc(0, -2.0, -5);
    mat4x4 proj_mat = mat4x4::get_proj_mat(1920, 1080, 0.1, 1000, 90);

    mesh TestMesh;
    if (!TestMesh.load_from_file((char*)"teapot.obj")) return 0;

    float theta = 0; 

    bool outline_only = false;
    bool allow_mouse_movement = false;
    bool allow_rotation = true;

    bool keys[6] = { 0, 0, 0, 0, 0, 0 };

    vec3 look_dir(0, 0, 1);
    vec3 up_dir(0, 1, 0);

    vec3 light_dir(1, -0.5, -0.7);
    light_dir.norm();

    Clock clock_for_movement;
    Clock clock_for_FPS;

    const int WINDOW_HEIGHT = 1080;
    const int WINDOW_WIDTH = 1920;

    Vector2i mouse_offset;

    RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "3D Engine");
    window.setVerticalSyncEnabled(true);
    Mouse::setPosition(Vector2i(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2), window);
    window.setMouseCursorVisible(false);
    allow_mouse_movement = true;

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case Event::Closed: window.close(); break;
            case Event::KeyPressed:
                if (event.key.code == Keyboard::Escape) window.close();
                if (event.key.code == Keyboard::W) keys[0] = true;
                if (event.key.code == Keyboard::A) keys[1] = true;
                if (event.key.code == Keyboard::S) keys[2] = true;
                if (event.key.code == Keyboard::D) keys[3] = true;
                if (event.key.code == Keyboard::Space) keys[4] = true;
                if (event.key.code == Keyboard::LShift) keys[5] = true;
                if (event.key.code == Keyboard::Tab) outline_only = !outline_only;
                if (event.key.code == Keyboard::LControl) allow_mouse_movement = !allow_mouse_movement;
                if (event.key.code == Keyboard::R) allow_rotation = !allow_rotation;
                break;
            case Event::KeyReleased:
                if (event.key.code == Keyboard::W) keys[0] = false;
                if (event.key.code == Keyboard::A) keys[1] = false;
                if (event.key.code == Keyboard::S) keys[2] = false;
                if (event.key.code == Keyboard::D) keys[3] = false;
                if (event.key.code == Keyboard::Space) keys[4] = false;
                if (event.key.code == Keyboard::LShift) keys[5] = false;
                break;
            case Event::MouseMoved:
                if (allow_mouse_movement)
                {
                    mouse_offset.x = event.mouseMove.x - WINDOW_WIDTH / 2;
                    mouse_offset.y = event.mouseMove.y - WINDOW_HEIGHT / 2;
                    Mouse::setPosition(Vector2i(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2), window);
                }
                break;
            default: break;
            }
        }

        if (clock_for_movement.getElapsedTime().asMilliseconds() >= 10)
        {
            vec3 vel(0, 0, 0);
            if (keys[0]) vel += vec3(0, 0, 0.1);
            if (keys[1]) vel += vec3(-0.1, 0, 0);
            if (keys[2]) vel += vec3(0, 0, -0.1);
            if (keys[3]) vel += vec3(0.1, 0, 0);
            if (keys[4]) vel += vec3(0, -0.1, 0);
            if (keys[5]) vel += vec3(0, 0.1, 0);

            vec3 temp_dir(look_dir.x, 0, look_dir.z);
            temp_dir.norm();
            float phi = acos(dot_prod(temp_dir, vec3(0, 0, 1)));
            phi = (temp_dir.x < 0) ? phi : -phi;
            vec3 rot_vel = vel * mat4x4::get_rot_y(phi);
            camera_loc += rot_vel;
            clock_for_movement.restart();
        }

        look_dir = look_dir * mat4x4::get_rot_y(-mouse_offset.x * 0.005);
        vec3 hor_dir(look_dir.x, 0, look_dir.z);
        hor_dir.norm();
        float phi = acos(dot_prod(hor_dir, vec3(0, 0, 1)));
        phi = (hor_dir.x < 0) ? -phi : phi;
        vec3 temp_dir = look_dir * mat4x4::get_rot_y(phi);
        temp_dir = temp_dir * mat4x4::get_rot_x(-mouse_offset.y * 0.005);
        look_dir = temp_dir * mat4x4::get_rot_y(-phi);
        look_dir.norm();
        vec3 target = camera_loc + look_dir;
        mouse_offset = Vector2i(0, 0);
        mat4x4 view_mat = mat4x4::get_point_at_mat(camera_loc, target, up_dir);
        view_mat.invert();

        window.clear(Color::White);

        vector<triangle> to_draw;
        for (auto t : TestMesh.tris)
        {
            triangle new_tri, proj;
            for (int k = 0; k < 3; k++)
            {
                new_tri.p[k] = t.p[k] * mat4x4::get_rot_y(theta * 1.5);
                new_tri.p[k] = new_tri.p[k] * mat4x4::get_rot_z(3.1415);
            }

            new_tri.normal = cross_prod(new_tri.p[2] - new_tri.p[0], new_tri.p[1] - new_tri.p[0]);
            new_tri.normal.norm();

            vec3 cam_dir = (new_tri.p[0]+new_tri.p[1]+new_tri.p[2])/3 - camera_loc;
            if (dot_prod(new_tri.normal, cam_dir) < 0)
                continue;

            for (int k = 0; k < 3; k++)
            {
                new_tri.p[k] = new_tri.p[k] * view_mat;
            }

            vector<triangle> clipped = new_tri.clip_fun(vec3(0, 0, 0.2), vec3(0, 0, 1));
            for (int n = 0; n < clipped.size(); n++)
            {
                for (int m = 0; m < 3; m++)
                {
                    clipped[n].p[m] = clipped[n].p[m] * proj_mat;
                }
            }

            vector<triangle> Q = clipped;
            for (int x = 0; x < 4; x++)
            {
                vector<triangle> temp;
                for (int y = 0; y < Q.size(); y++)
                {
                    triangle t = Q[y];
                    vector<triangle> new_t;
                    switch (x)
                    {
                    case 0: new_t = t.clip_fun(vec3(0, -1, 0), vec3(0, 1, 0)); break; //TOP
                    case 1: new_t = t.clip_fun(vec3(0, 1, 0), vec3(0, -1, 0)); break; //BOTTOM
                    case 2: new_t = t.clip_fun(vec3(-1, 0, 0), vec3(1, 0, 0)); break; //LEFT
                    case 3: new_t = t.clip_fun(vec3(1, 0, 0), vec3(-1, 0, 0)); break; //RIGHT
                    }
                    for (int z = 0; z < new_t.size(); z++)
                    {
                        temp.push_back(new_t[z]);
                    }
                }
                Q = temp;
            }
            for (auto y: Q) to_draw.push_back(y);
        }
        
        sort(to_draw.begin(), to_draw.end(), [](triangle& t1, triangle& t2)
            {
                float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
                float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
                return z1 > z2;
            });

        for (auto T : to_draw)
        {
            if (outline_only)
            {
                sf::VertexArray outline(sf::LinesStrip, 4);

                for (int j = 0; j < 4; j++)
                {
                    float x = (T.p[j % 3].x + 1) * window.getSize().x / 2;
                    float y = (T.p[j % 3].y + 1) * window.getSize().y / 2;
                    outline[j].color = sf::Color::Black;
                    outline[j].position = sf::Vector2f(x, y);
                }

                window.draw(outline);
            }
            else
            {
                VertexArray tri(Triangles, 3);

                for (int j = 0; j < 3; j++)
                {
                    float x = (T.p[j].x + 1) * window.getSize().x / 2;
                    float y = (T.p[j].y + 1) * window.getSize().y / 2;
                    tri[j].position = Vector2f(x, y);
                    int R = 153 * (0.3 + 0.7 * max(0.0f, dot_prod(-T.normal, light_dir)));
                    int G = 249 * (0.3 + 0.7 * max(0.0f, dot_prod(-T.normal, light_dir)));
                    int B = 142 * (0.3 + 0.7 * max(0.0f, dot_prod(-T.normal, light_dir)));
                    tri[j].color = Color(R, G, B);
                }

                window.draw(tri);
            }
        }

        int FPS = 1.0f / clock_for_FPS.getElapsedTime().asSeconds();
        clock_for_FPS.restart();
        string name = "FPS: " + to_string(FPS) + "; Triangles rendering: " + to_string(TestMesh.tris.size()) + "; Triangles drawing: " + to_string(to_draw.size());
        window.setTitle(name);
    theta += 0.01 * allow_rotation;
    window.display();
    }
    return 0;
}