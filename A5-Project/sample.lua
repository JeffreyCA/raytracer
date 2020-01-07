blue = gr.material({0.25, 0.25, 0.97}, {0.7, 0.7, 0.7}, 25)
moo = gr.material({0.5, 0.5, 0.5}, {0.8, 0.8, 0.8}, 45)
white = gr.material({0.75, 0.75, 0.75}, {0.6, 0.6, 0.6}, 30)

scene = gr.node('scene')
scene:translate(-3.5, -2.8, -15)

gap = 0.5

cube_group = gr.node('cube_group')
cube_group:rotate('y', 38.5)
cube_group:rotate('x', 32.5)
scene:add_child(cube_group)

for z = 1, 4 do
    for y = 1, 4 do
        for x = 1, 4 do
            cube = gr.cube('cube' .. tostring(x) .. tostring(y) .. tostring(z))
            cube:translate((x - 1) * (1 + gap), (y - 1) * (1 + gap), (z - 1) * (1 + gap))
            cube:set_material(blue)
            cube_group:add_child(cube)
        end
    end
end

cloud = gr.sphere('cloud')
cloud:scale(1.25, 1, 1)
cloud:translate(1.7, 5.5, 8)
cloud:set_material(white)
scene:add_child(cloud)

cow = gr.mesh('cow', 'cow.obj')
scene:add_child(cow)
cow:scale(0.5, 0.5, 0.5)
cow:rotate('y', -60)
cow:translate(3.8, 5, 5)
cow:set_material(moo)

-- Lights
red_light = gr.light({-100, -50, -15}, {1.0, 0.0, 0}, {1, 0, 0})
yellow_light = gr.light({0, 50, -15}, {1.0, 1.0, 0}, {1, 0, 0})
green_light = gr.light({100, -50, -15}, {0.05, 0.9, 0.05}, {1, 0, 0})

gr.render(scene, 'sample.png', 750, 750, {0, 0, 0}, {0, 0, -1}, {0, 1, 0}, 60, {0.4, 0.4, 0.4}, {red_light, yellow_light, green_light})
