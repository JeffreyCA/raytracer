white_mat = gr.material({1.0, 1.0, 1.0}, {0.0, 0.0, 0.0}, 1)
dim_white_mat = gr.material({0.8, 0.8, 0.8}, {0.0, 0.0, 0.0}, 1)
red_mat = gr.material({1.0, 0.0, 0.0}, {0.3, 0.3, 0.3}, 1)
blue_mat = gr.material({0.0, 0.0, 1.0}, {0.5, 0.5, 0.5}, 1)
black_mat = gr.material({0.0, 0.0, 0.0}, {1.0, 1.0, 1.0}, 10)
gold_mat = gr.material({0.9, 0.8, 0.4}, {0.8, 0.8, 0.4}, 50)
mirror_mat = gr.mirror_material({0.0, 0.0, 0.0}, {1.0, 1.0, 1.0}, 100, 0)
clear_mat = gr.clear_material({0.0, 0.0, 0.0}, {0.8, 0.8, 0.8}, 1, 0, 1.0)
translucent_mat = gr.clear_material({0.0, 0.0, 0.0}, {1, 1, 1}, 1, 0, 1.3)
red_checkered_mat = gr.checker_material({1, 0, 0}, {0, 1, 0}, {0.5, 0.5, 0.5}, 10, 0)
earth_mat = gr.image_material('earth.png', {1, 1}, {0, 0, 0}, 10, false)
stars_mat = gr.image_material('nightsky.png', {1, 1}, {0, 0, 0}, 100, true)

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 50)
mat2 = gr.material({0.0, 0.0, 0.0}, {1.0, 1.0, 1.0}, 10)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 50)

scene_root = gr.node('root')

box_height = 10.0
box_width = 10.0
box_length = 20.0

sphere = gr.sphere('sphere')
sphere:translate(0, 0, -5)
scene_root:add_child(sphere)
sphere:set_material(translucent_mat)

sphere2 = gr.sphere('sphere2')
sphere2:scale(30, 30, 30)
--sphere:rotate('x', 180)
sphere2:translate(0, 0, -100)
scene_root:add_child(sphere2)
sphere2:set_material(earth_mat)

light = gr.light({0, 0, 0}, {1, 1, 1}, {0, 0, 0})

ambient = 0.3

gr.render(scene_root, 'sphere.png', 500, 500,
	  {0.0, 0.0, 0.0}, {0, 0, -box_length / 2.0}, {0, 1, 0}, 50,
	  {ambient, ambient, ambient}, {light})

--[[
for x = 1, 360 do
	cone:translate(0, 0, -50)
	gr.render(scene_root, 'cone' .. string.format("%03d", x) .. '.png', 500, 500,
	  {0.0, 0.0, 0.0}, {0, 0, -box_length / 2.0}, {0, 1, 0}, 50,
	  {ambient, ambient, ambient}, {light})
	cone:translate(0, 0, 50)
	cone:rotate('x', -5)
	cone:rotate('y', -2)
	cone:rotate('z', -2)
end
--]]

-- ffmpeg -framerate 60 -i 'sphere%03d.png' out.gif