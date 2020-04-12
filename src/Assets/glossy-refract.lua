white_mat = gr.material({1.0, 1.0, 1.0}, {0.0, 0.0, 0.0}, 1)
dim_white_mat = gr.material({0.8, 0.8, 0.8}, {0.0, 0.0, 0.0}, 1)
red_mat = gr.material({1.0, 0.0, 0.0}, {0.5, 0.5, 0.5}, 1)
green_mat = gr.material({141 / 255, 228 / 255, 21 / 255}, {0.2, 0.2, 0.2}, 100)
blue_mat = gr.material({0.0, 0.0, 1.0}, {0.5, 0.5, 0.5}, 1)
black_mat = gr.material({0.0, 0.0, 0.0}, {1.0, 1.0, 1.0}, 10)
gold_mat = gr.material({0.9, 0.8, 0.4}, {0.8, 0.8, 0.4}, 50)
purple_mat = gr.material({0.5, 0.0, 0.5}, {0.4, 0.4, 0.4}, 10000)

refract_mat = gr.clear_material({0, 0, 0}, {0.85, 0.85, 0.85}, 100, 25, 1.0)
refract_mat2 = gr.clear_material({0, 0, 0}, {0.9, 0.9, 0.9}, 100, 30, 1.1)
refract_mat3 = gr.clear_material({0, 0, 0}, {0.9, 0.9, 0.9}, 100, 20, 1.3)
red_checkered_mat = gr.checker_material({1, 0, 0}, {1, 1, 1}, {0.5, 0.5, 0.5}, 10, 0)

scene_root = gr.node('root')

box_height = 10.0
box_width = 10.0
box_length = 20.0
fudge = 0.1

back_plane = gr.nh_plane('back_plane')
back_plane:scale(box_width, box_height, 1)
back_plane:translate(0, 0, -box_length)
scene_root:add_child(back_plane)
back_plane:set_material(dim_white_mat)

behind_plane = gr.nh_plane('behind_plane')
behind_plane:scale(box_width, box_height, 1)
behind_plane:translate(0, 0, fudge)
scene_root:add_child(behind_plane)
behind_plane:set_material(white_mat)

top_plane = gr.nh_plane('top_plane')
top_plane:scale(box_width, box_length, 1)
top_plane:rotate('X', 90)
top_plane:translate(0, box_height / 2.0 - fudge, -10.0)
scene_root:add_child(top_plane)
top_plane:set_material(white_mat)

bottom_plane = gr.nh_plane('bottom_plane')
bottom_plane:scale(box_width, box_length, 1)
bottom_plane:rotate('X', 90)
bottom_plane:translate(0, -box_height / 2.0 + fudge, -10.0)
scene_root:add_child(bottom_plane)
bottom_plane:set_material(red_checkered_mat)

left_plane = gr.nh_plane('left_plane')
left_plane:scale(box_length, box_height, 1)
left_plane:rotate('Y', 90)
left_plane:translate(-5, 0, -10.0)
scene_root:add_child(left_plane)
left_plane:set_material(red_mat)

right_plane = gr.nh_plane('right_plane')
right_plane:scale(box_length, box_height, 1)
right_plane:rotate('Y', -90)
right_plane:translate(5, 0, -10.0)
scene_root:add_child(right_plane)
right_plane:set_material(green_mat)

spheres = gr.node('spheres')
left_sphere = gr.sphere('left_sphere')
spheres:add_child(left_sphere)
left_sphere:scale(1.2, 1.2, 1.2)
left_sphere:translate(-2.8, -3.8, -14.5)
left_sphere:set_material(refract_mat)

centre_sphere = gr.sphere('centre_sphere')
spheres:add_child(centre_sphere)
centre_sphere:scale(1.2, 1.2, 1.2)
centre_sphere:translate(0, -3.8, -14.5)
centre_sphere:set_material(refract_mat2)

right_sphere = gr.sphere('right_sphere')
spheres:add_child(right_sphere)
right_sphere:scale(1.2, 1.2, 1.2)
right_sphere:translate(2.8, -3.8, -14.5)
right_sphere:set_material(refract_mat3)

scene_root:add_child(spheres)

light = gr.light({0, 4.0, -box_length / 2.0 - 2.0}, {0.5, 0.5, 0.5}, {5, 2, 0})
light_2 = gr.light({-1.0, 2.0, -5}, {255 / 255, 251 / 255, 235 / 255}, {1, 0, 0})

ambient = 0.3

gr.render(scene_root, 'glossy-refraction.png', 500, 500,
	  {0.0, -1.0, 0.0}, {0, -1.5, -box_length / 2.0}, {0, 1, 0}, 40,
	  {ambient, ambient, ambient}, {light_2})
