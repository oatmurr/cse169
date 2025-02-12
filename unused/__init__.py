# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTIBILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

bl_info = {
    "name" : "CSE169 Utilities",
    "author" : "relubwu",
    "description" : "Conversion utility for UCSD CSE169: Computer Animation",
    "blender" : (3, 0, 0),
    "version" : (0, 0, 1),
    "location" : "",
    "warning" : "",
    "category" : "Generic"
}

import io
import bpy
import bpy_extras
from math import radians
from mathutils import Matrix, Vector, Quaternion

scale = 0.01
shift = Vector((0, 0, -1))
bl_up = Vector((0, 0, 1))
gl_up = Vector((0, 1, 0))

def calcRotMtx(a, b):
    a = Vector((a.x, a.z, -a.y))
    b = Vector((b.x, b.z, -b.y))
    v = a.cross(b)
    s = v.length
    if s == 0:
        return Matrix.Identity(4)
    c = a.dot(b)
    cpMtx = Matrix((
        (0, -v.z, v.y),
        (v.z, 0, -v.x),
        (-v.y, v.x, 0)
    ))
    R = Matrix.Identity(3) + cpMtx + cpMtx @ cpMtx * ((1 - c) / s ** 2)
    return R.to_4x4()

class CSE169_OT_import_mixamo(bpy.types.Operator, bpy_extras.io_utils.ImportHelper):
    bl_label = "Imports Mixamo Rig"
    bl_idname = "import_scene.mixamo"
    
    filename_ext = ".fbx"

    filter_glob: bpy.props.StringProperty(
        default = "*.fbx",
        options = {'HIDDEN'},
        maxlen = 255
    )
    
    def execute(self, context):
        bpy.ops.import_scene.fbx(filepath = self.filepath)
        a = context.view_layer.objects.selected[0]
        b = context.view_layer.objects.selected[1]
        obj = a if a.type == 'MESH' else b
        context.view_layer.objects.active = obj
        bpy.ops.mesh.customdata_custom_splitnormals_clear()
        obj.data.auto_smooth_angle = radians(60)
        obj.modifiers[0].show_viewport = False
        bpy.ops.object.modifier_add(type='TRIANGULATE')
        bpy.ops.object.modifier_apply(modifier=obj.modifiers[1].name, report=True)
        bpy.ops.object.transform_apply(location=False, rotation=True, scale=False, properties=False)
        arm = a if a.type == 'ARMATURE' else b
        context.view_layer.objects.active = arm
        arm.data.pose_position = 'REST'
        bpy.ops.object.transform_apply(location=False, rotation=True, scale=False, properties=False)
        obj['cse169_prep'] = True
        arm['cse169_prep'] = True
        return {'FINISHED'}

