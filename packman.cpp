#include "packman.h"
#include <QProcess>
#include <iostream>

packman::packman(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowIcon(QIcon(":/resource/mummy.png"));

    m_open = new QPushButton("open");
    m_version = new QLineEdit();
    m_date = new QLineEdit();
    m_count = new QLineEdit();
    m_path = new QLineEdit();
    m_path->setMinimumWidth(800);
    m_path->setReadOnly(true);
    m_generate = new QPushButton("get started");
    m_generate->setMinimumHeight(100);
    m_log = new QTextEdit();
    m_centralEdit = new QTextEdit();
    m_centralEdit->setReadOnly(true);
    m_log->setReadOnly(true);

    QGridLayout* layout = new QGridLayout;
    layout->addWidget(m_open, 0, 0);
    layout->addWidget(m_path, 0, 1, 1, 3);
    layout->addWidget(m_generate, 0, 4, 3, 1);
    layout->addWidget(m_centralEdit, 1, 1, 1, 3);
    layout->addWidget(new QLabel("version: "), 2, 0, Qt::AlignCenter);
    layout->addWidget(m_version, 2, 1);
    layout->addWidget(m_date, 2, 2);
    layout->addWidget(m_count, 2, 3);
    layout->addWidget(new QLabel("log: "), 3, 0, Qt::AlignCenter);
    layout->addWidget(m_log, 3, 1, 1, 4);

    QWidget* widget = new QWidget();
    widget->setLayout(layout);

    this->setCentralWidget(widget);

    auto funcSetRootPath = [&]() {
        QString root = "/home";
        if (!m_path->text().isEmpty() && QDir(m_path->text()).exists())
            root = m_path->text();

        m_rootDirectory = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
            root,
            QFileDialog::ShowDirsOnly
            | QFileDialog::DontResolveSymlinks);

        m_path->setText(m_rootDirectory);

        QDir rootDirectory(m_rootDirectory);
        QStringList filter = { "*.tar.gz" };
        QStringList files = rootDirectory.entryList(filter, QDir::Files);

        m_centralEdit->clear();
        for (int i = 0; i < files.size(); i++)
        {
            // 찾은 파일을 화면에 표시한다
            m_centralEdit->append(files[i]);

            // 찾은 파일 이름을 파싱해서 버전 정보를 얻어온다
            QStringList fileNamePiece = files[i].split(QLatin1Char('_'), Qt::SkipEmptyParts);
            if (m_version->text().isEmpty() && fileNamePiece.size() > 2)
            {
                m_version->setText(fileNamePiece[1]);
                m_date->setText(QDate::currentDate().toString("yyyyMMdd"));
                // to do
                m_count->setText("1");
            }
            else if (!m_version->text().isEmpty() && fileNamePiece.size() > 2)
            {
                if (m_version->text().compare(fileNamePiece[1]) != 0)
                {
                    m_log->append("<font color=\"Red\">The versions of the files do not match</font>");
                    m_log->append(QString("%1 != %2").arg(m_version->text()).arg(fileNamePiece[1]));
                }
            }
        }
        if (files.size() == 0)
        {
            m_log->append("<font color=\"Red\">cannot find *.tar.gz files</font>");
        }

        m_files = files;
    };

    connect(m_open, &QPushButton::clicked, funcSetRootPath);
    connect(m_generate, SIGNAL(clicked()), this, SLOT(startUnPack()));
}

packman::~packman()
{}

void packman::startUnPack()
{
    if (m_files.size() == 0)
    {
        m_log->append("<font color=\"Red\">cannot find *.tar.gz files</font>");
        m_log->append("<font color=\"Red\">check your path</font>");
        return;
    }
    if (m_version->text().isEmpty())
    {
        m_log->append("<font color=\"Red\">version is empty</font>");
        return;
    }

    QProcess unzip;

    QStringList mode = { "DEV", "QA", "PROD" };
    QDir rootDirectory(m_rootDirectory);

    for (int i = 0; i < m_files.size(); i++)
    {
        for (int j = 0; j < mode.size(); j++)
        {
            if (m_files[i].contains(QString("_%1_").arg(mode[j]), Qt::CaseInsensitive))
            {
                m_log->append(m_files[i]);
                rootDirectory.mkdir(mode[j]);

                QString srcPath = m_rootDirectory + "/" + m_files[i];
                QString dstPath = m_rootDirectory + "/" + mode[j];
                unzip.start("tar", QStringList() << "-zxvf" << srcPath << "-C" << dstPath);
                if (!unzip.waitForStarted())
                {
                    m_log->append("<font color=\"Red\">error unzip.waitForStarted()</font>");
                }

                if (!unzip.waitForFinished())
                {
                    m_log->append("<font color=\"Red\">error unzip.waitForFinished()</font>");
                }

                m_log->append("unzip success");

                QDir currentDirectory(dstPath);
                QStringList exeFiles = currentDirectory.entryList({ "*.exe" }, QDir::Files);

                if (exeFiles.size() == 1)
                {
                    QFile exeFile(QString("%1/%2").arg(dstPath).arg(exeFiles[0]));
                    // have to set according to naming conventions
                    exeFile.rename(QString("%1/Release_%2_%3_%4.exe").arg(m_rootDirectory).arg(m_date->text()).arg(m_count->text()).arg(mode[j]));
                    rootDirectory.rmdir(mode[j]);
                    m_log->append("rename success");
                }
                else
                {
                    m_log->append("fail to rename");
                }

            }
        }
    }
}