import re


class LookupMesh (gdb.Command):
	"Takes mesh as input and returns list of faces with coordinates of their vertices"

	@staticmethod 
	def dump_mesh(args):
		print("")
		print("Dumping Vertex Coordinates...")
		num_of_vertices=(int)(gdb.parse_and_eval("%(args)s->n_vertices()" % locals()))
		num_of_vert_coords= num_of_vertices*3
		vert_coords_address=str(gdb.parse_and_eval("&(('OpenMesh::PropertyT<OpenMesh::VectorT<double,3>>'*)(%(args)s->vprops_.properties_[%(args)s->points_.idx_]))->data_[0]" % locals()))
		vert_coords_dump=gdb.execute("x/%(num_of_vert_coords)ifg %(vert_coords_address)s" % locals(), False, True)
		vert_coords_list=re.findall(r'\S+', re.sub('.*?:','',vert_coords_dump))
		num_of_vert_coords_dumped=len(vert_coords_list)
		print(" | Done. Number of coordinates dumped: %(num_of_vert_coords_dumped)i" % locals())

		print("")

		print("Dumping Face Halfedge Handles...")
		num_of_faces=(int)(gdb.parse_and_eval("%(args)s->n_faces()" % locals()))
		face_handles_address=str(gdb.parse_and_eval("&(%(args)s->faces_[0])" % locals()))
		face_handles_dump=gdb.execute("x/%(num_of_faces)iuw %(face_handles_address)s" % locals(), False, True)
		face_handles_list=re.findall(r'\S+', re.sub('.*?:','',face_handles_dump))
		num_of_face_handles_dumped=len(face_handles_list)
		print(" | Done. Number of face halfedge handles dumped: %(num_of_face_handles_dumped)i" % locals())

		print("")

		print("Dumping Edge Handles...")
		num_of_edges=(int)(gdb.parse_and_eval("%(args)s->n_edges()" % locals()))
		#print("Number of edges: %(num_of_edges)i" % locals())
		num_of_edge_handles=num_of_edges*8
		edge_handles_address=str(gdb.parse_and_eval("&(%(args)s->edges_[0]->halfedges_[0])" % locals()))
		edge_handles_dump=gdb.execute("x/%(num_of_edge_handles)iuw %(edge_handles_address)s" % locals(), False, True)
		edge_handles_list=re.findall(r'\S+', re.sub('.*?:','',edge_handles_dump))
		num_of_edge_handles_dumped=len(edge_handles_list)
		print(" | Done. Number of edge handles dumped: %(num_of_edge_handles_dumped)i" % locals())

		print("")

		print("Filtering relevant information from the edge handles dump...")
		#1st handle= face, 2nd handle=vertex, 3rd handle= next halfedge, 4th handle= prev halfedge
		edge_vertex_handles_list=edge_handles_list[1::4]
		num_of_edge_vertex_handles=len(edge_vertex_handles_list)
		edge_next_handles_list=edge_handles_list[2::4]
		num_of_edge_next_handles=len(edge_next_handles_list)
		print(" | Done. %(num_of_edge_vertex_handles)i Halfedge Vertex Handles and %(num_of_edge_next_handles)i Next Halfedge Handles" % locals())

		print("")

		dump =  {
			'num_of_faces':num_of_faces,
			'vert_coords_list':vert_coords_list,
			'face_handles_list':face_handles_list,
			'edge_vertex_handles_list':edge_vertex_handles_list,
			'edge_next_handles_list':edge_next_handles_list
			}

		print("Dump completed.")
		print("")
		print("")
		return dump

	def __init__(self):
		super(LookupMesh, self).__init__("lookup-mesh", gdb.COMMAND_OBSCURE)

	def invoke(self, args, from_tty):

		dump = self.dump_mesh(args)

		print("Looking up faces and their vertex coordinates...")
		faces=[]

		for x in range(0,dump['num_of_faces']):
			vertices=[]
			edge_handle=dump['face_handles_list'][x]
			vertex_handle=dump['edge_vertex_handles_list'][int(edge_handle)]

			vertex=[]
			for z in range(0,3):
				vertex.append(float(dump['vert_coords_list'][int(vertex_handle)*3+z]))
				#vertex.append(float("{0:.2f}".format(float(dump['vert_coords_list'][int(vertex_handle)*3+z]))))

			while (vertex not in vertices):
				vertices.append(vertex)
				edge_handle=dump['edge_next_handles_list'][int(edge_handle)]
				vertex_handle=dump['edge_vertex_handles_list'][int(edge_handle)]
				vertex=[]
				for z in range(0,3):
					vertex.append(float(dump['vert_coords_list'][int(vertex_handle)*3+z]))
					#vertex.append(float("{0:.2f}".format(float(dump['vert_coords_list'][int(vertex_handle)*3+z]))))

			faces.append(vertices)
			#print("faces[" + str(x) + "] = " + str(faces[x]))

		myfile = open('faces.txt', 'w')
		for item in faces:
			print(item, file=myfile)
		print(" | Faces printed to faces.txt")
		print("")

LookupMesh()
