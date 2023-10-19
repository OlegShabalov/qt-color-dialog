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

    void setItputColor(float hue) {
        _hue = hue;
        _updateColor();
        update();
    }

    void setOutputColor(unsigned char r, unsigned char g, unsigned char b) {
        _outputColor = QColor(r, g, b);
        const QColor temp = _outputColor.toHsv();
        _hue = qMax(temp.hsvHueF(), 0.0);
        _pointX = temp.saturationF();
        _pointY = 1 - temp.valueF();
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
            gradient.setColorAt(0, QColor::fromHsvF(_hue, 0, k));
            gradient.setColorAt(1, QColor::fromHsvF(_hue, 1, k));

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
            painter.setBrush(QBrush(_outputColor));
            painter.drawEllipse(x, y, 20, 20);
        }
        else {
            painter.setPen(QPen(Qt::white, 2));
            painter.setBrush(QBrush(_outputColor));
            painter.drawEllipse(x, y, 20, 20);
        }
    }

    void mousePressEvent(QMouseEvent * event) override {
        if (event->button() == Qt::LeftButton) {
            _pointX = event->pos().x() / static_cast<float>(width());
            _pointY = event->pos().y() / static_cast<float>(height());
            _pointX = qBound(0.0f, _pointX, 1.0f);
            _pointY = qBound(0.0f, _pointY, 1.0f);
            _updateColor();
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
        _updateColor();
        update();
        event->accept();
    }

private:
    void _updateColor() {
        _outputColor = QColor::fromHsvF(_hue, _pointX, 1 - _pointY).toRgb();
        t->set(_outputColor.red(), _outputColor.green(), _outputColor.blue());
    }

private:
    float _pointX;
    float _pointY;
    float _hue;

    QColor _outputColor;

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
        _hue = qMax(QColor(r, g, b).toHsv().hsvHueF(), 0.0);
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

            const QColor color = QColor::fromHsvF(k, 1, 1);

            painter.setPen(QPen(color, lineSize));
            painter.drawRect(0 + 6, h, width() - 12, lineSize);
        }

        const int x = width()  * 0.5f          - 10;
        const int y = height() * (1.0f - _hue) - 10;

        painter.setPen(QPen(QColor(70, 70, 70), 2));
        painter.setBrush(QBrush(QColor::fromHsvF(_hue, 1, 1)));
        painter.drawEllipse(x, y, 20, 20);
    }

    void mousePressEvent(QMouseEvent * event) override {
        if (event->button() == Qt::LeftButton) {
            _hue = 1.0f - (event->pos().y() / static_cast<float>(height()));
            _hue = qBound(0.0f, _hue, 1.0f);
            _colorSelector->setItputColor(_hue);
            update();
            event->accept();
            return;
        }
        event->ignore();
    }

    void mouseMoveEvent(QMouseEvent * event) override {
        _hue = 1.0f - (event->pos().y() / static_cast<float>(height()));
        _hue = qBound(0.0f, _hue, 1.0f);
        _colorSelector->setItputColor(_hue);
        update();
        event->accept();
    }

private:
    float _hue;
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

    //const QColor color(255, 255, 32); // yellow
    //const QColor color(0, 0, 0); // black
    //const QColor color(255, 255, 255); // white
    //const QColor color(255, 0, 0); // red
    //const QColor color(200, 200, 200); // light white
    const QColor color(150, 25, 100);

    hueSelector->setOutputColor(color.red(), color.green(), color.blue());
    colorSelector->setOutputColor(color.red(), color.green(), color.blue());
    t->set(color.red(), color.green(), color.blue());
}

QColor ColorDialog::currentColor() const {
    return _currentColor;
}

void ColorDialog::setCurrentColor(const QColor & color) {
    _currentColor = color;
}