class CSE169_OT_generate_skel(bpy.types.Operator, bpy_extras.io_utils.ExportHelper):
    bl_label = "Generate .skel File"
    bl_idname = "export_anim.skel"
    
    filename_ext = ".skel"
    buf = ""
    arm = None
    
    @classmethod
    def poll(cls, context):
        return len(context.view_layer.objects.selected) == 1 and context.view_layer.objects.selected[0].type == 'ARMATURE' and context.view_layer.objects.selected[0]['cse169_prep'] == 1
    
    def ppad(self, depth, text=''):
        for i in range(depth):
            self.buf += '\t'
        self.buf += text + '\n'
    
    def find(self, bone, depth):
        self.ppad(depth, "balljoint " + bone.name[10:] + " {")
        head = scale * bone.head_local + shift
        tail = scale * bone.tail_local + shift
        length = (tail - head).length
        matrix = calcRotMtx(bl_up, (tail - head).normalized())
        pose_bone = self.arm.pose.bones[bone.name]
        pose = pose_bone.rotation_quaternion
        posed_head = scale * pose_bone.head + shift
        posed_tail = scale * pose_bone.tail + shift
        pose_bone.rotation_quaternion = Quaternion()
        depsgraph = bpy.context.evaluated_depsgraph_get()
        pose_bone = self.arm.evaluated_get(depsgraph).pose.bones[bone.name]
        unposed_head = scale * pose_bone.head + shift
        unposed_tail = scale * pose_bone.tail + shift
        pose_bone.rotation_quaternion = pose
        euler = calcRotMtx((unposed_tail - unposed_head).normalized(), (posed_tail - posed_head).normalized()).to_euler()
        if bone.parent is not None:
            head = head - (scale * bone.parent.head_local + shift)
        else:
            matrix = Matrix.Identity(3)
        self.ppad(depth + 1, "offset " + f"{head[0]:.5f}" + ' ' + f"{head[2]:.5f}" + ' ' + f"{-head[1]:.5f}")
        self.ppad(depth + 1, "boxmin -0.01 " + f"{0:.5f}" + " -0.01")
        self.ppad(depth + 1, "boxmax 0.01 " + f"{length:.5f}" + " 0.01")
        self.ppad(depth + 1, "orient " + f"{matrix[0][0]:.5f}" + ' ' + f"{matrix[0][1]:.5f}" + ' ' + f"{matrix[0][2]:.5f}" + ' ' + f"{matrix[1][0]:.5f}" + ' ' + f"{matrix[1][1]:.5f}" + ' ' + f"{matrix[1][2]:.5f}" + ' ' + f"{matrix[2][0]:.5f}" + ' ' + f"{matrix[2][1]:.5f}" + ' '+ f"{matrix[2][2]:.5f}")
        self.ppad(depth + 1, "pose " + f"{euler.x:.5f}" + ' ' + f"{euler.y:.5f}" + ' ' + f"{euler.z:.5f}")
        for child in bone.children:
            self.find(child, depth + 1)
        self.ppad(depth, "}")
    
    def execute(self, context):
        outfile = self.filepath
        self.arm = context.view_layer.objects.selected[0]
        self.buf = ""
        self.find(self.arm.data.bones[0], 0)
        with io.open(outfile, 'w', newline='\r\n') as f:
            f.write(self.buf)
            print('SKEL O/P SAVED TO', outfile)
        return {'FINISHED'}
    
