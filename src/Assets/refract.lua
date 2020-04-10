stone = gr.clear_material({0.7, 0.0, 0.0}, {0.9, 0.9, 0.9}, 100, 0, 1.0)
grass = gr.material({0.86, 0.82, 0.71}, {0.0, 0.0, 0.0}, 0, 0)
blue = gr.material({0.68, 0.91, 0.99}, {0.0, 0.0, 0.0}, 0, 0)

scene = gr.node('scene')

plane = gr.nh_plane('floor')
scene:add_child(plane)
plane:set_material(grass)
plane:rotate('x', -90)
plane:scale(20, 30, 30)
plane:translate(0, 0, 0)

plane2 = gr.nh_plane('wall')
scene:add_child(plane2)
plane2:set_material(blue)
plane2:scale(20, 15, 10)
plane2:translate(0, 7.5, -5)

plane3 = gr.nh_plane('wallr')
scene:add_child(plane3)
plane3:set_material(blue)
plane3:rotate('y', 90)
plane3:scale(30, 15, 20)
plane3:translate(10, 7.5, 5)

plane4 = gr.nh_plane('walll')
scene:add_child(plane4)
plane4:set_material(blue)
plane4:rotate('y', 90)
plane4:scale(20, 15, 20)
plane4:translate(-10, 7.5, 5)

plane5 = gr.nh_plane('wallf')
scene:add_child(plane5)
plane5:set_material(blue)
plane5:rotate('y', -180)
plane5:scale(20, 15, 10)
plane5:translate(0, 7.5, 15)

plane6 = gr.nh_plane('ceiling')
scene:add_child(plane6)
plane6:set_material(blue)
plane6:rotate('x', 90)
plane6:scale(20, 30, 30)
plane6:translate(0, 15, 0)

-- sphere
sphere = gr.sphere('sphere')
--scene:add_child(sphere)
sphere:set_material(stone)
sphere:scale(2.0, 2.0, 2.0)
sphere:translate(2, 2.0, 1.5)

cone = gr.cone('cone', 2)
cone:scale(1, 1, 2)
cone:rotate('y', 70)
cone:translate(2, 2.0, 1.5)
scene:add_child(cone)
cone:set_material(stone)

-- cube
cube = gr.cube('cube')
cube:set_material(stone)
cube:rotate('x', 90)
cube:scale(2, 2, 2)
cube:translate(-3, 1.1, -1)

--scene:rotate('y', 90)
--scene:rotate('x', )

white_light = gr.light({9, 14, 12}, {0.7, 0.7, 0.7}, {1, 0, 0}, 0, 0)

ambient = 0.4
gr.render(scene, 'refract.png', 750, 750,
    {0, 4.5, 12}, {0, 0, -31}, {0, 1, 0}, 50,
    {ambient, ambient, ambient}, {white_light})
