red_checkered_mat = gr.checker_material({1, 0, 0}, {1, 1, 1}, {0, 0, 0}, 10, 0)
brick_mat = gr.image_material('brick.png', {1, 1}, {0, 0, 0}, 100, false, false)
basketball_mat = gr.image_material('basketball.png', {1, 1}, {0, 0, 0}, 100, false, false)
waffle_mat = gr.image_material('waffle.png', {1, 1}, {0, 0, 0}, 100, false, false)
vanilla_mat = gr.image_material('vanilla.png', {1, 1}, {0, 0, 0}, 100, false, false)
soup_mat = gr.image_material('campbells.png', {1, 1}, {0, 0, 0}, 100, false, false)

scene_root = gr.node('root')

box_height = 10.0
box_width = 10.0
box_length = 20.0
fudge = 0.1

back_plane = gr.nh_plane('back_plane')
back_plane:scale(box_width, box_height, 1)
back_plane:translate(0, 0, -box_length)
scene_root:add_child(back_plane)
back_plane:set_material(brick_mat)

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
left_plane:set_material(brick_mat)

right_plane = gr.nh_plane('right_plane')
right_plane:scale(box_length, box_height, 1)
right_plane:rotate('Y', -90)
right_plane:translate(5, 0, -10.0)
scene_root:add_child(right_plane)
right_plane:set_material(brick_mat)

basketball = gr.sphere('basketball')
scene_root:add_child(basketball)
basketball:scale(1.2, 1.2, 1.2)
basketball:rotate('x', -20)
basketball:rotate('y', -20)
basketball:translate(-3.0, -3.6, -16.0)
basketball:set_material(basketball_mat)

ice_cream = gr.node('ice_cream')

cone = gr.cone('cone', 3)
ice_cream:add_child(cone)
cone:rotate('z', 180)
cone:rotate('x', 90)
cone:scale(0.2, 0.6, 0.2)
cone:translate(1.05, -3, -6.0)
cone:set_material(waffle_mat)

cream = gr.sphere('cream')
ice_cream:add_child(cream)
cream:scale(0.6, 0.6, 0.6)
cream:rotate('x', -20)
cream:rotate('y', -20)
cream:translate(1.08, -1.0, -6.1)
cream:set_material(vanilla_mat)

ice_cream:translate(0.5, 0, -0.5)
scene_root:add_child(ice_cream)

soup = gr.cylinder('soup', 1.0, 2.5)
scene_root:add_child(soup)
soup:scale(0.7, 0.7, 0.7)
soup:rotate('x', 90)
soup:rotate('y', -96)
soup:translate(0, -3.8, -11.2)
soup:set_material(soup_mat)

light = gr.light({0, 4.0, -box_length / 2.0 - 2.0}, {0.5, 0.5, 0.5}, {5, 2, 0})
light_2 = gr.light({-2, 0.0, -1}, {255 / 255, 251 / 255, 235 / 255}, {1, 0, 0})

ambient = 0.4

gr.render(scene_root, 'wall-ball.png', 500, 500,
	  {0.0, 0.0, 0.0}, {0, 0, -box_length / 2.0}, {0, 1, 0}, 50,
	  {ambient, ambient, ambient}, {light_2})
