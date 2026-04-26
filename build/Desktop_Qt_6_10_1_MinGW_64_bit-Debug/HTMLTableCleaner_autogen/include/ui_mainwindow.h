/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_2;
    QTextEdit *sourceTextEdit;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_3;
    QTextEdit *resultTextEdit;
    QTableWidget *resultTableWidget;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QPushButton *openButton;
    QPushButton *cleanButton;
    QPushButton *saveButton;
    QPushButton *copyButton;
    QPushButton *clearButton;
    QPushButton *dbExportButton;
    QSpacerItem *horizontalSpacer_2;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1317, 848);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName("verticalLayout");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName("groupBox");
        verticalLayout_2 = new QVBoxLayout(groupBox);
        verticalLayout_2->setObjectName("verticalLayout_2");
        sourceTextEdit = new QTextEdit(groupBox);
        sourceTextEdit->setObjectName("sourceTextEdit");

        verticalLayout_2->addWidget(sourceTextEdit);


        horizontalLayout->addWidget(groupBox);

        groupBox_2 = new QGroupBox(centralwidget);
        groupBox_2->setObjectName("groupBox_2");
        verticalLayout_3 = new QVBoxLayout(groupBox_2);
        verticalLayout_3->setObjectName("verticalLayout_3");
        resultTextEdit = new QTextEdit(groupBox_2);
        resultTextEdit->setObjectName("resultTextEdit");

        verticalLayout_3->addWidget(resultTextEdit);

        resultTableWidget = new QTableWidget(groupBox_2);
        resultTableWidget->setObjectName("resultTableWidget");

        verticalLayout_3->addWidget(resultTableWidget);


        horizontalLayout->addWidget(groupBox_2);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        openButton = new QPushButton(centralwidget);
        openButton->setObjectName("openButton");
        openButton->setIconSize(QSize(16, 16));

        horizontalLayout_2->addWidget(openButton);

        cleanButton = new QPushButton(centralwidget);
        cleanButton->setObjectName("cleanButton");

        horizontalLayout_2->addWidget(cleanButton);

        saveButton = new QPushButton(centralwidget);
        saveButton->setObjectName("saveButton");

        horizontalLayout_2->addWidget(saveButton);

        copyButton = new QPushButton(centralwidget);
        copyButton->setObjectName("copyButton");

        horizontalLayout_2->addWidget(copyButton);

        clearButton = new QPushButton(centralwidget);
        clearButton->setObjectName("clearButton");

        horizontalLayout_2->addWidget(clearButton);

        dbExportButton = new QPushButton(centralwidget);
        dbExportButton->setObjectName("dbExportButton");

        horizontalLayout_2->addWidget(dbExportButton);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);


        verticalLayout->addLayout(horizontalLayout_2);

        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "HTML Table Cleaner", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "\320\230\321\201\321\205\320\276\320\264\320\275\321\213\320\271 HTML", nullptr));
        sourceTextEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "\320\222\321\201\321\202\320\260\320\262\321\214\321\202\320\265 HTML \320\272\320\276\320\264 \321\201 \321\202\320\260\320\261\320\273\320\270\321\206\320\265\320\271 \320\267\320\264\320\265\321\201\321\214...", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("MainWindow", "\320\236\321\207\320\270\321\211\320\265\320\275\320\275\321\213\320\271 HTML", nullptr));
        resultTextEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "\320\227\320\264\320\265\321\201\321\214 \320\261\321\203\320\264\320\265\321\202 \320\276\321\207\320\270\321\211\320\265\320\275\320\275\321\213\320\271 \320\272\320\276\320\264 \321\202\320\260\320\261\320\273\320\270\321\206\321\213...", nullptr));
        openButton->setText(QCoreApplication::translate("MainWindow", "\360\237\223\202 \320\236\321\202\320\272\321\200\321\213\321\202\321\214", nullptr));
        cleanButton->setText(QCoreApplication::translate("MainWindow", "\360\237\224\204 \320\236\321\207\320\270\321\201\321\202\320\270\321\202\321\214 HTML", nullptr));
        saveButton->setText(QCoreApplication::translate("MainWindow", "\360\237\222\276 \320\241\320\276\321\205\321\200\320\260\320\275\320\270\321\202\321\214", nullptr));
        copyButton->setText(QCoreApplication::translate("MainWindow", "\360\237\223\213 \320\232\320\276\320\277\320\270\321\200\320\276\320\262\320\260\321\202\321\214", nullptr));
        clearButton->setText(QCoreApplication::translate("MainWindow", "\360\237\227\221\357\270\217 \320\236\321\207\320\270\321\201\321\202\320\270\321\202\321\214 \320\277\320\276\320\273\321\217", nullptr));
        dbExportButton->setText(QCoreApplication::translate("MainWindow", "\360\237\223\232 \320\255\320\272\321\201\320\277\320\276\321\200\321\202 \320\262 \320\221\320\224", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
