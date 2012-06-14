bl_info = {
    "name": "MRW 7DFPS Level",
    "author": "Willi 'Mr. Wonko' Schinmeyer",
    "version": (0, 0, 2),
    "blender": (2, 6, 3),
    "location": "File > Export > MRW 7DFPS level",
    "description": "Export Mr. Wonko's 7 day FPS level file. Caution: Faces with no material won't be exported! Set game engine physics type to \"No Collision\" for nonsolid objects and disable their rendering for them to be invisible.",
    "category": "Import-Export"}

import bpy, mathutils

VERSION = 2

USE_COLOR_MATERIALS = False
IDENT = "HFPSLVL\0"
if VERSION == 2:
    IDENT = "MRWLVL1\0"
    USE_COLOR_MATERIALS = True

class SurfaceFlags:
    Solid = 1 << 0
    Invisible = 1 << 1
    
USHRT_MAX = 65535

matrixFormat = "[[{mat[0][0]}, {mat[0][1]}, {mat[0][2]}, {mat[0][3]}], "
matrixFormat += "[{mat[1][0]}, {mat[1][1]}, {mat[1][2]}, {mat[1][3]}], "
matrixFormat += "[{mat[2][0]}, {mat[2][1]}, {mat[2][2]}, {mat[2][3]}], "
matrixFormat += "[{mat[3][0]}, {mat[3][1]}, {mat[3][2]}, {mat[3][3]}]]"

entityStart = "{\n"
propertyFormat = "\t\"{}\" \"{}\"\n"
entityEnd = "}\n"

class Entity:
    def __init__(self, errorFunc):
        self.properties = {}
        self.classname = False
        self.reportError = errorFunc
        
    def loadFromObject(self, obj):
        # read user properties
        for key, value in obj.items():
            key = key.lower()
            try:
                value = str(value) #this unfortunately pretty much always works, hard to filter invalid values this way
                if key in {"_rna_ui", "cycles_visibility"}:
                    pass #ignore those
                elif key == "classname":
                    self.classname = value;
                else:
                    self.properties[key] = value
            except:
                pass
        if not self.classname:
            self.reportError("Entity object \"{}\" has no classname property! (This error should be impossible since it should be treated as geometry then!)".format(obj.name))
            return False
        # get transformation matrix
        self.properties["matrix_world"] = matrixFormat.format(mat = obj.matrix_world)
        return True
    
    def saveToFile(self, file):
        file.write(entityStart.encode())
        file.write(propertyFormat.format("classname", self.classname).encode())
        for key, value in self.properties.items():
            file.write(propertyFormat.format(key, value).encode())
        file.write(entityEnd.encode())
        return True

class Vertex:
    # co: mathutils.Vector (3d)
    # uv: pair
    def __init__(self, co, uv):
        self.co = co
        self.uv = uv
    
    def saveToFile(self, file):
        import struct
        file.write(struct.pack("5f", self.co[0], self.co[1], self.co[2], self.uv[0], self.uv[1]))

class Surface:
    # verts: list of Vertex
    # tris: list of vertexIndex (number)
    def __init__(self, verts, tris, material, flags = SurfaceFlags.Solid):
        self.verts = verts
        self.tris = tris
        self.material = material
        self.flags = flags
    
    def saveToFile(self, file):
        import struct
        # if the type of numVertices/numTriangles changes, changes to LevelExporter.readMesh will be required as well (limit check)
        if USE_COLOR_MATERIALS:
            file.write(struct.pack("3BI2H", round(self.material.diffuse_color[0]*255), round(self.material.diffuse_color[1]*255), round(self.material.diffuse_color[2]*255), self.flags, len(self.verts), len(self.tris)))
        else:
            file.write(struct.pack("64sI2H", self.material.name.encode(), self.flags, len(self.verts), len(self.tris)))
        for vert in self.verts:
            vert.saveToFile(file)
        for tri in self.tris:
            file.write(struct.pack("3H", tri[0], tri[1], tri[2]))
        return True

