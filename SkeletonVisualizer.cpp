#include "SkeletonVisualizer.h"
#include <vector>
#include <algorithm>

#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QAction>
#include <QMouseEvent>


#define PI 3.1415926535897932384626433832795028841971694
#define SELRAD 5 //selection radius
const unsigned int MSEC_PER_SEC = 1000;
const double TIME_INTERVAL = 0.05;

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
    
    updateTimer = new QTimer(this);
    updateTimer->start(TIME_INTERVAL * MSEC_PER_SEC);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(update()));

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

void Body::drawLineGivenColor(const QColor& c, Point a, Point b, QPainter* painter)
{
  auto rememberColor = painter->pen().color();
  painter->setPen(Qt::darkGreen);
  painter->drawLine(a, b);
  painter->setPen(rememberColor);
}

void Body::paint(QPainter* painter)
{
  qint64 counter = bone.lifeTimer.nsecsElapsed()/30000000;
  
  auto fXY = [&](double x){return 15+15*(sin(0.2*(x - counter))*sin(0.2*(x - counter)));};
  auto dfXY = [](auto funOxOy, double x, double dx){auto yPlus = funOxOy(x + dx); return yPlus - funOxOy(x);};
  
  if (bone.length == 0) bone.length = 0.01; 
  
  auto posScr = Point(0.0, 0.0);
  auto posFunc = Vector(0.0, fXY(0.0));
  auto posFuncCopy = posFunc;
  auto dPosScr = Vector(0.0, 0.0);
  auto dPosFun = Vector(0.0, 0.0);
  auto boneStart = bone.startPoint();
  auto boneEnd = bone.endPoint();
  auto arcAngle = M_PI;
  auto arcAngleTotal = 0.0;
  double targetCoordFuncOx = bone.length/2;

  posFuncCopy.rotate(-bone.angle);
  posScr = (boneStart + boneEnd)/2 + posFuncCopy;
  
  auto drawSegmAlong = [&,this](auto dx)
  {
    auto dy = dfXY(fXY, posFunc.x(), dx);
    dPosScr = dPosFun = Vector(dx, dy);
    dPosScr.rotate(-bone.angle + arcAngleTotal);
    drawLineGivenColor(Qt::darkGreen, posScr, posScr + dPosScr, painter);
    posScr += dPosScr;
    posFunc += dPosFun;
  };
  
  auto drawAlongSide = [&,this](auto& targFunOX)
  {
    while (posFunc.x() <= targFunOX) drawSegmAlong(1.0);
    if (posFunc.x() > targFunOX)
    {
      posFunc -= dPosFun; // step back 
      drawSegmAlong(targFunOX - posFunc.x()); // and finish missing segment
    }
  };
  
  auto drawAroundCenter = [&,this](auto center, auto targArc)
  {
    auto arcTotal = 0.0;
    while (arcTotal <= targArc)
    {
      auto dx = 1.0;
      auto dy = dfXY(fXY, posFunc.x(), dx);
      auto arc = dx / (fXY(posFunc.x()) +dy);
      
      auto posRadius = Vector(posScr - center);
      posRadius.rotate(-asin(arc));
      posRadius += (dy/posRadius.length())*posRadius;
      
      drawLineGivenColor(Qt::darkGreen, posScr, center + posRadius, painter);
      posScr = center + posRadius; 
      posFunc += Point(dx, dy);
      targetCoordFuncOx += dx;
      arcTotal += arc;
    }
    
//     if (arcTotal > targArc)
//     {
//       //calculate (dx,dy)
//       auto arc = (targArc - posFunc.x()) / (fXY(posFunc.x()));
//       auto dPosFunFin = Point(posFunc.x() - targArc, 0.0);
//       auto posRadius = Vector(posScr - center);
//       
//       posRadius.rotate(-asin(arc));
//       drawLineGivenColor(Qt::darkGreen, posScr, center + posRadius, painter);
//       posScr = center + posRadius;
//       posFunc += dPosFunFin;
//     }
    arcAngleTotal += targArc;
  };
 
  drawAlongSide(targetCoordFuncOx);
  drawAroundCenter(bone.endPoint(), arcAngle);
  targetCoordFuncOx += bone.length;
  drawAlongSide(targetCoordFuncOx);
  drawAroundCenter(bone.startPoint(), arcAngle);
  targetCoordFuncOx += bone.length/2;
  drawAlongSide(targetCoordFuncOx);
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
