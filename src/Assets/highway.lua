grass_mat = gr.image_material('grass.png', {100, 100}, {0, 0, 0}, 0, false, true)
road_mat = gr.image_material('road.png', {1, 15}, {0, 0, 0}, 100, false, true)
orange_car_mat = gr.image_material('orange_car.png', {1, 1}, {0, 0, 0}, 100, true, false)
police_car_mat = gr.image_material('police_car.png', {1, 1}, {0, 0, 0}, 100, true, false)
barn_mat = gr.image_material('barn.png', {1, 1}, {0, 0, 0}, 100, true, false)
tree_mat = gr.image_material('tree.png', {1, 1}, {0, 0, 0}, 100, true, false)

scene_root = gr.node('root')

box_height = 10.0
box_width = 10.0
box_length = 20.0

bottom_plane = gr.nh_plane('grass')
bottom_plane:scale(500, 800, 1)
bottom_plane:rotate('X', 90)
bottom_plane:translate(0, -5.0, -10.0)
scene_root:add_child(bottom_plane)
bottom_plane:set_material(grass_mat)

road = gr.nh_plane('road')
road:scale(10, 800, 1)
road:rotate('X', 90)
road:translate(0, -4.8, -10.0)
scene_root:add_child(road)
road:set_material(road_mat)

orange_car = gr.texture_mesh('orange_car', 'regular_car.obj')
orange_car:rotate('y', -90)
orange_car:translate(1.2, -3, -10)
scene_root:add_child(orange_car)
orange_car:set_material(orange_car_mat)

police_car = gr.texture_mesh('police_car', 'police_car.obj')
police_car:scale(1.2, 1.2, 1.2)
police_car:rotate('y', 90)
police_car:translate(2, -4, -28)
scene_root:add_child(police_car)
police_car:set_material(police_car_mat)

barn = gr.texture_mesh('barn', 'barn.obj')
barn:scale(2, 2, 2)
barn:rotate('x', -90)
barn:translate(-25, -6.2, -58)
scene_root:add_child(barn)
barn:set_material(barn_mat)

tree = gr.texture_mesh('tree', 'tree.obj')
tree:scale(0.35, 0.35, 0.35)
tree:rotate('x', -90)
tree:translate(9.5, -3.5, -26)
scene_root:add_child(tree)
tree:set_material(tree_mat)

tree2 = gr.texture_mesh('tree', 'tree.obj')
tree2:scale(0.15, 0.15, 0.15)
tree2:rotate('x', -90)
tree2:rotate('y', -45)
tree2:translate(3.5, -1.5, -26)
scene_root:add_child(tree2)
tree2:set_material(tree_mat)

light = gr.light({0, 0, 0}, {1, 1, 1}, {0, 0, 0})
ambient = 0.3

gr.render(scene_root, 'highway.png', 500, 500,
		{0.0, 0.0, 0.0}, {0, 0, -box_length / 2.0}, {0, 1, 0}, 50,
		{ambient, ambient, ambient}, {light})
