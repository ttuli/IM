#ifndef WINDOWS_BACKGROUND_H
#define WINDOWS_BACKGROUND_H

#include <QWidget>
#include <QQuickWidget>
#include <QQuickItem>

class WindowsBackground : public QWidget
{
    Q_OBJECT
public:
    explicit WindowsBackground(QWidget *parent = nullptr);

protected:
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;

    void initInterface();

    void setTitleWidget(QQuickWidget *widget,QQuickItem *item);

    void showEvent(QShowEvent*);

    void mouseMoveEvent(QMouseEvent *event);

    void mousePressEvent(QMouseEvent *event);

    void mouseReleaseEvent(QMouseEvent *event);

private:
    QPoint windowp_;
    QPoint cursorp_;
    bool leftButton_=false;

    QQuickWidget *title;
    QQuickItem *titleItem;
    QList<QRect> nonTitleAreas_;
signals:
};

#endif // WINDOWS_BACKGROUND_H
