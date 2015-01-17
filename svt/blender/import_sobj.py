#!BPY
 
"""
Name: 'Wavefront / Sculptor (.obj)...'
Blender: 242
Group: 'Import'
Tooltip: 'Load a Sculptor Wavefront OBJ File, Shift: batch import all dir.'
"""

#=========================================================================
# Wavefront OBJ Importer for Sculptor Wavefront files
#=========================================================================

#===============================
# Setup our runtime constants
#===============================

MESHVERSION=3   # If the export file doesn't work,
FILEVERSION=3   # try changing these to "2"

#===============================
# Import libraries
#===============================

try:
    import nt
    os=nt
    os.sep='\\'
except:    
    import posix
    os=posix
    os.sep='/'

def isdir(path):
    try:
        st = os.stat(path)
        return 1 
    except:
        return 0
    
def split(pathname):
         PATHNAME=pathname
         PATHNAME=PATHNAME.replace('\\','/') 
         k0=PATHNAME.split('/')
         directory=pathname.replace(k0[len(k0)-1],'')
         Name=k0[len(k0)-1]
         return directory, Name
        
def join(l0,l1):        
     return  l0+os.sep+l1
    
os.isdir=isdir
os.split=split
os.join=join

import math
import Blender
from Blender import *
from Blender import NMesh
from Blender.Draw import *
from Blender.BGL import *
from Blender import Material
from Blender import Window
import bpy
import BPyMesh
import BPyImage
import BPyMessages

