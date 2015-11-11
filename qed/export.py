import bpy
import Blender
from Blender import *
file = 'C:\\Documents and Settings\\oren.fromberg\\Desktop\\output.txt'
f = open(file,'w')

mesh = bpy.data.objects["hellknigh"].getData(mesh=1)

#get verts
for vert in mesh.verts:
	for i in range(3):
		f.write(str(vert.co[i])+'f, ')
f.write("\n\n");

#get indices
for face in mesh.faces:
	for i in range(3):
		f.write(str(face.verts[i].index)+f', ')

#uv coords
uv = {}
for face in mesh.faces:
	for i in range(3):
		uv[face.verts[i].index] = face.uv[i]
for i in range(len(uv)):
	for j in range(2):
		f.write(str(uv[i][j])+'f, ')

#close file
f.close()
