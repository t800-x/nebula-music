#ifndef EVENTBUS_H
#define EVENTBUS_H

#include <QObject>
#include <QString>
#include <QDebug>

class eventbus : public QObject
{
    Q_OBJECT
public:
    explicit eventbus(QObject *parent = nullptr);
    QString currentPane;
    int currentLyricindex;
    QString currentPage;
public slots:
    void emitQueueButtonClick() {emit queueButtonClicked();}
    void emitLyricsButtonClicked() {emit lyricsButtonClicked();}
    void emitLyricsViewChanged() {emit lyricsViewChanged();}
    void emitNavButtonClicked(QString btn) {emit navButtonClicked(btn);}

    QString getCurrentPane() {return currentPane;}
    void setCurrentPane(QString str) {currentPane = str; qDebug() << currentPane; emit currentPaneChanged();}

    int getCurrentLyricIndex() {return currentLyricindex;}
    void setCurrentLyricIndex(int idx) {currentLyricindex = idx;}
signals:
    void queueButtonClicked();
    void lyricsButtonClicked();
    void currentPaneChanged();
    void lyricsViewChanged();
    void navButtonClicked(QString btn);
};

#endif // EVENTBUS_H
