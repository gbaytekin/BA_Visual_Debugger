#ifndef VISUALDEBUGGER_HH_INCLUDED
#define VISUALDEBUGGER_HH_INCLUDED
#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/common/Types.hh>
#include <OpenFlipper/BasePlugin/BackupInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include <OpenFlipper/BasePlugin/MenuInterface.hh>

#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

class VisualDebugger : public QObject, BaseInterface, BackupInterface, LoggingInterface, LoadSaveInterface, MenuInterface
{
  Q_OBJECT
  Q_INTERFACES(BaseInterface)
  Q_INTERFACES(BackupInterface)
  Q_INTERFACES(MenuInterface)
  Q_INTERFACES(LoggingInterface)
  Q_INTERFACES(LoadSaveInterface)

signals:

  //BaseInterface
  void updateView();
  void updatedObject(int _id, const UpdateType& _type);
  void setSlotDescription(QString     _slotName,   QString     _slotDescription,
                          QString _parameter, QString _description);

  // BackupInterface
  void createBackup( int _objectid, QString _name, UpdateType _type = UPDATE_ALL);

  //LoggingInterface
  void log(Logtype _type, QString _message);
  void log(QString _message);

  // Menu Interface
  void getMenubarMenu (QString _name, QMenu *& _menu, bool _create);

  // LoadSaveInterface
  void addEmptyObject( DataType _type, int& _id);

public :
  
  VisualDebugger();
  ~VisualDebugger();
  
  QString name() { return QString("VisualDebugger"); };
  
  QString description() { return QString("Imports faces.txt from gdb"); };

private slots:

  /// BaseInterface
  void initializePlugin();
  void pluginsInitialized();

  /// Tell system that this plugin runs without ui
  void noguiSupported( ) {} ;

public slots:

  QString version() { return QString("1.0"); };
  int importFaces(const Vector& _position = Vector(0.0,0.0,0.0));

private:
  int addTriMesh();
  inline void add_face( int _vh1 , int _vh2, int _vh3 );
  std::vector<TriMesh::VertexHandle> vhandles_;

  TriMesh*  triMesh_;

  QMenu* primitivesMenu_;
  
};
#endif