#=========================
def ObjImport(file, Name, filename):
#=========================
    vcount     = 0
    vncount    = 0
    vtcount    = 0
    fcount     = 0
    gcount     = 0
    setcount   = 0
    groupflag  = 0
    objectflag = 0
    mtlflag    = 0
    baseindex  = 0
    basevtcount = 0
    basevncount = 0
    matindex   = 0

    pointList    = []
    uvList       = []
    normalList   = []
    faceList     = []
    materialList = []
    imagelist    = []
    
    uv = [] 
    lines = file.readlines()
    linenumber = 1

    for line in lines:
        words = line.split()
        if words and words[0] == "#":
            pass # ignore comments
        elif words and words[0] == "v":
            vcount = vcount + 1
            
            for n_ in [1,2,3]: 
               if words[n_].find(',')!=-1:
                    words[n_]=words[n_].replace(',','.')
                
            x = float(words[1])
            y = float(words[2])
            z = float(words[3])

            pointList.append([x, y, z])

        elif words and words[0] == "vt":
            vtcount = vtcount + 1
            for n_ in [1,2]: 
               if words[n_].find(',')!=-1:
                    words[n_]=words[n_].replace(',','.')

            u = float(words[1])
            v = float(words[2])
            uvList.append([u, v])

        elif words and words[0] == "vn":
            vncount = vncount + 1

            for n_ in [1,2,3]: 
               if words[n_].find(',')!=-1:
                    words[n_]=words[n_].replace(',','.')

            i = float(words[1])
            j = float(words[2])
            k = float(words[3])
            normalList.append([i, j, k])

        elif words and words[0] == "f":
            fcount = fcount + 1
            vi = [] # vertex  indices
            ti = [] # texture indices
            ni = [] # normal  indices
            words = words[1:]
            lcount = len(words)
            for index in (xrange(lcount)):
               if words[index].find( "/") == -1:
                     vindex = int(words[index])
                     if vindex < 0: vindex = baseindex + vindex + 1  
                     vi.append(vindex)
               else:
                   vtn = words[index].split( "/")
                   vindex = int(vtn[0])
                   if vindex < 0: vindex = baseindex + vindex + 1 
                   vi.append(vindex) 
            
                   if len(vtn) > 1 and vtn[1]:
                      tindex = int(vtn[1])
                      if tindex < 0: tindex = basevtcount +tindex + 1
                      ti.append(tindex)

                   if len(vtn) > 2 and vtn[2]:
                      nindex = int(vtn[2])
                      if nindex < 0: nindex = basevncount +nindex + 1
                      ni.append(nindex)
            faceList.append([vi, ti, ni, matindex])

        elif words and words[0] == "o":
            ObjectName = words[1]
            objectflag = 1
            #print "Name is %s" % ObjectName

        elif words and words[0] == "g":
            groupflag = 1
            index = len(words)
            if objectflag == 0:
               objectflag = 1
               if index > 1:
                  ObjectName = words[1].join("_")
                  GroupName = words[1].join("_") 
               else:
                  ObjectName = "Default" 
                  GroupName = "Default" 
               #print "Object name is %s" % ObjectName
               #print "Group name is %s" % GroupName
            else:
               if index > 1:
                  GroupName = join(words[1],"_") 
               else:
                  GroupName = "Default" 
               #print "Group name is %s" % GroupName
                  
            if mtlflag == 0:
               matindex = AddMeshMaterial(GroupName,materialList, matindex)
            gcount = gcount + 1 
               
            if fcount > 0: 
               baseindex = vcount
               basevncount = vncount
               basevtcount = vtcount

        elif words and words[0] == "mtllib":
            # try to export materials
            directory, dummy = os.split(filename)
            filename = os.join(directory, words[1])
            print  "try to import : ",filename
            
            try:
                file = open(filename, "r")
            except:
                print "no material file %s" % filename
            else:
                mtlflag = 0
                #file = open(filename, "r")
                line = file.readline()
                mtlflag = 1
                wireflag = 0
                while line:
                    words = line.split()
                    if words and words[0] == "newmtl":
                      name = words[1]
                      wireflag = 0
                      line = file.readline()  # Ns ?
                      words = line.split()
                      while words[0] not in ["Ka","Kd","Ks","wire","map_Kd"]:
                          line = file.readline()
                          words = line.split()
                             
                      if words[0] == "Ka":
                        Ka = [float(words[1]),
                              float(words[2]),
                              float(words[3])]
                        line = file.readline()  # Kd
                        words = line.split()
                        
                      if words[0] == "Kd":
                        Kd = [float(words[1]),
                              float(words[2]),
                              float(words[3])]
                        line = file.readline()  # Ks 
                        words = line.split()

                      if words[0] == "Ks":
                        Ks = [float(words[1]),
                              float(words[2]),
                              float(words[3])]
                        line = file.readline()  # Ks 
                        words = line.split()

                      if words[0] == "wire":
                        wireflag = 1
                        line = file.readline()  # wireframe
                        words = line.split()
                        
                      if words[0] == "map_Kd":
                        Kmap= words[1]
                        img=os.join(directory, Kmap)
                        im=Blender.Image.Load(img)
                        line = file.readline()  # Ks 
                        words = line.split()

                      print "new material:", name
                          
                      matindex = AddGlobalMaterial(name, matindex)                            
                      matlist = Material.Get() 
                        
                      if len(matlist) > 0:
                         if name!='defaultMat':
                             material = matlist[matindex]
                             material.R = Kd[0]
                             material.G = Kd[1]
                             material.B = Kd[2]
                             try:
                                  material.specCol[0] = Ks[0]
                                  material.specCol[1] = Ks[1]
                                  material.specCol[2] = Ks[2]
                             except:
                                  pass
                             try:
                                  alpha = 1 - ((Ka[0]+Ka[1]+Ka[2])/3)
                             except:
                                  pass
                             try:
                                  material.alpha = alpha
                             except:
                                  pass

			     if wireflag == 1:
                                  material.mode |= Material.Modes.WIRE

                             try:
                                 
                                 img=os.join(directory, Kmap)
                                 im=Blender.Image.Load(img)
                                 imagelist.append(im)
                             
                                 t=Blender.Texture.New(Kmap)
                                 t.setType('Image')
                                 t.setImage(im)
                             
                                 material.setTexture(0,t)
                                 material.getTextures()[0].texco=16
                             except:
                                  pass
                                
                         else:
                             material = matlist[matindex]
                             
                             material.R = 0.8
                             material.G = 0.8
                             material.B = 0.8
                             material.specCol[0] = 0.5
                             material.specCol[1] = 0.5
                             material.specCol[2] = 0.5

			     if wireflag == 1:
                                  material.mode |= Material.Modes.WIRE
                             
                             img=os.join(directory, Kmap)
                             im=Blender.Image.Load(img)
                             imagelist.append(im)
                             
                             t=Blender.Texture.New(Kmap)
                             t.setType('Image')
                             t.setImage(im)
                             
                             material.setTexture(0,t)
                             material.getTextures()[0].texco=16
                       
                      else:
                         mtlflag = 0
                             
                    line = file.readline()
                          
                        
                file.close()
                 
        elif words and words[0] == "usemtl":
            if mtlflag == 1:
               name = words[1]
               matindex = AddMeshMaterial(name, materialList, matindex) 
        elif words:   
            print "%s: %s" % (linenumber, words)
        linenumber = linenumber + 1
    file.close()

    # import in Blender
 
    print "import into Blender ..."
    mesh   = NMesh.GetRaw ()

    i = 0
    while i < vcount:
      x, y, z = pointList[i] 
      vert=NMesh.Vert(x, y, z)
      mesh.verts.append(vert)
      i=i+1

    if vtcount > 0:
       #mesh.hasFaceUV() = 1
       print ("Object has uv coordinates")
 
    if len(materialList) > 0:
       for m in materialList:
          try:
            M=Material.Get(m)
            mesh.materials.append(M) 
          except:
            pass

    total = len(faceList)
    i = 0

    for f in faceList:
        #if i%1000 == 0:
        #  print ("Progress = "+ str(i)+"/"+ str(total))

        i = i + 1
        vi, ti, ni, matindex = f
        face=NMesh.Face()
        if len(materialList) > 0:
           face.mat = matindex

        limit = len(vi)
        setcount = setcount + len(vi)
        c = 0    
    
        while c < limit:
          m = vi[c]-1
          if vtcount > 0 and len(ti) > c:
             n = ti[c]-1
          if vncount > 0 and len(ni) > c:
             p = ni[c]-1

          if vtcount > 0:
             try:
                  u, v = uvList[n]
             except:
                  pass 

             """ 
        #  multiply uv coordinates by 2 and add 1. Apparently blender uses uv range of 1 to 3 (not 0 to 1). 
             mesh.verts[m].uvco[0] = (u*2)+1
             mesh.verts[m].uvco[1] = (v*2)+1
            """

          if vncount > 0:
             if p > len(normalList):
                print("normal len = " +str(len(normalList))+ " vector len = " +str(len(pointList)))
                print("p = " +str(p))
             x, y, z = normalList[p]  
             mesh.verts[m].no[0] = x
             mesh.verts[m].no[1] = y
             mesh.verts[m].no[2] = z
          c = c+1  
      
        if len(vi) < 5:
          for index in vi:
            face.v.append (mesh.verts[index-1])
  
          if vtcount > 0:  
            for index in ti:
               u, v = uvList[index-1]
               face.uv.append((u,v))
               
            if len(imagelist)>0:
                face.image=imagelist[0]
                #print
                
          if vcount>0:
             face.smooth=1

          mesh.faces.append(face) 

    print "all other (general) polygons ..."
    for f in faceList:
        vi, ti, ni, matindex = f 
        if len(vi) > 4:
            # export the polygon as edges
            print ("Odd face, vertices = "+ str(len(vi)))
            for i in range(len(vi)-2):
               face = NMesh.Face()
               if len(materialList) > 0:
                  face.mat = matindex
               face.v.append(mesh.verts[vi[0]-1])
               face.v.append(mesh.verts[vi[i+1]-1])
               face.v.append(mesh.verts[vi[i+2]-1])

               if vtcount > 0: 
                  if len(ti) > i+2:
                     u, v = uvList[ti[0]-1]
                     face.uv.append((u,v))
                     u, v = uvList[ti[i+1]-1]
                     face.uv.append((u,v))
                     u, v = uvList[ti[i+2]-1]
                     face.uv.append((u,v))

               mesh.faces.append(face)

    mesh.setMaxSmoothAngle( 65 )
    mesh.setMode( "AutoSmooth" )
      
    NMesh.PutRaw(mesh, Name,1)

    print ("Total number of vertices is "+ str(vcount))
    print ("Total number of faces is "+ str(len(faceList)))
    print ("Total number of sets is "+ str(setcount))


    print("Finished importing " +str(Name)+ ".obj")

