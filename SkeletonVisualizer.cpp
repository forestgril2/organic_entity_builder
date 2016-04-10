#include "SkeletonVisualizer.h"
#include <vector>
#include <algorithm>

#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QAction>
#include <QMouseEvent>


#define PI 3.1415926535897932384626433832795028841971694
#define SELRAD 5 //selection radius

using namespace std;

typedef QRectF Rectangle;

double randd(double max)
{
    return ((double)rand()/(double)RAND_MAX)*max;
};


SkeletonVisualizer::SkeletonVisualizer(Skeleton* s) : skeleton(s),
    isVerticeSelected(false), w(500), h(500)
{
    setGeometry(200, 200, w, h);
    centerSkeleton();

    initLabel();
    //initAction();
}

void SkeletonVisualizer::centerSkeleton()
{
    skeleton->move(Vector(w/2,h/2));
}

void SkeletonVisualizer::initAction()
{
    QAction* action = new QAction(this);
    action->setText( "Quit" );
    connect(action, SIGNAL(triggered()), SLOT(close()) );
    menuBar()->addMenu( "File" )->addAction( action );
}

void SkeletonVisualizer::initLabel()
{
    label = new QLabel( this );
    label->setText( "SKELETON" );
    label->move(250, 20);
}

void Skeleton::initBones(double length)
{
    if (bones.size() < 1) bones.resize(1);
    bones[0] = Bone(Vector(0, 0), 20, 0);
    randomizeBones(length);
}

void Skeleton::move(Vector dr)
{
    for(Bone& b : bones) b.move(dr);
}

void Skeleton::randomizeBones(double length)
{
    for (auto b = 1; b < bones.size(); b++)
    {
        bones[b] = Bone(Vector(bones[b -1].endx(), bones[b -1].endy()),  0, 0);
        bones[b].randLength(length);
        bones[b].randAngle();
    }
}

SkeletonVisualizer::~SkeletonVisualizer()
{}

void SkeletonVisualizer::mousePressEvent(QMouseEvent *mouseEvent)
{
    Vector pt = QPointF(mouseEvent -> pos());

    if (mouseEvent->button() == Qt::LeftButton && isVerticeSelected)
    {
        Vector firstVertice = selectedVertice;
        if (checkSelectDeselectAllVertices(Vector(pt)))
        {
            if (isVerticeSelected) skeleton->addBone(firstVertice, selectedVertice);
            else skeleton->addBone(firstVertice, pt);
        }
        else skeleton->addBone(firstVertice, pt);
        isVerticeSelected = false;
    }
    else if (mouseEvent->button() == Qt::RightButton)
    {
        checkSelectDeselectAllVertices(pt);
    }

    this->repaint();
}
void Skeleton::addBone(Vector start, Vector end)
{
    Vector dr;
    Bone newBone;

    newBone.pos = start;

    dr = end - newBone.pos;

    newBone.length = dr.length();
    newBone.angle = atan2(-dr.y(), dr.x());

    bones.push_back(newBone);
}

Bone& Skeleton::lastBone()
{
    return bones.back();
}

Vector SkeletonVisualizer::skeletonBoneEnd()
{
    return (skeleton->lastBone()).endPoint();
}

bool SkeletonVisualizer::checkSelectDeselectAllVertices(const Vector& mousePt)
{   // return value informs, whether a point was selected
    for (Bone b : skeleton->bones)
    {
        if (checkSelectDeselectVertice(mousePt, b.endPoint())) return true;
        if (checkSelectDeselectVertice(mousePt, b.pos)) return true;
    }
    return false;
}

bool SkeletonVisualizer::checkSelectDeselectVertice(const Vector&
        mousePt, Vector endPoint)
{
    double dist = Vector(mousePt - endPoint).length();
    if (dist <= SELRAD)
    {
        if (isVerticeSelected)
        {
            if (Vector(selectedVertice) == endPoint) isVerticeSelected = false;
            else selectedVertice = endPoint;
        }
        else
        {
            selectedVertice = endPoint;
            isVerticeSelected = true;
        }
        return true;
    }
    return false;
}

void Body::paint(QPainter* painter)
{
  auto f = [](double x){return 5;};
  
  if (bone.length == 0) bone.length = 0.01; 
  
  auto posCordCart = Point(0.0, 0.0);
  auto targetCordCart = Point(0.0, 0.0);
  auto posCordFunc = Vector(0.0,f(0.0));
  auto dPos = Vector(0.0, 0.0);
  auto boneStart = bone.startPoint();
  auto boneEnd = bone.endPoint();

  //posCordFunc.rotate();
  posCordCart = (boneStart + boneEnd)/2 + Point(0.0,f(0.0));
  
  while (posCordFunc.x() <= bone.length/2)// paint upper part pixel by pixel
  {
    //calculate (dx,dy)
    dPos = Vector(1.0, 0);
    
    targetCordCart = posCordCart + dPos;
    auto rememberColor = painter->pen().color();
    painter->setPen(Qt::darkGreen);
    painter->drawLine(posCordCart, targetCordCart);
    painter->setPen(rememberColor);
    posCordCart = targetCordCart; 
    posCordFunc += dPos;
  } 
}

void Skeleton::paint(QPainter* painter) const
{
    for (auto b : bones) 
    {
      b.paint(painter);
      Body body(b);
      body.paint(painter);
    }
}

void SkeletonVisualizer::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    painter.fillRect(0, 0, width(), height(), Qt::white);
    paintCenterCross(&painter);

    painter.setPen(QPen(Qt::red, 3, Qt::SolidLine, Qt::RoundCap));
    skeleton->paint(&painter);

    paintSelections(&painter);
}

void SkeletonVisualizer::paintSelections(QPainter* painter)
{
    if (isVerticeSelected)
    {
        painter->setPen(QPen(Qt::darkGreen, 2, Qt::SolidLine, Qt::RoundCap));
        painter->drawEllipse(selectedVertice.x() -SELRAD, selectedVertice.y()
                             -SELRAD, 2*SELRAD, 2*SELRAD);
    }
}

void SkeletonVisualizer::paintCenterCross(QPainter* painter)
{
    painter->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap));
    painter->drawLine(w/2 -SELRAD, h/2, w/2 +SELRAD, h/2);
    painter->drawLine(w/2, h/2 -SELRAD, w/2, h/2 +SELRAD);
}

void Bone::paint(QPainter* painter) const
{
    if (length != 0) painter->drawLine(x(), y(), endx(), endy());
}

void Bone::randPos(double max)
{
    pos.setX(-max + randd(2*max));
    pos.setY(-max + randd(2*max));
}

void Bone::randAngle()
{
    angle = randd(2*PI);
}

void Bone::randLength(double max)
{
    length =  randd(max);
}

#include "SkeletonVisualizer.moc"
