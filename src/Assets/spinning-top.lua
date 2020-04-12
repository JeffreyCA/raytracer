white_mat = gr.material({1.0, 1.0, 1.0}, {0.0, 0.0, 0.0}, 1)
dim_white_mat = gr.material({0.8, 0.8, 0.8}, {0.0, 0.0, 0.0}, 1)
red_mat = gr.material({1.0, 0.0, 0.0}, {0.5, 0.5, 0.5}, 1)
blue_mat = gr.material({0.0, 0.0, 1.0}, {0.5, 0.5, 0.5}, 1)
black_mat = gr.material({0.0, 0.0, 0.0}, {1.0, 1.0, 1.0}, 10)
gold_mat = gr.material({0.9, 0.8, 0.4}, {0.8, 0.8, 0.4}, 50)
mirror_mat = gr.mirror_material({0.0, 0.0, 0.0}, {1.0, 1.0, 1.0}, 100, 0)
clear_mat = gr.clear_material({0.0, 0.0, 0.0}, {0.8, 0.8, 0.8}, 100, 0, 1.0)
grass_mat = gr.image_material('grass.png', {1, 1}, {0, 0, 0}, 0, false, false)
brick_mat = gr.image_material('brick.png', {1, 1}, {0, 0, 0}, 100, false, false)
earth_mat = gr.image_material('earth.png', {1, 1}, {0, 0, 0}, 100, false, false)
wood_mat = gr.image_material('wood.png', {1, 1}, {0.3, 0.3, 0.3}, 100, false, false)

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 50)
mat2 = gr.material({0.0, 0.0, 0.0}, {1.0, 1.0, 1.0}, 10)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 50)

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
bottom_plane:set_material(white_mat)

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
right_plane:set_material(blue_mat)

spinning_top = gr.node('top')
cone = gr.cone('c', 5)
cone:rotate('y', 180)
cone:rotate('x', -90)
spinning_top:add_child(cone)
cone:set_material(wood_mat)

cylinder = gr.cylinder('cylinder', 0.8, 4)
cylinder:translate(0, 0, -6)
cylinder:rotate('y', 180)
cylinder:rotate('x', -90)
spinning_top:add_child(cylinder)
cylinder:set_material(wood_mat)

scene_root:add_child(spinning_top)

light_1 = gr.light({0.0, -4.0, -3}, {0.7, 0.7, 0.7}, {2, 2, 0})
light_2 = gr.light({0.0, 2.5, -10}, {255 / 255, 251 / 255, 235 / 255}, {1, 0, 0})
ambient = 0.3

-- z: -12.5 to -16
-- x: -2 to 2
-- y: -5
for i = 1, 90 do
	x = 0.0 + math.cos(4 * math.rad(i)) * 2.0;
	y = -14.25 + math.sin(4 * math.rad(i)) * 1.75;
	spinning_top:rotate('x', -10)
	spinning_top:rotate('y', -(4 * math.rad(i) * 57 - 90))
	spinning_top:scale(0.3, 0.3, 0.4)
	spinning_top:translate(x, -4.7, y)
	gr.render(scene_root, 'top' .. string.format("%03d", i) .. '.png', 500, 500,
		{0.0, 0.0, 0.0}, {0, 0, -box_length / 2.0}, {0, 1, 0}, 50,
		{ambient, ambient, ambient}, {light_1, light_2})
	spinning_top:translate(-x, 4.7, -y)
	spinning_top:scale(1.0/0.3, 1.0/0.3, 1.0/0.4)
	spinning_top:rotate('y', (4 * math.rad(i) * 57 - 90))
	spinning_top:rotate('x', 10)
end
