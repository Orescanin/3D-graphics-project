#include <main_state.h>
#include <glad/glad.h>
#include <math.h>


#include <rafgl.h>

#include <game_constants.h>

typedef struct _vertex_t
{                       /* offsets      */
    vec3_t position;    /* 0            */
    vec3_t colour;      /* 3 * float    */
    float alpha;        /* 6 * float    */
    float u, v;         /* 7 * float    */
    vec3_t normal;       /* 9 * float    */
    float random_number;
} vertex_t;

vertex_t vertex(vec3_t pos, vec3_t col, float alpha, float u, float v, vec3_t normal, float random)
{
    vertex_t vert;

    vert.position = pos;
    vert.colour = col;
    vert.alpha = alpha;
    vert.u = u;
    vert.v = v;
    vert.normal = normal;
    vert.random_number = random;

    return vert;
}
void v3show(vec3_t v)
{
    printf("(%.2f %.2f %.2f)\n", v.x, v.y, v.z);
}
int calculate_brightness(rafgl_pixel_rgb_t pixel)
{
    //printf("%d\n", pixel.r + pixel.g + pixel.b);
    return pixel.r + pixel.g + pixel.b;
}
float getHeight(int x, int y, rafgl_raster_t *height_map)
{
    float height = calculate_brightness(pixel_at_pm(height_map, y, x));
    height /= (256 + 256 + 256);
    height *= 0.5;
    return height;
}
vec3_t getNormal(int x, int z, rafgl_raster_t *raster)
{
    float heighL = getHeight(x-1, z, raster);
    float heighR = getHeight(x+1, z, raster);
    float heightD = getHeight(x, z-1, raster);
    float heightU = getHeight(x, z+1, raster);
    vec3_t normal = vec3(heighL - heighR, 2.0f, heightD - heightU);
    return normal;
}
int grid_width, grid_height;

static vec3_t _sky_colour;

static rafgl_texture_t skybox_tex;
static GLuint skybox_shader;
static GLuint skybox_uni_P, skybox_uni_V;
static rafgl_meshPUN_t skybox_mesh;

static GLuint vao, vbo, shader_program_id, uni_M, uni_VP, uni_phase, uni_camera_pos, sky_colour;
static rafgl_raster_t water_normal_raster;
static rafgl_texture_t water_normal_tex;

static rafgl_meshPUN_t heightMap;
static rafgl_raster_t heightMap_raster;


static rafgl_framebuffer_simple_t fbo;