# previous state: object mode enabled, nothing selected
# creates an unhidden duplicate of the supplied object on layer 0
# all the duplicate's modifiers get applied, editmode is enabled and all faces are unhidden and triangulated.
def createSimplifiedCopy(obj):
    # select object, making sure its visible and on the correct layer
    objHidden = obj.hide
    objLayer0 = obj.layers[0]
    obj.hide = False
    obj.layers[0] = True
    obj.select = True
    
    # create duplicate (works by selection (no, does not? I get context errors if I don't set the active object))
    bpy.context.scene.objects.active = obj
    bpy.ops.object.duplicate()
    dupObj = bpy.context.active_object
    
    # restore previous object state
    obj.hide = objHidden
    obj.layers[0] = objLayer0
    
    # apply modifiers
    modifiers = dupObj.modifiers[:]
    for modifier in modifiers:
        bpy.ops.object.modifier_apply(modifier=modifier.name)
    del modifiers
    
    # enter edit mode
    bpy.ops.object.editmode_toggle()
    
    # Enter face selection mode
    bpy.context.tool_settings.mesh_select_mode = (False, False, True)
    
    # unhide all
    bpy.ops.mesh.reveal()
    
    # triangulate
    bpy.ops.mesh.select_all(action='SELECT')
    bpy.ops.mesh.quads_convert_to_tris()
    bpy.ops.mesh.select_all(action='DESELECT')
    
    return dupObj

# previous state: in editmode of mesh object obj with nothing selected
# separates each material into a new object/mesh
# returns created objects
def separateByMaterial(obj): 
    prevObjs = {obj for obj in bpy.context.scene.objects}
    # go through material slots
    # but caution: materials may be used multiple times!
    processedMaterials = set()
    for materialIndex, material in enumerate(obj.material_slots):
        # only process each material once (may be in multiple slots)
        if material not in processedMaterials:
            # select all slots with this material
            for materialIndex2 in range(materialIndex, len(obj.material_slots)):
                material2 = obj.material_slots[materialIndex2]
                if material2 == material:
                    obj.active_material_index = materialIndex
                    bpy.ops.object.material_slot_select()
            # separate these
            bpy.ops.mesh.separate()
            
            processedMaterials.add(material)
    return {obj for obj in bpy.context.scene.objects}.difference(prevObjs)

