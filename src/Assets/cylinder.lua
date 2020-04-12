white_mat = gr.material({1.0, 1.0, 1.0}, {0.0, 0.0, 0.0}, 1)
dim_white_mat = gr.material({0.8, 0.8, 0.8}, {0.0, 0.0, 0.0}, 1)
red_mat = gr.material({1.0, 0.0, 0.0}, {0.7, 0.7, 0.7}, 100)
blue_mat = gr.material({0.0, 0.0, 1.0}, {0.5, 0.5, 0.5}, 1)
black_mat = gr.material({0.0, 0.0, 0.0}, {1.0, 1.0, 1.0}, 10)
gold_mat = gr.material({0.9, 0.8, 0.4}, {0.8, 0.8, 0.4}, 50)
mirror_mat = gr.mirror_material({0.0, 0.0, 0.0}, {1.0, 1.0, 1.0}, 100, 0)
glass_mat = gr.clear_material({0.7, 0.7, 0.7}, {0.5, 0.5, 0.5}, 10000, 0, 1.0)
water_mat = gr.clear_material({186/510, 221/510, 248/510}, {0.6, 0.6, 0.6}, 10000, 0, 1.95)
red_checkered_mat = gr.checker_material({1, 0, 0}, {1, 1, 1}, {0.1, 0.1, 0.1}, 8, 10000)

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 50)
mat2 = gr.material({0.0, 0.0, 0.0}, {1.0, 1.0, 1.0}, 10)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 50)

scene_root = gr.node('root')

box_height = 10.0
box_width = 10.0
box_length = 20.0

plane = gr.nh_plane('black')
plane:scale(500, 500, 1)
plane:translate(0, 0, -50)
scene_root:add_child(plane)
plane:set_material(black_mat)

main = gr.node('main')
glass = gr.cylinder('glass', 2, 5)
main:add_child(glass)
glass:scale(0.2, 0.2, 0.2)
glass:rotate('x', 90)
glass:translate(0, 0, 2)
glass:set_material(glass_mat)

water = gr.cylinder('water', 4, 6)
main:add_child(water)
water:rotate('x', 90)
water:translate(0, -1.88, -20)
water:scale(0.5, 0.5, 0.5)
water:set_material(water_mat)

straw = gr.cylinder('straw', 0.1, 7)
main:add_child(straw)
straw:rotate('x', 90)
straw:rotate('z', 25)
straw:translate(-0.5, 1.2, -10)
straw:set_material(red_checkered_mat)

scene_root:add_child(main)

light = gr.light({5, -2, 0}, {1, 1, 1}, {1, 0, 0})

ambient = 0.3

gr.render(scene_root, 'cylinder.png', 500, 500,
	  {0.0, 0.0, 5.0}, {0, 0, -box_length / 2.0}, {0, 1, 0}, 50,
	  {ambient, ambient, ambient}, {light})
