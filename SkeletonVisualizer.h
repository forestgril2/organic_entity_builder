#ifndef SkeletonVisualizer_H
#define SkeletonVisualizer_H

#include <QtGui/QMainWindow>
#include <QtGui/QPainter>
#include <QtGui/QLabel>
#include <math.h>
#include <vector>

typedef class QPointF Point;
class Vector : public QPointF
{
public:
  Vector() : Point() {};
  Vector(const Point& p) : Point(p) {};
  Vector(double x, double y) : Point(x, y) {};
  double length() {return sqrt(x()*x() + y()*y());};
};

struct Bone
{
  Bone() : pos(Vector(0.0, 0.0)), length(0), angle(0) {};
  Bone(Vector P, double l, double a) : pos(P), length(l), angle(a) {};

  void move(Vector v) {pos = pos + v;};
  double setx(double x) {pos.setX(x);};
  double sety(double y) {pos.setY(y);};

  double x() const {return pos.x();};
  double y() const {return pos.y();};
  double endx() const {return pos.x() + cos(angle)*length;}; 
  double endy() const {return pos.y() - sin(angle)*length;};
  Vector endPoint() const {return Vector(endx(), endy());};

  void randPos(double max);
  void randAngle();
  void randLength(double max);

  void paint(QPainter* painter) const;

  Vector pos;
  double length;
  double angle; 
};

class Skeleton
{
  friend class SkeletonVisualizer;

public:
  Bone& lastBone();
  Skeleton(unsigned n, double length) {bones.resize(n); initBones(length);};
  Skeleton() : Skeleton(1, 0.0) {};
  void move(Vector dr);

private:
  void paint(QPainter* painter) const;
  
  void randomizeBones(double length);
  void initBones(double length);
  void addBone(Vector start, Vector end);

  std::vector<Bone> bones;
};

class SkeletonVisualizer : public QMainWindow
{
  Q_OBJECT

public:
  SkeletonVisualizer(Skeleton* s);
  
  virtual void paintEvent(QPaintEvent*);
  virtual ~SkeletonVisualizer();

private:
  void mousePressEvent(QMouseEvent *mouseEvent);
  bool checkSelectDeselectAllVertices(const Vector& mousePt);
  void paintCenterCross(QPainter* painter);
  void paintSelections(QPainter* painter);
  
  void initLabel();
  void initAction();
  Vector skeletonBoneEnd();
    void centerSkeleton();
  bool checkSelectDeselectVertice(const Vector& mousePt, Vector endPoint);
  
  Skeleton* skeleton;
  QLabel* label;

  Point selectedVertice;
  bool isVerticeSelected;
  double w;
  double h;
};

#endif // SkeletonVisualizer_H
