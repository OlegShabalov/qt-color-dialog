#ifndef COLORDIALOG_HPP
#define COLORDIALOG_HPP

#include <QDialog>



class ColorDialog : public QDialog {
    Q_OBJECT

public:
    ColorDialog(QWidget * parent = nullptr);
    ColorDialog(const QColor & initial, QWidget * parent = nullptr);

    QColor currentColor() const;

public Q_SLOTS:
    void setCurrentColor(const QColor & color);

Q_SIGNALS:
    void currentColorChanged(const QColor & color);

private:
    QColor _currentColor;
};



#endif // COLORDIALOG_HPP