class LevelExporter:
    def __init__(self, errorFunc):
        self.reportError = errorFunc
        self.invNonsolidWarned = set() # objects that are both invisible and nonsolid - so we only warn once
        
    def export(self, filename):
        # We work on layer 0
        prevLayer0State = bpy.context.scene.layers[0]
        # read objects
        if not self.readObjects():
            bpy.context.scene.layers[0] = prevLayer0State
            return
        bpy.context.scene.layers[0] = prevLayer0State
        # save to file
        with open(filename, "wb") as file:
            self.saveToFile(file)
            file.close()
            return
        self.reportError("Could not open \"{}\" for writing!".format(filename))
        
    def readObjects(self):
        self.entities = []
        self.surfaces = []
        # go through all objects in the scene
        for obj in bpy.context.scene.objects:
            # objects with a custom property "classname" are entities
            if "classname" in obj:
                if not self.readEntity(obj):
                    return False
            # other mesh entities are geometry
            elif obj.type == 'MESH':
                if not self.readGeometry(obj):
                    return False
        return True
    
    def readEntity(self, obj):
        ent = Entity(self.reportError)
        if not ent.loadFromObject(obj):
            return False;
        self.entities.append(ent)
        return True
        
    def readGeometry(self, obj):
        if bpy.context.mode != 'OBJECT':
            self.reportError("Must be in Object Mode to export!")
            return False
        
        print("Processing geometry object \"{}\"...".format(obj.name))
            
        bpy.ops.object.select_all(action='DESELECT')
        
        dupObj = createSimplifiedCopy(obj)
        
        # we're now in editmode of a triangulated copy of the object
        
        # we're about to create lots of objects by splitting by material - we won't be able to tell which are new unless we remember the previous ones.
        
        newObjs = separateByMaterial(dupObj)
        
        # delete object
        bpy.ops.object.editmode_toggle()
        bpy.ops.object.delete()
        
        # process new material separated objects (newObjs)
        success = True # we'll process all objects even in case of failure so no separate deletion is necessary
        for newObj in newObjs:
            mesh = newObj.data
            # only export meshes with faces
            if len(mesh.polygons) > 0:
                # all faces got the same material - but which one?
                material = newObj.material_slots[mesh.polygons[0].material_index].material
                if not self.readMesh(obj, mesh, material): #using the original object here so we have access to the original name
                    success = False
            # okay, we're done - delete the object
            newObj.select = True
            bpy.context.scene.objects.active = newObj
            bpy.ops.object.delete()
        
        return success
    
    def readMesh(self, obj, mesh, material):
        # make sure there's a uv map
        if len(mesh.uv_layers) == 0:
            self.reportError("Mesh of object \"{}\" has no UV map!".format(mesh.name))
            return False
        uv_layer = mesh.uv_layers[0]
        uv_loops = uv_layer.data
        if len(uv_loops) == 0:
            self.reportError("Mesh of object \"{}\" has no UV map!".format(mesh.name))
            return False
        
        # a single vertex may have multiple uv coordinates (since they're per face)
        # I need to export that as multiple vertices, so I use this:
        # { blender vertex index : [ blender uv loop index ] }
        exportIndexLookup = {}
        verts = []
        # also adds the vertex to verts, if necessary
        def getExportIndex(vertexIndex, loopIndex):
            uv = uv_loops[loopIndex].uv.to_tuple()
            indices = exportIndexLookup.get(vertexIndex)
            if not indices:
                exportIndexLookup[vertexIndex] = indices = {}
            index = indices.get(uv)
            if not index:
                verts.append(Vertex(mesh.vertices[vertexIndex].co, uv))
                index = len(verts) - 1
                indices[uv] = index
            return index
        
        # the polys may get new vertex indices due to vertex splitting due to different UVs - this is all done by this beautiful list comprehension.
        tris = [ [ getExportIndex(vertIndex, loopIndex) for vertIndex, loopIndex in zip(poly.vertices, poly.loop_indices) ] for poly in mesh.polygons ]
        
        # make sure we're not exceeding limits
        if len(verts) > USHRT_MAX or len(tris) > USHRT_MAX:
            reportError("After applying modifiers, triangulating and splitting at UV seams, the mesh of \"{}\" has more than 65535 triangles or vertices of the same material!".format(obj.name))
            return False
        
        # generate surface flags based on object settings
        flags = 0
        if obj.game.physics_type != 'NO_COLLISION':
            flags |= SurfaceFlags.Solid
        if obj.hide_render:
            flags |= SurfaceFlags.Invisible
        
        # warn about nonsensical invisible & nonsolid combination - but only once per object
        if flags & SurfaceFlags.Invisible and ~flags & SurfaceFlags.Solid:
            if obj.name not in self.invNonsolidWarned:
                self.invNonsolidWarned.add(obj.name)
                print("Warning: invisible nonsolid object \"{}\"!".format(obj.name))
        
        # append surface to list of surfaces
        self.surfaces.append(Surface(verts, tris, material, flags))
        
        return True
    
    def saveToFile(self, file):
        # save header
        file.write(IDENT.encode())
        import struct
        file.write(struct.pack("2I", VERSION, len(self.surfaces)))
        # save surfaces
        for surface in self.surfaces:
            if not surface.saveToFile(file):
                return False
        # save entities
        for ent in self.entities:
            if not ent.saveToFile(file):
                return False
        return True

class HFPSLVLExport(bpy.types.Operator):
    """Export to the HydraFPS Level file format"""

    bl_idname = "export_scene.hfps_lvl"
    bl_label = "Export MRW 7DFPS Level"

    filepath = bpy.props.StringProperty(subtype='FILE_PATH')

    def execute(self, context):
        #Append .hlvl
        filepath = bpy.path.ensure_ext(self.filepath, ".hlvl")
        
        def report_error(msg):
            self.report({'ERROR'}, msg)
        
        #ctx = bpy.context
        #bpy.context = context
        exporter = LevelExporter(report_error)
        exporter.export(filepath)
        #bpy.context = ctx
        
        return {"FINISHED"}

    def invoke(self, context, event):
        if not self.filepath:
            self.filepath = bpy.path.ensure_ext(bpy.data.filepath, ".hlvl")
        WindowManager = context.window_manager
        WindowManager.fileselect_add(self)
        return {"RUNNING_MODAL"}

def menu_func(self, context):
    self.layout.operator(HFPSLVLExport.bl_idname, text="HydraFPS Level")


def register():
    bpy.utils.register_module(__name__)

    bpy.types.INFO_MT_file_export.append(menu_func)


def unregister():
    bpy.utils.unregister_module(__name__)

    bpy.types.INFO_MT_file_export.remove(menu_func)


if __name__ == "__main__":
    register()
