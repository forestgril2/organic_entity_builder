#include <QtGui/QApplication>
#include "SkeletonVisualizer.h"


int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    SkeletonVisualizer skeletonvisualizer;
    skeletonvisualizer.show();
    return app.exec();
}
