import bpy

print("start export_prototype")

for obi, ob in enumerate(bpy.data.objects):
    mat = ob.matrix_world
    for vertexi, vertex in enumerate(ob.data.vertices):
        ob.data.vertices[vertexi].co = mat @ ob.data.vertices[vertexi].co
    bpy.data.objects[obi].matrix_world[0][3] = 0.0
    bpy.data.objects[obi].matrix_world[1][3] = 0.0
    bpy.data.objects[obi].matrix_world[2][3] = 0.0

print("end export_prototype")