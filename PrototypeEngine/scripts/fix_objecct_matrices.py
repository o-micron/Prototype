import bpy
from mathutils import Matrix, Vector

scene = bpy.context.scene

mesh_obs = [o for o in scene.objects if o.type == 'MESH']


for o in mesh_obs:
    me = o.data
    mw = o.matrix_world
    origin = sum((v.co for v in me.vertices), Vector()) / len(me.vertices)

    T = Matrix.Translation(-origin)
    me.transform(T)
    mw.translation = mw @ origin