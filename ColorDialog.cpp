#include "ColorDialog.hpp"

#include <QPainter>
#include <QHBoxLayout>
#include <QMouseEvent>

#include <QDebug>



class Test : public QWidget {
public:
    Test(QWidget * parent)
        : QWidget(parent)
    {  }

    void set(unsigned char r, unsigned char g, unsigned char b) {
        _r = r;
        _g = g;
        _b = b;

        QPalette pal = QPalette();
        pal.setColor(QPalette::Window, QColor(r, g, b));
        setAutoFillBackground(true);
        setPalette(pal);

        update();
    }

private:
    unsigned char _r;
    unsigned char _g;
    unsigned char _b;
};

///////////////////////////////////////////////////////////////////////////////

class ColorSelector : public QWidget {
public:
    ColorSelector(QWidget * parent, Test * t)
        : QWidget(parent)
        , t(t)
    {  }

    void setItputColor(unsigned char r, unsigned char g, unsigned char b) {
        _ir = r;
        _ig = g;
        _ib = b;
        _pointToColor();
        update();
    }

    void setOutputColor(unsigned char r, unsigned char g, unsigned char b) {
        _or = r;
        _og = g;
        _ob = b;
        const QColor temp = QColor(_or, _og, _ob).toHsv();
        _pointX = temp.saturationF();
        _pointY = 1 - temp.valueF();
        t->set(_or, _og, _ob);
    }

protected:
    void paintEvent(QPaintEvent * event) override {
        Q_UNUSED(event)

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        const int lineSize = 4;

        for (int h = 0; h < height(); h += lineSize) {
            const float k = 1 - (h / static_cast<float>(height()));

            QLinearGradient gradient(QPointF(0, 0), QPointF(width(), 1));
            gradient.setColorAt(0, QColor(255 * k, 255 * k, 255 * k));
            gradient.setColorAt(1, QColor(_ir * k, _ig * k, _ib * k));

            QBrush brush(gradient);
            painter.setPen(QPen(brush, 1));
            painter.setBrush(brush);
            painter.drawRect(0, h, width(), lineSize);
        }

        painter.setBrush(Qt::NoBrush);

        const int x = width()  * _pointX - 10;
        const int y = height() * _pointY - 10;

        if (_pointY < 0.4f - _pointX * 0.3f) {
            painter.setPen(QPen(QColor(70, 70, 70), 2));
            painter.setBrush(QBrush(QColor(_or, _og, _ob)));
            painter.drawEllipse(x, y, 20, 20);
        }
        else {
            painter.setPen(QPen(Qt::white, 2));
            painter.setBrush(QBrush(QColor(_or, _og, _ob)));
            painter.drawEllipse(x, y, 20, 20);
        }
    }

    void mousePressEvent(QMouseEvent * event) override {
        if (event->button() == Qt::LeftButton) {
            _pointX = event->pos().x() / static_cast<float>(width());
            _pointY = event->pos().y() / static_cast<float>(height());
            _pointX = qBound(0.0f, _pointX, 1.0f);
            _pointY = qBound(0.0f, _pointY, 1.0f);
            _pointToColor();
            update();
            event->accept();
            return;
        }
        event->ignore();
    }

    void mouseMoveEvent(QMouseEvent * event) override {
        _pointX = event->pos().x() / static_cast<float>(width());
        _pointY = event->pos().y() / static_cast<float>(height());
        _pointX = qBound(0.0f, _pointX, 1.0f);
        _pointY = qBound(0.0f, _pointY, 1.0f);
        _pointToColor();
        update();
        event->accept();
    }

private:
    void _pointToColor() {
        const float kx = 1 - _pointX;
        const float ky = 1 - _pointY;

        _or = ((255 - _ir) * kx + _ir) * ky;
        _og = ((255 - _ig) * kx + _ig) * ky;
        _ob = ((255 - _ib) * kx + _ib) * ky;

        t->set(_or, _og, _ob);
    }

private:
    float _pointX;
    float _pointY;

