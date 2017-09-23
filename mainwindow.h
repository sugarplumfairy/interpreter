#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QPlainTextEdit>
#include <QObject>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;

class LineNumberArea;
#include <QMainWindow>
#include <QCloseEvent>
#include<cstdio>
#include<cmath>
#include<cctype>
#include<algorithm>
#include<map>
#include<queue>
#include<stack>
#include<vector>
#include<iostream>
#include<algorithm>
#include<cstring>
#include<ctime>
#include<sstream>
#include "highlighter.h"
#include <QWidget>
#include <QMessageBox>
#include <QLineEdit>
#include <QDialog>
#include<QTextDocument>
#include<QSyntaxHighlighter>
class lineNumberArea;
class CodeEditor;
using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void newFile();   // 新建操作
    bool maybeSave(); // 判断是否需要保存
    bool save();      // 保存操作
    bool saveAs();    // 另存为操作
    bool saveFile(const QString &fileName); // 保存文件
    bool loadFile(const QString &fileName);

    int line();
    int search(int hash);
    void destroy(int flower);
    int function_parameter();
    void Pretreatment();
    void myPrint();
    void jump();
    void parseIn();
    void parseOut();
    int expression(int type);
    void definition(int type);
    int match(char a);
    int next(int &type_value);
    void function_resolve();
    int function_match(int *tail);
    void function_body();
    void point_back();
    string string_factor();
    string string_deal_tail(string lvalue);
    string string_deal();
    double string_cmp_tail(string lvalue);
    double string_cmp();
    double string_enter_tail(double lvalue);
    double string_enter();
    double factor();
    double one_tail(double lvalue);
    double one();
    double two_tail(double lvalue);
    double two();
    double three_tail(double lvalue);
    double three();
    double four_tail(double lvalue);
    double four();
    double five_tail(double lvalue);
    double five();
    double expression_six_enter_tail(double lvalue);
    double expression_six_enter();
    int inside();
    int if_function();
    int while_function();
    int do_function();
    int return_function();
    void program();
    int minc_main(string s);

    QTextEdit *led;
    QDialog *dlg;
    QDialog *box;
    string hh;
    ~MainWindow();
protected:
    void closeEvent(QCloseEvent *event); // 关闭事件
private slots:
    void on_action_N_triggered();

    void on_action_S_triggered();

    void on_action_A_triggered();

    void on_action_O_triggered();

    void on_action_E_triggered();

    void on_action_Z_triggered();

    void on_action_X_triggered();

    void on_action_C_triggered();

    void on_action_P_triggered();
    void showFindText();
    void setText();
    void on_action_S_2_triggered();

    void on_action_I_triggered();

    void on_action_K_triggered();
private:
    Ui::MainWindow *ui;
    // 为真表示文件没有保存过，为假表示文件已经被保存过了
    bool isUntitled;
    // 保存当前文件的路径
    QString curFile;
    QLineEdit *findLineEdit;
    QDialog *findDlg;
    CodeEditor *codeEditor;
    QWidget *lineNumberArea;
    Highlighter *highlighter;
};

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    CodeEditor(QWidget *a = 0);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);

private:
    QWidget *lineNumberArea;
};


class LineNumberArea : public QWidget
{
public:
    LineNumberArea(CodeEditor *editor) : QWidget(editor) {
        codeEditor = editor;
    }

    QSize sizeHint() const override {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    CodeEditor *codeEditor;
};





#endif // MAINWINDOW_H
