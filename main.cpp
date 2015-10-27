#include <QtGui/QApplication>
#include "SkeletonVisualizer.h"


int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    Skeleton s(1, 0);
    SkeletonVisualizer skeletonvisualizer(&s);
    skeletonvisualizer.show();
    return app.exec();
}
