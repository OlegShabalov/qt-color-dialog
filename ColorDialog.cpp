#include "ColorDialog.hpp"

#include <QPainter>
#include <QHBoxLayout>



class ColorBox : public QWidget {
    //Q_OBJECT

public:
    ColorBox(QWidget * parent)
        : QWidget(parent)
    {  }

protected:
    void paintEvent(QPaintEvent * event) override {
        Q_UNUSED(event)

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        const int aa = 20; // 1

        for (int h = 0; h < height(); h += aa) {
            const float k = 1 - (h / static_cast<float>(height()));
            QColor color(0, 222, 255);
            unsigned char r = color.red();
            unsigned char g = color.green();
            unsigned char b = color.blue();

            r *= k;
            g *= k;
            b *= k;

            QLinearGradient gradient(QPointF(0, 0), QPointF(width(), 1));
            gradient.setColorAt(0, QColor(255 * k, 255 * k, 255 * k));
            gradient.setColorAt(1, QColor(r, g, b));

            QBrush brush(gradient);
            QPen pen(brush, 1);
            painter.setPen(pen);
            painter.setBrush(brush);
            painter.drawRect(0, h, width(), aa);
        }

        painter.setBrush(Qt::NoBrush);

        painter.setPen({Qt::white, 4});
        painter.drawEllipse(100, 100, 20, 20);
        painter.setPen({Qt::black, 1.5});
        painter.drawEllipse(100, 100, 20, 20);
    }
};

class ColorBox2 : public QWidget {
    //Q_OBJECT

public:
    ColorBox2(QWidget * parent)
        : QWidget(parent)
    {  }

protected:
    void paintEvent(QPaintEvent * event) override {
        Q_UNUSED(event)

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        const int aa = 20; // 1

        for (int w = 0; w < width(); w += aa) {
            const float k = 1 - (w / static_cast<float>(width()));
            QColor color(0, 222, 255);
            unsigned char r = color.red();
            unsigned char g = color.green();
            unsigned char b = color.blue();

            r = (255 - r) * k + r;
            g = (255 - g) * k + g;
            b = (255 - b) * k + b;

            QLinearGradient gradient(QPointF(0, 0), QPointF(1, height()));
            gradient.setColorAt(0, QColor(r, g, b));
            gradient.setColorAt(1, Qt::black);

            QBrush brush(gradient);
            QPen pen(brush, 1);
            painter.setPen(pen);
            painter.setBrush(brush);
            painter.drawRect(w, 0, aa, height());
        }

        painter.setBrush(Qt::NoBrush);

        painter.setPen({Qt::white, 4});
        painter.drawEllipse(100, 100, 20, 20);
        painter.setPen({Qt::black, 1.5});
        painter.drawEllipse(100, 100, 20, 20);
    }
};

class ColorBox3 : public QWidget {
    //Q_OBJECT

public:
    ColorBox3(QWidget * parent)
        : QWidget(parent)
    {  }

protected:
    void paintEvent(QPaintEvent * event) override {
        Q_UNUSED(event)

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        const int margin = 10;
        const double diameter = std::min(width(), height()) - 2 * margin;

        const QPointF center(width()/2.0, height()/2.0);
        const QRectF rect(center.x() - diameter/2.0,
                          center.y() - diameter/2.0, diameter, diameter);

        double angle = 0;
        while (angle < 360) {
            QColor color = QColor::fromHsv(angle, 255, 255);

            QRadialGradient gradient(center, diameter/2.0);
            gradient.setColorAt(0, Qt::white);
            gradient.setColorAt(1, color);

            QBrush brush(gradient);
            QPen pen(brush, 1.0);
            painter.setPen(pen);
            painter.setBrush(brush);
            painter.drawPie(rect, angle * 16, 16 * 1);

            angle += 1; // 1 // 0.5
        }
    }
};





ColorDialog::ColorDialog(QWidget * parent)
    : ColorDialog(Qt::white, parent)
{  }

ColorDialog::ColorDialog(const QColor & initial, QWidget * parent)
    : QDialog(parent)
    , _currentColor(initial)
{
    QHBoxLayout * l = new QHBoxLayout(this);

    l->addWidget(new ColorBox(this), 1);
    l->addWidget(new ColorBox2(this), 1);
    l->addWidget(new ColorBox3(this), 1);
}

QColor ColorDialog::currentColor() const {
    return _currentColor;
}

void ColorDialog::setCurrentColor(const QColor & color) {
    _currentColor = color;
}
