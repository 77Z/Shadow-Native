bl_info = {
    "name": "Shadow Engine Mesh Format",
    "description": "Import-Export as Shadow Engine Mesh Format",
    "author": "Vince Richter",
    "version": (1, 0),
    "blender": (4, 1, 1),
    "location": "File > Import-Export",
    "warning": "",
    "wiki_url": "https://shadow.77z.dev",
    "tracker_url": "https://shadow.77z.dev",
    "support": "COMMUNITY",
    "category": "Import-Export"
}

import bpy

def menu_func_export(self, context):
    self.layout.operator("", text="Shadow Engine Mesh (.bin)")

def register():
    from . import properties
    from . import ui
    
    properties.register()
    ui.register()

    bpy.types.TOPBAR_MT_file_export.append(menu_func_export)

def unregister():
    from . import properties
    from . import ui

    properties.unregister()
    ui.unregister()

    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)

if __name__ == "__main__":
    register() 