#=========================================
def AddMeshMaterial(name, materialList, matindex):
#=========================================
    
   index = 0
   found = 0 
   limit = len(materialList)

   while index < limit:
     if materialList[index] == name:
        matindex = index 
        found = 1
        index = limit
     index = index + 1
   
   if found == 0:      
      materialList.append(name)
      matindex = len(materialList)-1
        
   return matindex

#=========================================
def AddGlobalMaterial (name, matindex):
#=========================================
    
   index = 0
   found = 0
   matindex  = 0
   MatList = Material.Get()
   limit = len(MatList)

   while index < limit:
     if MatList[index].name == name:
        matindex = index 
        found = 1
        index = limit
     index = index + 1

   if found == 0:
      material = Material.New(name)
      matindex = index
    
   return matindex

#=========================================
def load_obj_ui(filepath, BATCH_LOAD= False):
#=========================================

   if BPyMessages.Error_NoFile(filepath):
      return
	
   try:
      FILE=open (filepath,"r")
      directory, Name = os.split(filepath)
      print directory, Name
      words = Name.split(".")
      Name = words[0]
      ObjImport(FILE, Name, filepath)
      FILE.close()
      Draw ()
   except IOError:
      Draw ()

#=========================================
def load_obj_ui_batch(file):
#=========================================
   load_obj_ui(file, True)

#=========================================
# Script Main
#=========================================
if __name__=='__main__':
   if os and Window.GetKeyQualifiers() & Window.Qual.SHIFT:
     	Window.FileSelector(load_obj_ui_batch, 'Import OBJ Dir', '')
   else:
       	Window.FileSelector(load_obj_ui, 'Import a Sculptor OBJ', '*.obj')
