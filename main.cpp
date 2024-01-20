#include <QApplication>

#include <QPushButton>
#include <QVBoxLayout>
#include <QDebug>

#include "ColorDialog.hpp"



int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QWidget widget;
    QVBoxLayout * layout = new QVBoxLayout(&widget);
    QPushButton * button = new QPushButton("Get color", &widget);
    layout->addWidget(button);

    widget.connect(button, &QPushButton::clicked, button, [=]() {
        qDebug() << ColorDialog::getColor(Qt::blue, button, "Select color");
    });

    widget.show();

    return a.exec();
}
