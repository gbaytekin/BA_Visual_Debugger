#include "visualDebugger.hh"
#include "OpenFlipper/BasePlugin/PluginFunctions.hh"
#include <ACG/Geometry/Algorithms.hh>
#include <cstdlib>

VisualDebugger::VisualDebugger() :
        triMesh_(0),
	primitivesMenu_(0)
{

}

VisualDebugger::~VisualDebugger()
{
  if ( OpenFlipper::Options::gui()) {
    delete primitivesMenu_;
  }
}

void VisualDebugger::initializePlugin()
{
  emit setSlotDescription("importFaces(Vector)",
                          tr("Imports faces.txt from gdb (ObjectId is returned)"),
                          QString("Position"),
                          QString("Center position"));
}

void VisualDebugger::pluginsInitialized() {

  if ( OpenFlipper::Options::gui()) {

    emit getMenubarMenu(tr("&Primitives"), primitivesMenu_, true );

    QAction* action;

    action = primitivesMenu_->addAction("Import Faces"       ,this,SLOT(importFaces()));
    //action->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"primitive_cube.png"));

    WhatsThisGenerator whatsThisGen("VisualDebugger");
    whatsThisGen.setWhatsThis(action,tr("Import Faces."),"Imported Faces");
  }
}

void VisualDebugger::add_face( int _vh1 , int _vh2, int _vh3 ) {
 std::vector<TriMesh::VertexHandle> vhandles;

 vhandles.push_back(vhandles_[_vh1]);
 vhandles.push_back(vhandles_[_vh2]);
 vhandles.push_back(vhandles_[_vh3]);

 triMesh_->add_face(vhandles);

}

int VisualDebugger::addTriMesh() {
  int objectId = -1;

  emit addEmptyObject( DATA_TRIANGLE_MESH, objectId );

  TriMeshObject* object;
  if ( !PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,"Unable to create new Object");
    return -1;
  }

  return objectId;
}

int VisualDebugger::importFaces(const Vector& _position) {


  int newObject = addTriMesh();

  TriMeshObject* object;
  if ( !PluginFunctions::getObject(newObject,object) ) {
        emit log(LOGERR,"Unable to create new Object");
        return -1;
  } else {

    object->setName( "Imported Faces " + QString::number(newObject) );

    triMesh_ =  object->mesh();

    triMesh_->clear();

    QFile inputFile("/home/gbaytekin/OpenFlipper/build/faces.txt");
    if (inputFile.open(QIODevice::ReadOnly))
    {
	QTextStream in(&inputFile);
	QString line = in.readLine();
	vhandles_.resize(line.toInt());
   	while (!in.atEnd())
   	{
          line = in.readLine();
	  line.remove('[');
	  line.remove(']');
	  line.remove(' ');
	  QStringList list = line.split(',');
	  vhandles_[list[0].toInt()] = triMesh_->add_vertex(TriMesh::Point(list[1].toFloat(),list[2].toFloat(),list[3].toFloat())+_position);
	  vhandles_[list[4].toInt()] = triMesh_->add_vertex(TriMesh::Point(list[5].toFloat(),list[6].toFloat(),list[7].toFloat())+_position);
	  vhandles_[list[8].toInt()] = triMesh_->add_vertex(TriMesh::Point(list[9].toFloat(),list[10].toFloat(),list[11].toFloat())+_position);
	  add_face(list[0].toInt(),list[4].toInt(),list[8].toInt());
   	}
	inputFile.close();
    }

    triMesh_->update_normals();

    emit updatedObject(newObject,UPDATE_ALL);
    emit createBackup(newObject, "Original Object");

    PluginFunctions::viewAll();

    return newObject;
  }

  return -1;
}

Q_EXPORT_PLUGIN2( visualDebugger , VisualDebugger );