    unsigned char _ir;
    unsigned char _ig;
    unsigned char _ib;

    unsigned char _or;
    unsigned char _og;
    unsigned char _ob;

    Test * t;
};

///////////////////////////////////////////////////////////////////////////////

class HueSelector : public QWidget {
public:
    HueSelector(QWidget * parent)
        : QWidget(parent)
    {
        setFixedWidth(24);
    }

    void setColorSelector(ColorSelector * colorSelector) {
        _colorSelector = colorSelector;
    }

    void setOutputColor(unsigned char r, unsigned char g, unsigned char b) {
        const int h = qMax(QColor(r, g, b).toHsv().hsvHue(), 0);
        _pointY = 1 - (h / 359.0f);
        const QColor temp = QColor::fromHsv(h, 255, 255);
        _r = temp.red();
        _g = temp.green();
        _b = temp.blue();
        _colorSelector->setItputColor(_r, _g, _b);
        update();
    }

protected:
    void paintEvent(QPaintEvent * event) override {
        Q_UNUSED(event)

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        const int lineSize = 1;

        for (int h = 0; h < height(); h += lineSize) {
            const float k = 1 - (h / static_cast<float>(height()));

            const QColor color = QColor::fromHsv(k * 359, 255, 255);

            painter.setPen(QPen(color, lineSize));
            painter.drawRect(0+6, h, width()-12, lineSize);
        }

        const int x = width()  * 0.5f    - 10;
        const int y = height() * _pointY - 10;

        painter.setPen(QPen(QColor(70, 70, 70), 2));
        painter.setBrush(QBrush(QColor(_r, _g, _b)));
        painter.drawEllipse(x, y, 20, 20);
    }

    void mousePressEvent(QMouseEvent * event) override {
        if (event->button() == Qt::LeftButton) {
            _pointY = event->pos().y() / static_cast<float>(height());
            _pointY = qBound(0.0f, _pointY, 1.0f);
            _pointToColor();
            update();
            event->accept();
            return;
        }
        event->ignore();
    }

    void mouseMoveEvent(QMouseEvent * event) override {
        _pointY = event->pos().y() / static_cast<float>(height());
        _pointY = qBound(0.0f, _pointY, 1.0f);
        _pointToColor();
        update();
        event->accept();
    }

private:
    void _pointToColor() {
        const QColor c = QColor::fromHsv((1-_pointY) * 359, 255, 255).toRgb();
        _r = c.red();
        _g = c.green();
        _b = c.blue();
        _colorSelector->setItputColor(_r, _g, _b);
    }

private:
    float _pointY;

    unsigned char _r;
    unsigned char _g;
    unsigned char _b;

    ColorSelector * _colorSelector;
};

///////////////////////////////////////////////////////////////////////////////

ColorDialog::ColorDialog(QWidget * parent)
    : ColorDialog(Qt::white, parent)
{  }

ColorDialog::ColorDialog(const QColor & initial, QWidget * parent)
    : QDialog(parent)
    , _currentColor(initial)
{
    QHBoxLayout * l = new QHBoxLayout(this);

    Test * t = new Test(this);
    t->setMinimumSize(100, 100);
    ColorSelector * colorSelector = new ColorSelector(this, t);
    l->addWidget(colorSelector, 1);

    HueSelector * hueSelector = new HueSelector(this);
    hueSelector->setColorSelector(colorSelector);
    l->addWidget(hueSelector, 1);

    l->addWidget(t);

    //const QColor color(255, 255, 32);
    //const QColor color(0, 0, 0); // black
    //const QColor color(255, 255, 255); // white
    //const QColor color(255, 0, 0); // red
    //const QColor color(200, 200, 200); // light white
    const QColor color(150, 25, 100);

    hueSelector->setOutputColor(color.red(), color.green(), color.blue());
    colorSelector->setOutputColor(color.red(), color.green(), color.blue());
}

QColor ColorDialog::currentColor() const {
    return _currentColor;
}

void ColorDialog::setCurrentColor(const QColor & color) {
    _currentColor = color;
}
