white_mat = gr.material({1.0, 1.0, 1.0}, {0.0, 0.0, 0.0}, 1)
dim_white_mat = gr.material({0.8, 0.8, 0.8}, {0.0, 0.0, 0.0}, 1)
red_mat = gr.material({1.0, 0.0, 0.0}, {0.7, 0.7, 0.7}, 100)
blue_mat = gr.material({0.0, 0.0, 1.0}, {0.5, 0.5, 0.5}, 1)
black_mat = gr.material({0.0, 0.0, 0.0}, {1.0, 1.0, 1.0}, 10)
gold_mat = gr.material({0.9, 0.8, 0.4}, {0.8, 0.8, 0.4}, 50)
mirror_mat = gr.mirror_material({0.0, 0.0, 0.0}, {1.0, 1.0, 1.0}, 100, 0)
clear_mat = gr.clear_material({0.0, 0.0, 0.0}, {0.8, 0.8, 0.8}, 10, 0, 1.3)
red_checkered_mat = gr.checker_material({1, 0, 0}, {0, 1, 0}, {0.5, 0.5, 0.5}, 10, 0)
earth_mat = gr.image_material('earth.png', {0, 0, 0}, 10, false)
stars_mat = gr.image_material('nightsky.png', {0, 0, 0}, 100, true)
wood_mat = gr.image_material('wood.png', {0, 0, 0}, 100, true)

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 50)
mat2 = gr.material({0.0, 0.0, 0.0}, {1.0, 1.0, 1.0}, 10)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 50)

scene_root = gr.node('root')

box_height = 10.0
box_width = 10.0
box_length = 20.0

cylinder = gr.cylinder('cylinder', 3, 5)
scene_root:add_child(cylinder)
-- cylinder:rotate('y', 180)
-- cylinder:rotate('x', 180)
-- cylinder:translate(0, 0, -30)
-- cylinder:translate(0, 0, -30)
cylinder:set_material(red_mat)

--[[
cylinder2 = gr.cylinder('cylinder2', 0.25, 9)
scene_root:add_child(cylinder2)
cylinder2:rotate('y', 30)
cylinder2:rotate('x', 90)
cylinder2:translate(0, 0, -20)
cylinder2:set_material(white_mat)
--]]

light = gr.light({5, -2, 0}, {1, 1, 1}, {1, 0, 0})

ambient = 0.3

gr.render(scene_root, 'cylinder.png', 500, 500,
	  {0.0, 0.0, 0.0}, {0, 0, -box_length / 2.0}, {0, 1, 0}, 50,
	  {ambient, ambient, ambient}, {light})


for x = 1, 360 do
	-- cylinder:rotate('z', 90)
	cylinder:translate(0, 0, -20)
	gr.render(scene_root, 'cylinder' .. string.format("%03d", x) .. '.png', 500, 500,
	  {0.0, 0.0, 0.0}, {0, 0, -box_length / 2.0}, {0, 1, 0}, 50,
	  {ambient, ambient, ambient}, {light})
	cylinder:translate(0, 0, 20)
	-- cylinder:rotate('z', -5)
	cylinder:rotate('x', -5)
	-- cylinder:rotate('y', -5)
end


-- ffmpeg -framerate 60 -i 'sphere%03d.png' out.gif