#ifndef CURTAILLABEL_H
#define CURTAILLABEL_H

#include <QLabel>

class CurtailLabel : public QLabel
{
    Q_OBJECT
public:
    explicit CurtailLabel(QWidget *parent = 0);

    QString text() const;
    void setText(const QString &text);

signals:

public slots:

    // QWidget interface
protected:
private:
    QString m_text;

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *);
};

#endif // CURTAILLABEL_H
