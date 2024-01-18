#ifndef COLORDIALOG_HPP
#define COLORDIALOG_HPP

#include <QDialog>



class ColorDialogPrivate;



class ColorDialog : public QDialog {
    Q_OBJECT

public:
    ColorDialog(QWidget * parent = nullptr);
    ColorDialog(const QColor & initial, QWidget * parent = nullptr);

    ~ColorDialog() override;

    QColor currentColor() const;

public Q_SLOTS:
    void setCurrentColor(const QColor & color);
    void setEnabledAlphaChannel(bool enabled);
    void setEnabledNoButtons(bool enabled);

Q_SIGNALS:
    void currentColorChanged(const QColor & color);

private:
    ColorDialogPrivate * _d;
};



#endif // COLORDIALOG_HPP
