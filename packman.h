#pragma once

#include <QtWidgets>

class packman : public QMainWindow
{
    Q_OBJECT
public:
    packman(QWidget* parent = nullptr);
    ~packman();

public slots:
    void startUnPack();

private:
    QPushButton* m_open;
    QLineEdit* m_path;
    QTextEdit* m_centralEdit;
    QLineEdit* m_version;
    QLineEdit* m_date;
    QLineEdit* m_count;
    QPushButton* m_generate;
    QTextEdit* m_log;
    QStringList m_files;
    QString m_rootDirectory;
};