void main_state_init(GLFWwindow *window, void *args, int width, int height)
{
    fbo = rafgl_framebuffer_simple_create(width, height);

    rafgl_raster_load_from_image(&heightMap_raster, "res/images/heightmap1.png");


    rafgl_meshPUN_init(&heightMap);
    rafgl_meshPUN_load_terrain_from_heightmap(&heightMap, grid_width, grid_height, "res/images/heightmap.png", 10);

    rafgl_texture_load_cubemap_named(&skybox_tex, "above_the_sea", "jpg");
    skybox_shader = rafgl_program_create_from_name("v9_skybox_shader");
    skybox_uni_P = glGetUniformLocation(skybox_shader, "uni_P");
    skybox_uni_V = glGetUniformLocation(skybox_shader, "uni_V");

    rafgl_meshPUN_init(&skybox_mesh);
    rafgl_meshPUN_load_cube(&skybox_mesh, 1.0f);


    rafgl_raster_load_from_image(&water_normal_raster, "res/images/water.png");

    /* rezervisemo texture slot.  */
    rafgl_texture_init(&water_normal_tex);

    rafgl_texture_load_from_raster(&water_normal_tex, &water_normal_raster);



    /* ovde mozemo da izmenimo podesavanja teksture */

    glBindTexture(GL_TEXTURE_2D, water_normal_tex.tex_id); /* bajndujemo doge teksturu */



    glGenerateMipmap(GL_TEXTURE_2D);



/*  Filtriranje teksture, za slucaj umanjenja (MIN) i uvecanja (MAG)  */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
/*
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
*/



/*  Sta raditi ako su UV koordinate van 0-1 opsega? Ograniciti na ivicu (CLAMP) ili ponavljati (REPEAT) */
/*
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
*/
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


    glBindTexture(GL_TEXTURE_2D, 0); /* unbajndujemo doge teksturu */


    /* GENERISANJE TERENA!!! */
    int x, y, v = 0;
    grid_width = 256;
    grid_height = 256;

    for(x = 0; x < 256; x++)
    {
        for(y = 0; y < 256; y++)
        {
            //printf("%.2f ", getHeight(x, y, &heightMap_raster));
        }
    }

    vertex_t *grid_vertices = malloc(grid_width * grid_height * 3 * 2 * sizeof(vertex_t));

    for(y = 0; y < grid_height; y++)
    {
        for(x = 0; x < grid_width; x++, v += 3 * 2)
        {
            //height = getHeight(-0.5f * grid_width + x, -0.5f * grid_height + y, heightMap_raster);
            //printf("%.2lf\n", height);
            grid_vertices[v +  0].position = vec3(-0.5f * grid_width * 0.001f + x       , getHeight(-0.5f * grid_width + x       , -0.5f * grid_height + y       , &heightMap_raster), -0.5f * grid_height * 0.001f + y       );
            grid_vertices[v +  1].position = vec3(-0.5f * grid_width * 0.001f + x + 1.0f, getHeight(-0.5f * grid_width + x + 1.0f, -0.5f * grid_height + y       , &heightMap_raster), -0.5f * grid_height * 0.001f + y       );
            grid_vertices[v +  2].position = vec3(-0.5f * grid_width * 0.001f + x       , getHeight(-0.5f * grid_width + x       , -0.5f * grid_height + y + 1.0f, &heightMap_raster), -0.5f * grid_height * 0.001f + y + 1.0f);

            grid_vertices[v +  3].position = vec3(-0.5f * grid_width * 0.001f + x + 1.0f, getHeight(-0.5f * grid_width + x + 1.0f, -0.5f * grid_height + y       , &heightMap_raster), -0.5f * grid_height * 0.001f + y       );
            grid_vertices[v +  4].position = vec3(-0.5f * grid_width * 0.001f + x + 1.0f, getHeight(-0.5f * grid_width + x + 1.0f, -0.5f * grid_height + y + 1.0f, &heightMap_raster), -0.5f * grid_height * 0.001f + y + 1.0f);
            grid_vertices[v +  5].position = vec3(-0.5f * grid_width * 0.001f + x       , getHeight(-0.5f * grid_width + x       , -0.5f * grid_height + y + 1.0f, &heightMap_raster), -0.5f * grid_height * 0.001f + y + 1.0f);
            /*
            v3show(grid_vertices[v + 0].position);
            v3show(grid_vertices[v + 1].position);
            v3show(grid_vertices[v + 2].position);

            v3show(grid_vertices[v + 3].position);
            v3show(grid_vertices[v + 4].position);
            v3show(grid_vertices[v + 5].position);
            printf("\n");
            */

            grid_vertices[v + 0].alpha = 0.2f;
            grid_vertices[v + 1].alpha = 1.0f;
            grid_vertices[v + 2].alpha = 0.4f;

            grid_vertices[v + 3].alpha = 0.5f;
            grid_vertices[v + 4].alpha = 1.0f;
            grid_vertices[v + 5].alpha = 0.8f;

            grid_vertices[v + 0].normal = getNormal(-0.5f * grid_width + x       , -0.5f * grid_height + y       , &heightMap_raster);
            grid_vertices[v + 1].normal = getNormal(-0.5f * grid_width + x + 1.0f, -0.5f * grid_height + y       , &heightMap_raster);
            grid_vertices[v + 2].normal = getNormal(-0.5f * grid_width + x       , -0.5f * grid_height + y + 1.0f, &heightMap_raster);

            grid_vertices[v + 3].normal = getNormal(-0.5f * grid_width + x + 1.0f, -0.5f * grid_height + y       , &heightMap_raster);
            grid_vertices[v + 4].normal = getNormal(-0.5f * grid_width + x + 1.0f, -0.5f * grid_height + y + 1.0f, &heightMap_raster);
            grid_vertices[v + 5].normal = getNormal(-0.5f * grid_width + x       , -0.5f * grid_height + y + 1.0f, &heightMap_raster);

            grid_vertices[v + 0].u = 0.0f;
            grid_vertices[v + 1].u = 0.0f;
            grid_vertices[v + 2].u = 1.0f;

            grid_vertices[v + 3].u = 1.0f;
            grid_vertices[v + 4].u = 0.0f;
            grid_vertices[v + 5].u = 1.0f;

            grid_vertices[v + 0].v =  0.0f;
            grid_vertices[v + 1].v = 1.0f;
            grid_vertices[v + 2].v = 1.0f;

            grid_vertices[v + 3].v = 0.0f;
            grid_vertices[v + 4].v = 1.0f;
            grid_vertices[v + 5].v = 1.0f;

            grid_vertices[v + 0].random_number = (grid_vertices[v + 0].position.x + grid_vertices[v + 0].position.y + grid_vertices[v + 0].position.z + 2.0f) / 10;
            grid_vertices[v + 1].random_number = (grid_vertices[v + 1].position.x + grid_vertices[v + 1].position.y + grid_vertices[v + 1].position.z + 2.0f) / 10;
            grid_vertices[v + 2].random_number = (grid_vertices[v + 2].position.x + grid_vertices[v + 2].position.y + grid_vertices[v + 2].position.z + 2.0f) / 10;

            grid_vertices[v + 3].random_number = (grid_vertices[v + 3].position.x + grid_vertices[v + 3].position.y + grid_vertices[v + 3].position.z + 2.0f) / 10;
            grid_vertices[v + 4].random_number = (grid_vertices[v + 4].position.x + grid_vertices[v + 4].position.y + grid_vertices[v + 4].position.z + 2.0f) / 10;
            grid_vertices[v + 5].random_number = (grid_vertices[v + 5].position.x + grid_vertices[v + 5].position.y + grid_vertices[v + 5].position.z + 2.0f) / 10;
        }
    }


    shader_program_id = rafgl_program_create_from_name("grass_shader");
    uni_M = glGetUniformLocation(shader_program_id, "uni_M");
    uni_VP = glGetUniformLocation(shader_program_id, "uni_VP");
    uni_phase = glGetUniformLocation(shader_program_id, "uni_phase");
    uni_camera_pos = glGetUniformLocation(shader_program_id, "uni_camera_pos");
    sky_colour = glGetUniformLocation(shader_program_id, "sky_colour");

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, grid_width * grid_height * 3 * 2 * sizeof(vertex_t), grid_vertices, GL_STATIC_DRAW);

    /* position */
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*) 0);

    /* colour */
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*) (sizeof(vec3_t)));

    /* alpha */
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*) (2 * sizeof(vec3_t)));

    /* UV coords */
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*) (2 * sizeof(vec3_t) + 1 * sizeof(float)));

    /* normal */
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*) (2 * sizeof(vec3_t) + 3 * sizeof(float)));


    /* random numbers */
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*) (3 * sizeof(vec3_t) + 3 * sizeof(float)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    glEnable(GL_DEPTH_TEST);

    //_sky_colour = vec3(0.68, 0.85, 0.90);
    //glClearColor(0.68, 0.85, 0.90, 1.0f);
    _sky_colour = vec3(1.0f, 1.0f, 1.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}


mat4_t model, view, projection, view_projection;

/* field of view */
float fov = 75.0f;

vec3_t camera_position = vec3m(-5.0f, 2.0f, 2.0f);
vec3_t camera_target = vec3m(0.0f, 0.0f, 0.0f);
vec3_t camera_up = vec3m(0.0f, 1.0f, 0.0f);
vec3_t aim_dir = vec3m(0.0f, 0.0f, -1.0f);

float camera_angle = -M_PIf * 0.5f;
float angle_speed = 0.2f * M_PIf;
float move_speed = 0.8f;

float hoffset = 0.10f * M_PIf;



float time = 0.0f;
int reshow_cursor_flag = 0;
int last_lmb = 0;

void main_state_update(GLFWwindow *window, float delta_time, rafgl_game_data_t *game_data, void *args)
{
    //v3show(camera_position);
    time += delta_time;
    if(!game_data->keys_down[RAFGL_KEY_LEFT_CONTROL])
        move_speed = 50.0f, angle_speed = 1.0f * M_PIf;
    else move_speed = 0.8f, angle_speed = 0.2f * M_PIf;


    if(game_data->keys_down['A']) camera_angle -= angle_speed * delta_time;
    if(game_data->keys_down['D']) camera_angle += angle_speed * delta_time;


    if(game_data->is_lmb_down)
    {

        if(reshow_cursor_flag == 0)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

        float ydelta = game_data->mouse_pos_y - game_data->raster_height / 2;
        float xdelta = game_data->mouse_pos_x - game_data->raster_width / 2;

        if(!last_lmb)
        {
            ydelta = 0;
            xdelta = 0;
        }

        hoffset -= ydelta / game_data->raster_height;
        camera_angle += xdelta / game_data->raster_width;

        glfwSetCursorPos(window, game_data->raster_width / 2, game_data->raster_height / 2);
        reshow_cursor_flag = 1;
    }
    else if(reshow_cursor_flag)
    {
        reshow_cursor_flag = 0;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    last_lmb = game_data->is_lmb_down;

    aim_dir = vec3(cosf(camera_angle), 0.0f, sinf(camera_angle));
    /*
    if(game_data->keys_down['A'])
        camera_position = v3_add(camera_position, vec3(0,0,0.01f));
    if(game_data->keys_down['D'])
        camera_position = v3_add(camera_position, vec3(0,0,-0.01f));
    */
    if(game_data->keys_down['W']) camera_position = v3_add(camera_position, v3_muls(aim_dir, move_speed * delta_time));
    if(game_data->keys_down['S']) camera_position = v3_add(camera_position, v3_muls(aim_dir, -move_speed * delta_time));

    if(game_data->keys_down[RAFGL_KEY_SPACE]) camera_position.y += move_speed * 1.0f * delta_time;
    if(game_data->keys_down[RAFGL_KEY_LEFT_SHIFT]) camera_position.y -= move_speed * 1.0f * delta_time;

    if(game_data->keys_down[RAFGL_KEY_ESCAPE]) glfwSetWindowShouldClose(window, GLFW_TRUE);

    float aspect = ((float)(game_data->raster_width)) / game_data->raster_height;    projection = m4_perspective(fov, aspect, 0.01f, 1000.0f);

    if(!game_data->keys_down['T'])
    {
        view = m4_look_at(camera_position, v3_add(camera_position, v3_add(aim_dir, vec3(0.0f, hoffset, 0.0f))), camera_up);
    }
    else
    {
        view = m4_look_at(camera_position, vec3(0.0f, 0.0f, 0.0f), camera_up);
    }

    model = m4_identity();

    view_projection = m4_mul(projection, view);



}


void main_state_render(GLFWwindow *window, void *args)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



    /* Prvo box */
/*    glDepthMask(GL_FALSE);

    glUseProgram(skybox_shader);
    glUniformMatrix4fv(skybox_uni_V, 1, GL_FALSE, (void*) view.m);
    glUniformMatrix4fv(skybox_uni_P, 1, GL_FALSE, (void*) projection.m);

    glBindVertexArray(skybox_mesh.vao_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_tex.tex_id);

    glDrawArrays(GL_TRIANGLES, 0, skybox_mesh.vertex_count);
    glDepthMask(GL_TRUE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);


    /* bitno je da i za vreme crtanja imamo bajndovanu teksturu */

    glUseProgram(shader_program_id);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, water_normal_tex.tex_id);

    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
    glBindVertexArray(vao);

    glUniformMatrix4fv(uni_M, 1, GL_FALSE, (void*) model.m);
    glUniformMatrix4fv(uni_VP, 1, GL_FALSE, (void*) view_projection.m);
    glUniform1f(uni_phase, time * 0.1f);
    glUniform3f(uni_camera_pos, camera_position.x, camera_position.y, camera_position.z);
    glUniform3f(sky_colour, _sky_colour.x, _sky_colour.y, _sky_colour.z);

    glDrawArrays(GL_TRIANGLES, 0, grid_width * grid_height * 6);
    glBindVertexArray(0);
    glDisableVertexAttribArray(4);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);

}


void main_state_cleanup(GLFWwindow *window, void *args)
{
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteShader(shader_program_id);
}