class CSE169_OT_generate_skin(bpy.types.Operator, bpy_extras.io_utils.ExportHelper):
    bl_label = "Generate .skin File"
    bl_idname = "export_anim.skin"
    
    filename_ext = ".skin"
    buf = ""
    bones = []
    
    @classmethod
    def poll(cls, context):
        if len(context.view_layer.objects.selected) != 2:
            return False
        a = context.view_layer.objects.selected[0]
        b = context.view_layer.objects.selected[1]
        types = [a.type, b.type]
        if 'MESH' not in types and 'ARMATURE' not in types:
            return False
        return a['cse169_prep'] == 1 and b['cse169_prep'] == 1
    
    def ppad(self, depth, text=''):
        for i in range(depth):
            self.buf += '\t'
        self.buf += text + '\n'
        
    def find(self, bone):
        self.bones.append(bone.name)
        for child in bone.children:
            self.find(child)
    
    def execute(self, context):
        outfile = self.filepath
        a = context.view_layer.objects.selected[0]
        b = context.view_layer.objects.selected[1]
        obj = a if a.type == 'MESH' else b
        arm = a if a.type == 'ARMATURE' else b
        mesh = obj.data
        positions = []
        normals = []
        skinweights = []
        triangles = []
        bindings = []
        self.bones = []
        LUT = dict()
        self.find(arm.data.bones[0])
        for idx, bone in enumerate(self.bones):
            LUT[bone] = idx
        # 1, 2, 3 - pos, normals, skinweights
        for v in mesh.vertices:
            positions.append((scale * Vector((v.co.x, v.co.z, -v.co.y))) + shift)
            normals.append(Vector((v.normal.x, v.normal.z, -v.normal.y)))
            sw = [len(v.groups)]
            for grp in v.groups:
                sw.append(LUT[obj.vertex_groups[grp.group].name])
                sw.append(grp.weight)
            skinweights.append(sw)
        # 4 - polygons
        for f in mesh.polygons:
            triangles.append([f.vertices[0], f.vertices[1], f.vertices[2]])
        # 5 - binding matrices walk
        for bone in self.bones:
            bindings.append(Matrix.Translation((scale * Vector((arm.data.bones[bone].head_local[0], arm.data.bones[bone].head_local[2], -arm.data.bones[bone].head_local[1]))) + shift))
        # Write File
        wm = context.window_manager
        wm.progress_begin(0, 5)
        self.ppad(0, 'positions ' + str(len(positions)) + ' {')
        for p in positions:
            self.ppad(1, f"{p.x:.5f}" + ' ' + f"{p.y:.5f}" + ' ' + f"{p.z:.5f}")
        self.ppad(0, '}')
        wm.progress_update(1)
        self.ppad(0, 'normals ' + str(len(normals)) + ' {')
        for n in normals:
            self.ppad(1, f"{n.x:.5f}" + ' ' + f"{n.y:.5f}" + ' ' + f"{n.z:.5f}")
        self.ppad(0, '}')
        wm.progress_update(2)
        self.ppad(0, 'skinweights ' + str(len(skinweights)) + ' {')
        for sw in skinweights:
            tmp = str(sw[0])
            grps = sw[1:]
            for i in range(sw[0]):
                tmp += ' ' + str(grps[2 * i]) + ' ' + f"{grps[2 * i + 1]:.5f}"
            self.ppad(1, tmp)
        self.ppad(0, '}')  
        wm.progress_update(3)
        self.ppad(0, 'triangles ' + str(len(triangles)) + ' {')
        for f in triangles:
            self.ppad(1, str(f[0]) + ' ' + str(f[1]) + ' ' + str(f[2]))
        self.ppad(0, '}')  
        wm.progress_update(4)
        self.ppad(0, 'bindings ' + str(len(bindings)) + ' {')
        for b in bindings:
            self.ppad(1, "matrix {")
            self.ppad(2, f"{b[0][0]:.5f}" + ' ' + f"{b[1][0]:.5f}" + ' ' + f"{b[2][0]:.5f}")
            self.ppad(2, f"{b[0][1]:.5f}" + ' ' + f"{b[1][1]:.5f}" + ' ' + f"{b[2][1]:.5f}")
            self.ppad(2, f"{b[0][2]:.5f}" + ' ' + f"{b[1][2]:.5f}" + ' ' + f"{b[2][2]:.5f}")
            self.ppad(2, f"{b[0][3]:.5f}" + ' ' + f"{b[1][3]:.5f}" + ' ' + f"{b[2][3]:.5f}")
            self.ppad(1, "}")
        self.ppad(0, '}')
        wm.progress_update(5)
        with io.open(outfile, 'w', newline='\r\n') as f:
            f.write(self.buf)
            print('SKIN O/P SAVED TO', outfile)
        wm.progress_end()
        return {'FINISHED'}

class CSE169_PT_panel(bpy.types.Panel):
    bl_label = "CSE169 Utils Panel"
    bl_idname = "CSE169_PT_panel"
    bl_space_type = "VIEW_3D"
    bl_region_type = "UI"
    bl_category = "CSE169"
    
    def draw(self, context):
        layout = self.layout
        row = layout.row()
        row.operator('import_scene.mixamo', text = 'Import', icon = 'IMPORT')
        row = layout.row()
        row.operator('export_anim.skel', text = 'Generate .skel', icon = 'OUTLINER_DATA_ARMATURE')
        row = layout.row()
        row.operator('export_anim.skin', text = 'Generate .skin', icon = 'OUTLINER_OB_ARMATURE')

classes = {
    CSE169_OT_import_mixamo,
    CSE169_OT_generate_skel,
    CSE169_OT_generate_skin,
    CSE169_PT_panel
}

cls_register, unregister = bpy.utils.register_classes_factory(classes)

def register():
    cls_register()
    
if __name__ == "__main__":
    register()