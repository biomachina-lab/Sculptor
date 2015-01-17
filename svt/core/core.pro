##
# SVT - Scientific Visualization Toolkit
# ======================================
#
# --- Core Module ---
#
# Stefan Birmanns 2003
##

!include( $(SVT_DIR)/qmake/main.pri ){ 
	error(Please set the SVT_DIR environment variable!) 
}

TEMPLATE		=	lib
CONFIG			+=	dll console stl 
QT			+=	opengl

SOURCES 		=       src/svt_node.cpp \
				src/svt_node_noautodl.cpp \
				src/svt_rotation.cpp \
				src/svt_sphere.cpp \
				src/svt_box.cpp \
				src/svt_scene.cpp \
				src/svt_optisphere_triangle.cpp \
				src/svt_color.cpp \
				src/svt_scenegraph.cpp \
				src/svt_optisphere.cpp \
				src/svt_fps.cpp \
				src/svt_init.cpp \
				src/svt_properties.cpp \	
				src/svt_cylinder.cpp \
				src/svt_logo.cpp \
				src/svt_texture.cpp \
				src/svt_cone.cpp \
				src/svt_const.cpp \
				src/svt_disc.cpp \
				src/svt_solid_cylinder.cpp \
				src/svt_arrow.cpp \
				src/svt_solid_cone.cpp \
				src/svt_lightsource.cpp \
				src/svt_light.cpp \
				src/svt_rectangle.cpp \
				src/svt_triangle.cpp \
#                               src/glf.c \                                
#        			src/svt_text.cpp \
#        			src/svt_font.cpp \
    				src/svt_line.cpp \
			        src/svt_animation.cpp \
			        src/svt_animation_time.cpp \
			        src/svt_animation_translation.cpp \
			        src/svt_animation_rotation.cpp \
			        src/svt_animation_simultaneous.cpp \
			        src/svt_height_field.cpp \
			        src/svt_animation_visible.cpp \
			        src/svt_animation_stop.cpp \
			        src/svt_animation_pause.cpp \
			        src/svt_animation_switch_scene.cpp \
			        src/svt_float2color.cpp \
 			        src/svt_float2color_bw.cpp \
			        src/svt_float2color_rwb.cpp \
			        src/svt_float2color_ryg.cpp \
			        src/svt_float2color_rgb.cpp \
                                src/svt_float2opacity.cpp \
                                src/svt_float2opacity_linear.cpp \
                                src/svt_float2opacity_constant.cpp \
			        src/svt_mesh.cpp \
			        src/svt_ply.c \
				src/svt_device.cpp \
				src/svt_channel.cpp \
                                src/svt_clipplane.cpp \
                                src/svt_pipette.cpp \
                                src/svt_forceArrows.cpp \
                                src/svt_tool.cpp \
                                src/svt_move_tool.cpp \
                                src/svt_billboard.cpp \
                                src/svt_force_tool.cpp \
                                src/svt_adv_force_tool.cpp \
                                src/svt_adv_force_calc.cpp \
                                src/svt_texture_3d.cpp \
                                src/svt_contextData.cpp \
                                src/svt_displayList.cpp \
                                src/svt_Camera.cpp \
#                                src/svt_sky.cpp \
#                                src/svt_QuadTree.cpp \
#                                src/svt_HeightField.cpp \
                                src/svt_container.cpp \
                                src/svt_extrusion.cpp \
                                src/svt_edge_heap.cpp \
                                src/svt_edge_heap_set.cpp \
                                src/svt_pm.cpp \
                                src/svt_quadric.cpp \
                                src/svt_vertex_array.cpp \
                                src/svt_float2color_points.cpp \
                                src/svt_float2opacity_points.cpp \
                                src/svt_float2mat.cpp \
                                src/svt_float2mat_points.cpp \
                                src/svt_float2mat_linear.cpp \
                                src/svt_aliasobj.cpp \
                                src/svt_palette.cpp \
				src/svt_nvpm.cpp \
                                src/svt_vbo.cpp \
                                src/svt_simple_text.cpp \
                                src/svt_textnode.cpp \
                                src/svt_coord_system.cpp \
                                src/svt_gpgpu.cpp \
                                src/svt_fbo.cpp \
                                src/svt_fbo_ao.cpp


TARGET			=	svt_core
DESTDIR			=	../lib

#
# MinGW
#   This is needed as otherwise linking to standard windows DLLs (like Cg) brings trouble while linking our DLL (cannot export...)
#
win32-g++:QMAKE_LFLAGS_WINDOWS_DLL = -Wl,-subsystem,windows -shared
win32-g++:QMAKE_LFLAGS_CONSOLE_DLL = -Wl,-subsystem,windows -shared
#   This is again because of linking problems with Cg. Only necessary in core, the other modules should only have WIN32 defined.
win32-g++:DEFINES	+=	WIN32_MSVC

#
# Libraries
#
macx-g++:LIBS		+=      -lsvt_basics -lsvt_file_io -lsvt_system -llive -F/Library/Frameworks -framework Cg -lsvt_shader
linux-g++:LIBS     	+=	-lsvt_basics -lsvt_file_io -lsvt_system -llive -lCgGL -lCg -lsvt_shader
win32-msvc:LIBS		+=	svt_basics.lib svt_file_io.lib svt_system.lib live.lib cg.lib cgGL.lib svt_shader
win32-g++:LIBS 		+=	-lsvt_basics -lsvt_file_io -lsvt_system -llive -lCgGL -lCg -lsvt_shader
