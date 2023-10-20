#include "ColorDialog.hpp"

#include <QPainter>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QBitmap>



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

// Because of to the use of a mask, the edge of the circle is drawn
// with ragged edges. To fix this, the edge of the circle is drawn twice:
// in the internal and external widgets.

class AlphaSelectorInternal : public QWidget {
public:
    AlphaSelectorInternal(QWidget * parent)
        : QWidget(parent)
    {  }

    void setColor(unsigned char r, unsigned char g, unsigned char b) {
        _r = r;
        _g = g;
        _b = b;
    }

    void setAlpha(float alpha) {
        _alpha = alpha;
    }

protected:
    void paintEvent(QPaintEvent * event) override {
        Q_UNUSED(event)

        QPainter painter(this);

        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(200, 200, 200));

        for (int h = 0; h < height(); h += 12) {
            painter.drawRect(0 + 6, h, 6, 6);
            painter.drawRect(0 + 6 + 6, h + 6, 6, 6);
        }

        QLinearGradient gradient(QPointF(0, 0), QPointF(0, height()));
        gradient.setColorAt(0, QColor(_r, _g, _b, 0));
        gradient.setColorAt(1, QColor(_r, _g, _b, 255));

        painter.setBrush(QBrush(gradient));
        painter.drawRect(0, 0, width(), height());

        const int x = width()  * 0.5f   - 10;
        const int y = height() * _alpha - 10;

        painter.setRenderHint(QPainter::Antialiasing);

        painter.setPen(QPen(QColor(70, 70, 70), 2));
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(x, y, 20, 20);



        QBitmap mask(width(), height());
        mask.clear();
        QPainter maskPainter(&mask);

        maskPainter.setPen(Qt::NoPen);
        maskPainter.setBrush(QBrush(Qt::color1));

        maskPainter.drawRect(0 + 6, 0, width() - 12, height());
        maskPainter.drawEllipse(x, y, 20, 20);

        setMask(mask);
    }

private:
    float _alpha;
    unsigned char _r;
    unsigned char _g;
    unsigned char _b;
};

class AlphaSelector : public QWidget {
public:
    AlphaSelector(QWidget * parent)
        : QWidget(parent)
    {
        setFixedWidth(24);
        _alpha = 0.5;

        QVBoxLayout * layout = new QVBoxLayout(this);
        layout->setMargin(0);
        layout->setSpacing(0);

        _internal = new AlphaSelectorInternal(this);
        layout->addWidget(_internal);
        _internal->setAlpha(_alpha);
    }

    void setItputColor(const QColor & color) {
        _internal->setColor(color.red(), color.green(), color.blue());
        update();
    }

    void setOutputColor(const QColor & color) {
        _alpha = color.alphaF();
        _internal->setAlpha(_alpha);
        _internal->setColor(color.red(), color.green(), color.blue());
        update();
    }

protected:
    void paintEvent(QPaintEvent * event) override {
        Q_UNUSED(event)

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        const int x = width()  * 0.5f   - 10;
        const int y = height() * _alpha - 10;

        painter.setPen(QPen(QColor(70, 70, 70), 2));
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(x, y, 20, 20);
    }

    void mousePressEvent(QMouseEvent * event) override {
        if (event->button() == Qt::LeftButton) {
            _alpha = event->pos().y() / static_cast<float>(height());
            _alpha = qBound(0.0f, _alpha, 1.0f);
            _internal->setAlpha(_alpha);
            update();
            event->accept();
            return;
        }
        event->ignore();
    }

    void mouseMoveEvent(QMouseEvent * event) override {
        _alpha = event->pos().y() / static_cast<float>(height());
        _alpha = qBound(0.0f, _alpha, 1.0f);
        _internal->setAlpha(_alpha);
        update();
        event->accept();
    }

private:
    float _alpha;
    AlphaSelectorInternal * _internal;
};

///////////////////////////////////////////////////////////////////////////////

class ColorSelector : public QWidget {
public:
    ColorSelector(QWidget * parent, Test * t)
        : QWidget(parent)
        , t(t)
    {  }

    void setAlphaSelector(AlphaSelector * alphaSelector) {
        _alphaSelector = alphaSelector;
    }

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
        update();
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

            painter.setPen(Qt::NoPen);
            painter.setBrush(QBrush(gradient));
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
        _alphaSelector->setItputColor(_outputColor);

        t->set(_outputColor.red(), _outputColor.green(), _outputColor.blue());
    }

private:
    float _pointX;
    float _pointY;
    float _hue;

    QColor _outputColor;

    AlphaSelector * _alphaSelector;

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

            painter.setPen(Qt::NoPen);
            painter.setBrush(QBrush(color));
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
    l->addWidget(hueSelector);

    AlphaSelector * alphaSelector = new AlphaSelector(this);
    colorSelector->setAlphaSelector(alphaSelector);
    l->addWidget(alphaSelector);

    l->addWidget(t, 0, Qt::AlignTop);

    const QColor color(255, 255, 32); // yellow
    //const QColor color(0, 0, 0); // black
    //const QColor color(255, 255, 255); // white
    //const QColor color(255, 0, 0); // red
    //const QColor color(200, 200, 200); // light white
    //const QColor color(150, 25, 100, 100);

    hueSelector->setOutputColor(color.red(), color.green(), color.blue());
    colorSelector->setOutputColor(color.red(), color.green(), color.blue());
    alphaSelector->setOutputColor(color);
    t->set(color.red(), color.green(), color.blue());
}

QColor ColorDialog::currentColor() const {
    return _currentColor;
}

void ColorDialog::setCurrentColor(const QColor & color) {
    _currentColor = color;
}
