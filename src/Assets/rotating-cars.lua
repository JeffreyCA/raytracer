white_mat = gr.material({1.0, 1.0, 1.0}, {0.0, 0.0, 0.0}, 1)
dim_white_mat = gr.material({0.8, 0.8, 0.8}, {0.0, 0.0, 0.0}, 1)
red_mat = gr.material({1.0, 0.0, 0.0}, {0.3, 0.3, 0.3}, 1)
blue_mat = gr.material({0.0, 0.0, 1.0}, {0.5, 0.5, 0.5}, 1)
black_mat = gr.material({0.0, 0.0, 0.0}, {1.0, 1.0, 1.0}, 10)
gold_mat = gr.material({0.9, 0.8, 0.4}, {0.8, 0.8, 0.4}, 50)
mirror_mat = gr.mirror_material({0.0, 0.0, 0.0}, {1.0, 1.0, 1.0}, 100, 0)
clear_mat = gr.clear_material({0.0, 0.0, 0.0}, {0.8, 0.8, 0.8}, 1, 0, 1.2)
red_checkered_mat = gr.checker_material({1, 0, 0}, {0, 1, 0}, {0.5, 0.5, 0.5}, 10, 0)
earth_mat = gr.image_material('earth.png', {1, 1}, {0, 0, 0}, 10, false)
stars_mat = gr.image_material('nightsky.png', {1, 1}, {0, 0, 0}, 100, true)
wood_mat = gr.image_material('wood.png', {1, 1}, {0, 0, 0}, 100, false)

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 50)
mat2 = gr.material({0.0, 0.0, 0.0}, {1.0, 1.0, 1.0}, 10)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 50)

scene_root = gr.node('root')

box_height = 10.0
box_width = 10.0
box_length = 20.0

car = gr.texture_mesh('car', 'car.obj')
scene_root:add_child(car)
car:set_material(wood_mat)

light = gr.light({0, 0, 0}, {1, 1, 1}, {0, 0, 0})
ambient = 0.3

for x = 1, 360 do
	car:translate(0, -1, -8)
	gr.render(scene_root, 'car' .. string.format("%03d", x) .. '.png', 500, 500,
		{0.0, 0.0, 0.0}, {0, 0, -box_length / 2.0}, {0, 1, 0}, 50,
		{ambient, ambient, ambient}, {light})
	car:translate(0, 1, 8)
	car:rotate('x', -1)
	car:rotate('y', -1)
end

-- ffmpeg -framerate 60 -i 'car%03d.png' out.gif
