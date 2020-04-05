white_mat = gr.material({1.0, 1.0, 1.0}, {0.0, 0.0, 0.0}, 1)
dim_white_mat = gr.material({0.8, 0.8, 0.8}, {0.0, 0.0, 0.0}, 1)
red_mat = gr.material({1.0, 0.0, 0.0}, {0.3, 0.3, 0.3}, 1)
blue_mat = gr.material({0.0, 0.0, 1.0}, {0.5, 0.5, 0.5}, 1)
black_mat = gr.material({0.0, 0.0, 0.0}, {1.0, 1.0, 1.0}, 10)
gold_mat = gr.material({0.9, 0.8, 0.4}, {0.8, 0.8, 0.4}, 50)
mirror_mat = gr.mirror_material({0.0, 0.0, 0.0}, {1.0, 1.0, 1.0}, 100, 0)
clear_mat = gr.clear_material({0.0, 0.0, 0.0}, {0.8, 0.8, 0.8}, 1, 0, 1.2)
red_checkered_mat = gr.checker_material({1, 0, 0}, {0, 1, 0}, {0.5, 0.5, 0.5}, 10, 0)

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 50)
mat2 = gr.material({0.0, 0.0, 0.0}, {1.0, 1.0, 1.0}, 10)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 50)

scene_root = gr.node('root')

box_height = 10.0
box_width = 10.0
box_length = 20.0

sphere = gr.nh_sphere('sphere', {0, 0, 0}, 10)
scene_root:add_child(sphere)
sphere:set_material(red_checkered_mat)

light = gr.light({-1.0, 2.0, -5}, {100 / 255, 100 / 255, 100 / 255}, {1, 0, 0})

ambient = 0.3

for x = 1, 90 do
	sphere:translate(0, 0, -50)
	gr.render(scene_root, 'sphere' .. string.format("%03d", x) .. '.png', 500, 500,
	  {0.0, 0.0, 0.0}, {0, 0, -box_length / 2.0}, {0, 1, 0}, 50,
	  {ambient, ambient, ambient}, {light})
      sphere:translate(0, 0, 50)
      sphere:rotate('z', 5)
end

-- ffmpeg -framerate 60 -i 'cow%03d.png' out.gif