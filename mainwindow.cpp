#define _CRT_SECURE_NO_WARNINGS
#define POOLSIZE 10000
#include <mainwindow.h>
#include <ui_mainwindow.h>
#include <QMessageBox>
#include <QPushButton>
#include <QFileDialog>
#include <QTextStream>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QProcess>
#include <QWidget>
#include <QTextBlock>
#include <QPainter>
#include<QPlainTextEdit>
#include <QObject>
#include<cstdio>
#include<cmath>
#include<cctype>
#include<cstring>
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
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>


//命名空间
using namespace std;

//记录minFlower用于函数存取变量
stack<int> mf;

//用于永久存储字符串和double型变量
stack<string> s;
stack<double> d;

/*
symbols:标识符表
current_id:用于每次查询是否存在相同变量或者添加新的变量
*/
int *symbols;
int *current_id;
int *symbols_tail;
char *head;
char *src, *old_src;
int *idMain;
int *text;
int * old_text;
int flagMain = 0;
int token;
//输出流
stringstream stream;

//返回值的全局变量(real型或string型)
double return_real;
string return_string;
//记录花括号
int flower = 0;
string outHintStr;
string inHintStr;
//系统关键字
enum {
    INT = 1, REAL, STRING, WHILE, IF, DO, IN, OUT, Main, Return
};

/*
标识符表的枚举类型
Hash：标识符的唯一标识
Value: 若为变量存变量值，若为函数则存地址
Type: 区分数据类型
Token: 区分变量及函数
Log: 判断全局或本地变量
IdSize：每一个标识符在表中占用的域长度
Flower：记录所处的花括号层数，用于作用域判断
*/
enum {
    Hash, Value, Type, Token, Address, Flower, WwwLz, IdSize
};

//操作符(按优先级顺序排列)
enum {
    Assign = 128, Add, Sub, Mul, Div, Mod, Con, Del, Or, And, Non, Mor, Les, Moe, Loe, Eq, Neq, Pos, Neg
};

/*
函数、标识符、系统函数
*/

enum {
    Fun = 1, Id, Sys
};

//预先声明函数
//表达式识别入口
double expression_six_enter();
//字符串处理入口
string string_deal();
int match(char a);
void point_back();

void program();

int if_function();
int while_function();
int return_function();
int do_function();
double three();

int MainWindow::line() {
    int line = 1;
    char *text = src;
    while (text != 0 && *text != 0) {
        if (*text == '\n') {
            line++;
        }
        text--;
    }
    return line;
}

//查变量
int MainWindow::search(int hash) {
    current_id = symbols_tail - IdSize;
    while (current_id[Token] && current_id[Flower] >= mf.top()) {
        if (current_id[Hash] == hash) {
            return 1;
        }
        current_id -= IdSize;
    }

    current_id = symbols;

    while (current_id[Token] && current_id[Flower] == 0) {
        if (current_id[Hash] == hash) {
            return 1;
        }
        current_id += IdSize;
    }

    current_id = symbols_tail;
    current_id[Token] = Id;
    current_id[Hash] = hash;
    current_id[Value] = 0;
    current_id[Flower] = flower;
    current_id[Type] = INT;
    symbols_tail += IdSize;

    return 1;
}


//销毁作用域以外的变量
void MainWindow::destroy(int flower) {
    current_id = symbols_tail - IdSize;
    while (current_id[Token] && current_id[Flower] >= flower) {
        current_id[Token] = 0;
        current_id -= IdSize;
    }
    symbols_tail = current_id + IdSize;
}
//解析函数参数列表
int MainWindow::function_parameter() {
    int wwwlz = 0, k = 1;
    while (*src != ')') {
        token = *src++;
        //如果找到逗号则continue
        if (token == ',') {
            continue;
        }
        //查找参数声明语句
        if ((token >= 'a' && token <= 'z') || (token >= 'A' && token <= 'Z')) {
            //记录标识符首地址
            char *identifier = src - 1;
            //标识符的哈希值
            int hash = token;
            while ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z') || (*src >= '0' && *src <= '9')) {
                hash = hash * 131 + *src++;
            }
            int type = 0;
            current_id = symbols;
            int i = 0;
            //查找数据类型的标识符声明
            while (current_id[Token]) {
                i++;
                if (current_id[Hash] == hash) {
                    while (*src == ' ') {
                        src++;
                    }
                    type = i;
                    break;
                }
                current_id += IdSize;
            }

            switch (type) {
            case 1: {
                wwwlz = wwwlz + 137 * k++;
                break;
            }
            case 2: {
                wwwlz = wwwlz + 149 * k++;
                break;
            }
            case 3: {
                wwwlz = wwwlz + 151 * k++;
                break;
            }
            default: {
                //do nothing
            }
            }
        }
    }
    src++;
    return wwwlz;
}
//预存系统变量
void MainWindow::Pretreatment() {
    current_id = symbols;
    while (*src != 0) {
        token = *src++;
        int hash = token;
        if (token == ' ') {
            continue;
        }
        else {
            while (*src != ' ' && *src != 0) {
                hash = hash * 131 + *src;
                src++;
            }

            //标明是系统变量
            current_id[Token] = Sys;
            current_id[Hash] = hash;
            current_id[Flower] = 0;
            current_id += IdSize;
        }
    }
}
//输出语句
void MainWindow::myPrint() {
    while (*src != '"') {
        if (*src == 0 || *src == '\n' || *src == ';') {
            printf("Error:Less of '\"'! (line %d)", line());
            stream << "Error:Less of '\"'! (line:" << line() << ")";
            system("pause");
        }
        if (*src == '\\') {
            src++;
            if (*src == 'n') {
                //printf("\n");
                //stream << "\n";

                src++;
            }
            else if (*src == 't') {
                //printf("\t");
                //stream << "\t";

                src++;
            }
            else if (*src == '0') {
                src++;
                while (*src != '"') {
                    if (*src == 0 || *src == '\n' || *src == ';') {
                        printf("Error:Less of '\"'! (line %d)", line());
                        stream << "Error:Less of '\"'! (line:" << line() << ")";
                        system("pause");
                    }
                    src++;
                }
                return;
            }
            else if (*src == '\\') {
                //printf("\\");
                //stream << "\\";

                src++;
            }
            else if (*src == '"') {
                //printf("\"");
                //stream << "\"";

                src++;
            }
            else {
                //printf("\\%c", *src);
                //stream << "\\" << *src;

                src++;
            }
        }
        else {
            printf("%c", *src);
            stream << *src;
            src++;
        }
    }
}
//空格跳过语句
void MainWindow::jump() {
    while (*src == ' ' || *src == '\t' || *src == '\n') {
        src++;
    }
}

//解析in并赋值给参数
void MainWindow::parseIn() {
    jump();
    char *last_pos = src;
    int hash = *src;
    //�жϸ��ַ��Ƿ�Ϊ��ĸ��ͷ
    if ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z')) {
        src++;
        while ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z') || (*src >= '0' && *src <= '9')) {
            hash = hash * 131 + *src;
            src++;
        }
        //		oldHash = hash;
        //		hash = hash + location + flower * 149 + ifOrwhile;
        if (search(hash)) {
            int type = current_id[Type];
            switch (type) {
            case 1: {
                int a;
                //in a;
                led->clear();
                QString hintStr = QString::fromStdString(inHintStr);
                if(inHintStr != "")
                    dlg->setWindowTitle(hintStr);
                else
                    dlg->setWindowTitle("Input");
                dlg->exec();
                a = atoi(hh.c_str());
                current_id[Value] = a;
                break;
            }
            case 2: {
                double a;
                led->clear();
                QString hintStr = QString::fromStdString(inHintStr);
                if(inHintStr != "")
                    dlg->setWindowTitle(hintStr);
                else
                    dlg->setWindowTitle("Input");
                dlg->exec();
                a = atof(hh.c_str());
                d.push(a);
                current_id[Value] = (int)&d.top();
                break;
            }
            case 3: {
                string ss;
                led->clear();
                QString hintStr = QString::fromStdString(inHintStr);
                if(inHintStr != "")
                    dlg->setWindowTitle(hintStr);
                else
                    dlg->setWindowTitle("Input");
                dlg->exec();
                ss=hh;
                s.push(ss);
                current_id[Value] = (int)&s.top();
                /*
                cout << *(string *)(current_id + Value - 1);*/
                break;
            }
            default: {
                printf("Error:variable error! (line %d)", line());
                stream << "Error:variable error! (line:" << line() << ")";
                system("pause");
            }
            }
        }
    }
    else {
        printf("Error:wrong argument! (line %d)", line());
        stream << "Error:wrong argument!  (line:" << line() << ")";
        system("pause");
    }
}
//解析out
void MainWindow::parseOut() {
    jump();
    //ָ���Ѿ�������out�Ժ��ĵ�һ���ַ�
    int hash = *src;
    //������һ����Ϊ��ĸ
    if ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z')) {
        char *old_src = src;
        src++;
        while ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z') || (*src >= '0' && *src <= '9')) {
            hash = hash * 131 + *src;
            src++;
        }
        jump();
        if (*src != ',' && *src != ';') {
            if (search(hash)) {
                int type = 0;
                type = current_id[Type];
                switch (type) {
                case 1: {
                    src = old_src;
                    int result = (int)expression_six_enter();
                    jump();
                    if (*src == ',') {
                        src++;
                        int time = result;
                        jump();
                        if (*src == '"') {
                            string s = string_deal();
                            while (time--) {
                                cout << s;
                                stream << s;
                            }
                        }
                        else if ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z') || (*src >= '0' && *src <= '9')) {
                            char *old_src = src;
                            int hash = *src;
                            src++;
                            while ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z') || (*src >= '0' && *src <= '9')) {
                                hash = hash * 131 + *src;
                                src++;
                            }
                            //						if (*src != ';') {
                            if (search(hash)) {
                                src = old_src;
                                int type = current_id[Type];
                                switch (type) {
                                case 1: {
                                    int result = (int)expression_six_enter();
                                    while (time--) {
                                        printf("%d", result);
                                        stream << result;
                                    }
                                    break;
                                }
                                case 2: {
                                    double result = expression_six_enter();
                                    while (time--) {
                                        printf("%lf", result);
                                        stream << result;
                                    }
                                    break;
                                }
                                case 3: {
                                    string s = string_deal();
                                    while (time--) {
                                        cout << s;
                                        stream << s;
                                    }
                                    break;
                                }
                                default: {
                                    printf("Error:error in line %d", line());
                                    stream << "Error:error in " << line() ;
                                    system("pause");
                                }
                                }
                            }
                            else {

                            }
                            //	}
                            //}
                        }
                    }
                    break;
                }
                case 2: {
                    double result = expression_six_enter();
                    printf("%lf", result);
                    stream << result;
                    break;
                }
                case 3: {
                    string s = string_deal();
                    cout << s;
                    stream << s;
                    break;
                }
                default: {
                    printf("Error:error in line %d", line());
                    stream << "Error:error in line" << line();
                    system("pause");
                }
                }
            }
            else {
                printf("Puta bat zara!");
                system("pause");
            }
        }
        //去symbol表中寻找变量
        else if (search(hash)) {
            int type = current_id[Type];
            switch (type) {
                //int型
            case 1: {
                //循环输出的次数
                int time = current_id[Value];
                jump();
                if (*src == ',') {
                    src++;
                    jump();
                    //如果识别到‘“’
                    if (*src == '"') {
                        src++;
                        char *old_src = src;
                        //循环输出
                        while (time--) {
                            src = old_src;
                            myPrint();
                        }
                        match('\"');
                    }
                    else if ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z')) {
                        hash = *src;
                        src++;
                        while ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z') || (*src >= '0' && *src <= '9')) {
                            hash = hash * 131 + *src;
                            src++;
                        }
                        if (search(hash)) {
                            while (time--) {
                                switch (current_id[Type]) {
                                case 1: {
                                    //输出 int
                                    printf("%d", current_id[Value]);
                                    stream << current_id[Value];
                                    break;
                                }
                                case 2: {
                                    //输出real
                                    printf("%lf", *(double *)current_id[Value]);
                                    stream << *(double *)current_id[Value];
                                    break;
                                }
                                case 3: {
                                    //输出字符串
                                    cout << *(string *)current_id[Value];
                                    stream << *(string *)current_id[Value];
                                    break;
                                }
                                default: {
                                    printf("Error!");
                                    stream << "Error!";
                                    system("pause");
                                }
                                }
                            }
                        }
                    }
                }
                else if (*src == ';') {
                    printf("%d", current_id[Value]);
                    stream << current_id[Value];
                }
                break;
            }
            case 2: {
                if (current_id[Value]) {
                    printf("%lf", *(double *)current_id[Value]);
                    stream << *(double *)current_id[Value];
                }
                else {
                    printf("0.0");
                    stream << "0.0";
                }
                break;
            }
            case 3: {
                if (current_id[Value]) {
                    cout << *(string *)current_id[Value];
                    stream << *(string *)current_id[Value];
                }
                else {
                    printf("null");
                    stream << "null";
                }
                break;
            }
            default: {
                printf("Error:variable not found! (line %d)", line());
                stream << "Error:variable not found!  (line:" << line() << ")";
                system("pause");
            }
            }
        }
    }
    //������һ��������
    else if (*src == '"') {
        /*src++;
        myPrint();
        src++;*/
        stream << string_deal();

        //cout << s;
        jump();
        if (*src == ',') {
            src++;
            jump();
            int hash = *src;
            if ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z')) {
                src++;
                while ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z') || (*src >= '0' && *src <= '9')) {
                    hash = hash * 131 + *src;
                    src++;
                }
                //oldHash = hash;
                //hash = hash + location + flower * 149 + ifOrwhile;
                if (search(hash)) {
                    int type = current_id[Type];
                    switch (type) {
                    case 1: {
                        int a = current_id[Value];
                        printf("%d", a);
                        stream << a;
                        break;
                    }
                    case 2: {
                        double a = *(double *)current_id[Value];
                        printf("%lf", a);
                        stream << a;
                        break;
                    }
                    case 3: {
                        cout << *(string *)current_id[Value];
                        stream << *(string *)current_id[Value];
                        break;
                    }
                    default: {
                        printf("Error:variable error! (line %d)", line());
                        stream << "Error:variable error!  (line:" << line() << ")";
                        system("pause");
                    }
                    }
                }
            }
        }
    }
    else if (*src > '0' && *src <= '9') {
        /*int time = *src - '0';*/
        int time = (int)expression_six_enter();
        /*src++;
        while (*src >= '0' && *src <= '9') {
        time = time * 10 + *src - '0';
        src++;
        }*/
        jump();
        if (*src == ',') {
            src++;
            jump();
            //����������
            if (*src == '"') {
                //src++;
                //char *old_src = src;
                ////ѭ������
                //while (time--) {
                //	src = old_src;
                //	myPrint();
                //}
                //match('\"');
                string s = string_deal();
                while (time--) {
                    cout << s;
                    stream<<s;
                }
            }
            else if ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z')) {
                hash = *src;
                src++;
                while ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z') || (*src >= '0' && *src <= '9')) {
                    hash = hash * 131 + *src;
                    src++;
                }
                if (search(hash)) {
                    switch (current_id[Type]) {
                    case 1: {
                        //���� int
                        /*printf("%d", current_id[Value]);
                        stream << current_id[Value];
                        break;*/
                        int result = (int)expression_six_enter();
                        while (time--) {
                            printf("%d", result);
                            stream<<result;
                        }
                        break;
                    }
                    case 2: {
                        //����real
                        /*printf("%lf", *(double *)current_id[Value]);
                        stream << *(double *)current_id[Value];
                        break;*/
                        double result = expression_six_enter();
                        while (time--) {
                            printf("%lf", result);
                            stream<<result;
                        }
                        break;
                    }
                    case 3: {
                        //�����ַ���
                        /*cout << *(string *)current_id[Value];
                        stream << *(string *)current_id[Value];
                        break;*/
                        point_back();
                        string s;
                        s = string_deal();
                        while (time--) {
                            cout << s;
                            stream<<s;
                        }
                        break;
                    }
                    default: {
                        printf("Error:can't search! (line %d)", line());
                        stream << "Error:can't search!  (line:" << line() << ")";
                        system("pause");
                    }
                    }
                }
            }
            //����
            else {
                printf("Error:wrong argument! (line %d)", line());
                stream << "Error:wrong argument!  (line:" << line() << ")";
                system("pause");
            }
        }
        //����û��ƥ�䵽������������
        else {
            printf("Error:wrong argument! (line %d)", line());
            stream << "Error:wrong argument!  (line:" << line() << ")";
            system("pause");
        }

    }
    match(';');
}
//解析表达式
int MainWindow::expression(int type) {
    if (type == IF) {
        if (if_function())return 1;
    }
    else if (type == WHILE) {
        if (while_function())return 1;
    }
    else if (type == DO) {
        if (do_function())return 1;
    }
    else if (type == IN) {
        //去掉空格
        jump();
        //判断in关键字后是否有"
        if (*src == '"') {
            //输出引号里的内容
            inHintStr = string_deal();
            //判断引号结束以后是否有逗号
            if (*src == ',') {
                src++;
                //进入解析In的函数
                parseIn();
            }
            //没有匹配到逗号报错
            else {
                printf("Error:wrong arguments! (line %d)", line());
                stream << "Error:wrong argument!  (line:" << line() << ")";
                system("pause");
            }
        }
        else if ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z')) {
            //进入解析In的函数
            parseIn();
        }
        else {
            printf("Error:wrong argument! (line %d)", line());
            stream << "Error:wrong argument!  (line:" << line() << ")";
            system("pause");
        }
        jump();
        if (*src == ';') {
            src++;
            return 0;
        }
        else {
            printf("Error:wrong argument! (line %d)", line());
            stream << "Error:wrong argument!  (line:" << line() << ")";
            system("pause");
        }
    }
    else if (type == OUT) {
        parseOut();
    }
    else if (type == Return) {
        if (return_function())return 1;
    }
    else if (type == STRING) {
        point_back();
        string_deal();
        if (*src == ';') {
            src++;
        }
    }
    else if (type == INT || type == REAL) {
        point_back();
        expression_six_enter();
        if (*src == ';') {
            src++;
        }
    }
    else {
        printf("ERROR: unknown type : in line %d", line());
        stream << "ERROR: unknown type : in line " << line();
        system("pause");
    }
    return 0;
}
//解析int、real、string类型后对标识符赋值
void MainWindow::definition(int type) {
    jump();
    int hash;
    while (*src != ';') {
        //记录标识符的地址
        char *last_pos = src;
        if (*src == '\n') {
            printf("Error:wrong arguments! (line %d)", line());
            stream << "Error:wrong argument!  (line:" << line() << ")";
        }
        else if (*src == ' ' || *src == '\t') {
            src++;
            continue;
        }
        else if ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z')) {
            hash = *src;
            src++;
            while ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z') || (*src >= '0' && *src <= '9')) {
                hash = hash * 131 + *src;
                src++;
            }
            current_id = symbols;
            int k = 0;
            while (current_id[Token]) {
                k++;
                if (current_id[Token] == Sys && current_id[Hash] == hash) {
                    if (k == Main && !flagMain) {
                        idMain = (int *)last_pos;
                        flagMain = 1;
                    }
                    else {
                        printf("Error:wrong arguments!(line %d)", line());
                        stream << "Error:wrong argument!  (line:" << line() << ")";
                        system("pause");
                    }
                }
                current_id += IdSize;
            }
            jump();
            //如果此时为(则说明为函数声明
            if (*src == '(') {
                char *identifier = last_pos;
                src++;
                int wwwlz = function_parameter();
                current_id = symbols;
                int k = 0;
                while (current_id[Token]) {
                    k++;
                    if (current_id[Hash] == hash && k != 9 && current_id[WwwLz] == wwwlz) {
                        printf("Error:Repeated function declaration (line %d)", line());
                        stream << "Error:Repeated function declaration  (line:" << line() << ")";
                        system("pause");
                    }
                    current_id += IdSize;
                }
                current_id[Token] = Fun;
                current_id[Hash] = hash;
                current_id[Address] = (int)identifier;
                current_id[Flower] = flower;
                current_id[Type] = type;
                current_id[WwwLz] = wwwlz;
                symbols_tail = current_id + IdSize;
                //判断是main函数，则需要解析函数体；.,
                current_id = symbols;
                current_id = current_id + IdSize * 8;
                if (current_id[Hash] == hash) {
                    //解析main函数的参数
                    if (match('{')) flower++;
                    //解析main函数的函数体
                    program();
                    /*match('}');*/
                    break;
                }
                else {
                    int back = 1;
                    //跳过函数体
                    jump();
                    if (*src == '{') {
                        back = 1;
                        while (back) {
                            src++;
                            if (*src == '{') {
                                back++;
                            }
                            else if (*src == '}') {
                                back--;
                            }
                        }
                    }
                    src++;
                    return;
                }

            }
            //为,则为变量声明，继续读取后续的变量
            else if (*src == ',') {
                current_id = symbols;
                while (current_id[Token]) {
                    if (current_id[Hash] == hash && current_id[Flower] == flower) {
                        printf("Error:Repeated variable declaration (line %d)", line());
                        stream << "Error:Repeated function declaration  (line:" << line() << ")";
                        system("pause");
                    }
                    current_id += IdSize;
                }
                //存储新变量
                current_id[Token] = Id;
                current_id[Hash] = hash;
                current_id[Address] = (int)last_pos;
                current_id[Flower] = flower;
                current_id[Type] = type;
                //为double型变量和string型变量申请空间
                if (type == REAL) {
                    double b = 0;
                    d.push(b);
                    current_id[Value] = (int)&d.top();
                }
                else if (type == STRING) {
                    string b;
                    s.push(b);
                    current_id[Value] = (int)&s.top();
                }
                symbols_tail = current_id + IdSize;
                src++;
                jump();
                if (*src == ';') {
                    printf("Error:wrong arguments! (line %d)", line());
                    stream << "Error:wrong arguments!  (line:" << line() << ")";
                    system("pause");
                }
                continue;
            }
            //变量声明结束
            else if (*src == ';') {
                current_id = symbols;
                while (current_id[Token]) {
                    if (current_id[Hash] == hash && current_id[Flower] == flower) {
                        printf("Error:Repeated variable declaration (line %d)", line());
                        stream << "Error:Repeated variable declaration  (line:" << line() << ")";
                        system("pause");
                    }
                    current_id += IdSize;
                }
                //存储新变量
                current_id[Token] = Id;
                current_id[Hash] = hash;
                current_id[Address] = (int)last_pos;
                current_id[Type] = type;
                current_id[Flower] = flower;
                if (type == REAL) {
                    double b = 0;
                    d.push(b);
                    current_id[Value] = (int)&d.top();
                }
                else if (type == STRING) {
                    string b;
                    s.push(b);
                    current_id[Value] = (int)&s.top();
                }
                symbols_tail = current_id + IdSize;
                src++;
                break;
            }
            else if (*src == '=') {//改动 林伟涛 2017.9.10
                current_id = symbols;
                while (current_id[Token]) {
                    if (current_id[Hash] == hash && current_id[Flower] == flower) {
                        printf("Error:Repeated variable declaration (line %d)", line());
                        stream << "Error:Repeated variable declaration  (line:" << line() << ")";
                        system("pause");
                    }
                    current_id += IdSize;
                }
                //存储新变量
                current_id[Token] = Id;
                current_id[Hash] = hash;
                current_id[Address] = (int)last_pos;
                current_id[Flower] = flower;
                current_id[Type] = type;
                symbols_tail = current_id + IdSize;
                //为double型变量和string型变量申请空间
                if (type == 2) {
                    double b = 0;
                    d.push(b);
                    current_id[Value] = (int)&d.top();
                }
                else if (type == 3) {
                    string b;
                    s.push(b);
                    current_id[Value] = (int)&s.top();
                }
                src = last_pos;
                if (type == INT || type == REAL)
                    expression_six_enter();
                else string_deal();
                jump();
                if (*src == ',') {
                    src++;
                    continue;
                }
                else if (*src == ';') {
                    src++;
                    break;
                }

            }
            //错误的语句
            else {
                printf("Error:wrong arguments (line %d)", line());
                stream << "Error:wrong arguments  (line:" << line() << ")";
                system("pause");
            }
        }
    }
}

//取下一个标记，并识别当前标记是否正确
int MainWindow::match(char a) {
    //遇到空格 换行 tab 指针后跳
    while (*src == ' ' || *src == '\n' || *src == '\t') {
        src++;
    }
    //如果一直遇不到语法定义必须的字符，证明输入代码有错，直接报错，结束程序
    if (*src != a) {
        printf("Less of %c (line %d )", a, line());
        stream << "Less of " << a << " (line " << line() << ")";
        system("pause");
    }
    //指针跳到需要匹配的字符地址处
    src++;
    //跳过需要匹配字符后面的空格 换行 tab
    while (*src == ' ' || *src == '\n' || *src == '\t') {
        src++;
    }
    return 1;

}

//识别后一个单元（变量或者数字，函数名，if-else，while，复合句），传递这个单元的类型到type_value
int MainWindow::next(int &type_value) {
    jump();

    if ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z')) {
        //计算当前标识符的hash值
        int hash = *src;
        //变量的类型
        int type = 0;
        int i = 0;
        //查找变量声明语句
        src++;
        while ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z') || (*src >= '0' && *src <= '9')) {
            hash = hash * 131 + *src;
            src++;
        }
        if (search(hash)) {
            if (current_id[Token] == Id || current_id[Token] == Fun) {
                type_value = current_id[Type];
                return 1;
            }
            else return 0;

        }
    }
    else if (*src >= '0' && *src <= '9') {
        type_value = 1;
        return 1;
    }
    else if (*src == '-' || *src == '+') {
        type_value = 1;
        return 1;
    }
}

//匹配参数列表
void MainWindow::function_resolve() {
    jump();
    if (*src == '(') {
        src++;
        int *tail = symbols_tail;
        int wwwlz = 0;
        while (*src != ')') {
            jump();
            if ((*src > '0' && *src <= '9')) {
                /*float mod = 0.1;*/
                double num = expression_six_enter();
                int _num = (int)num;
                /*src++;*/
                //while ((*src >= '0' && *src <= '9') || *src == '.') {
                //	if (*src >= '0' && *src <= '9') {
                //		if (mod == 0.1) {
                //			num = num / mod + *src - '0';
                //		}
                //		else {
                //			num += (*src - '0') / mod;
                //			mod *= 10;
                //		}
                //	}
                //	else if (*src == '.') {
                //		if (mod == 10) {
                //			printf("Error:wrong in (line %d)!", line);
                //			//system("pause");
                //		}
                //		mod = 10;
                //	}
                //	src++;
                //}
                current_id = symbols_tail;
                current_id[Type] = (_num - num == 0) ? INT : REAL;
                if (current_id[Type] == INT) {
                    current_id[Value] = _num;
                }
                else {
                    d.push(num);
                    current_id[Value] = (int)&d.top();
                }
                current_id[Token] = Id;
                current_id[Flower] = flower + 1;
                symbols_tail += IdSize;
            }
            else if ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src >= 'Z')) {
                int type;
                next(type);
                point_back();
                string snum;
                double num;
                if (type == STRING) {
                    snum = string_deal();
                }
                else {
                    num = expression_six_enter();
                }
                current_id = symbols_tail;
                current_id[Type] = type;
                if (current_id[Type] == INT) {
                    current_id[Value] = (int)num;
                }
                else if (current_id[Type] == REAL) {
                    d.push(num);
                    current_id[Value] = (int)&d.top();
                }
                else if (current_id[Type] == STRING) {
                    s.push(snum);
                    current_id[Value] = (int)&s.top();
                }
                current_id[Token] = Id;
                current_id[Flower] = flower + 1;
                symbols_tail += IdSize;

            }
            else if (*src == '\"') {
                string a;
                src++;
                while (*src != '\"') {
                    a = a + *src;
                    if (*src == '\n') {
                        printf("Error:less of \" (line %d)", line());
                        stream << "Error:less of \"  (line:" << line() << ")";
                        system("pause");
                    }
                    src++;
                }
                src++;
                s.push(a);
                current_id = symbols_tail;
                current_id[Value] = (int)&s.top();
                current_id[Token] = Id;
                current_id[Type] = STRING;
                current_id[Flower] = flower + 1;
                symbols_tail += IdSize;
            }
            //else if ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src >= 'Z')) {
            //	/*int hash = *src;
            //	src++;
            //	while ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z') || (*src >= '0' && *src <= '9')) {
            //		hash = hash * 131 + *src;
            //		src++;
            //	}
            //	if (search(hash)) {
            //		int i = current_id[Type];
            //		int a = current_id[Value];
            //		current_id = symbols_tail;
            //		current_id[Token] = Id;
            //		current_id[Type] = i;
            //		current_id[Value] = a;
            //		current_id[Flower] = flower + 1;
            //		symbols_tail += IdSize;
            //	}*/
            //	double num = expression_six_enter();

            //}
            jump();
            if (*src == ',') {
                src++;
                jump();
                continue;
            }
            else if (*src == '\n') {
                printf("Error:less of ')' (line %d)", line());
                stream << "Error:less of ')' (line:" << line() << ")";
                system("pause");
            }
        }
        src++;
        jump();
    }
}

//匹配函数
int MainWindow::function_match(int *tail) {
    int *taill = tail;
    src = (char *)current_id[Address];
    int type = current_id[Type];
    while (*src != '(') {
        src++;
    }
    src++;
    jump();
    while (*src != ')') {
        if (taill[Token] == 0) {
            return 0;
        }
        if ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z')) {
            //计算当前标识符的hash值
            int hash = *src++;
            //变量的类型
            int type = 0;
            int i = 0;
            //查找变量声明语句
            while ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z') || (*src >= '0' && *src <= '9')) {
                hash = hash * 131 + *src;
                src++;
            }
            current_id = symbols;
            while (current_id[Token] && i < 3) {
                i++;
                if (current_id[Hash] == hash && current_id[Token] == Sys) {
                    type = i;
                    current_id += IdSize;
                    break;
                }
                current_id += IdSize;
            }
            if (!type) {
                printf("Error:wrong arguments in (line %d)", line());
                stream << "Error:wrong arguments in (line:" << line() << ")";
                system("pause");
            }
            else {
                if (type == taill[Type]) {
                    src++;
                    jump();
                    if ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z')) {
                        int hash = *src++;
                        while ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z') || (*src >= '0' && *src <= '9')) {
                            hash = hash * 131 + *src;
                            src++;
                        }
                        taill[Hash] = hash;
                        taill += IdSize;
                        if (taill[Token] != 0) {
                            jump();
                            if (*src == ',') {
                                src++;
                                jump();
                                if (*src == ')') {
                                    printf("Error:less of variable define (line %d)", line());
                                    stream << "Error:less of variable define (line:" << line() << ")";
                                    system("pause");
                                }
                            }
                            continue;
                        }
                    }
                    else {
                        printf("Error:wrong arguments in (line %d)", line());
                        stream << "Error:wrong arguments in  (line:" << line() << ")";
                        system("pause");
                    }
                }
                else {
                    return 0;
                }
            }

            jump();

            //if (*src == ',') {
            //	src++;
            //	jump();
            //	if (*src == ')') {
            //		printf("Error:less of variable define (line %d)", line());
            //		system("pause");
            //	}
            //}
        }
        else {
            printf("Error:less of variable define (line %d)", line());
            stream << "Error:less of variable define  (line:" << line() << ")";
            system("pause");
        }
    }
    if (taill[Token] != 0) {
        return 0;
    }
    return 1;
}

//解析函数体
void MainWindow::function_body() {
    src = (char *)current_id[Address];
    while (*src != '{') {
        src++;
    }
    while (*src != 0) {
        if (*src == ' ' || *src == '\t') {
            src++;
            continue;
        }
        else if (*src == '\n') {
            src++;
            continue;
        }
        else if ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z')) {
            //计算当前标识符的hash值
            int hash = *src;
            //记录关键字的地址
            char *last_pos = src - 1;
            //变量的类型
            int type = 0;
            int i = 1;
            int have_use = 0;
            //查找变量声明语句
            src++;
            while ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z') || (*src >= '0' && *src <= '9')) {
                hash = hash * 131 + *src;
                src++;
            }
            current_id = symbols;
            while (current_id[Token] && i <= STRING) {
                if (current_id[Hash] == hash && current_id[Token] == Sys) {
                    type = i++;
                    current_id += IdSize;
                    break;
                }
                i++;
                current_id += IdSize;
            }
            //如果小于等于3，则说明要声明变量或函数
            if (type > 0 && type <= 3) {
                definition(type);
            }
            else if (!flower) {
                printf("Error:error! (line %d)", line());
                stream << "Error:error!  (line:" << line() << ")";
                system("pause");
            }
            else {
                //识别if、while等语句
                while (current_id[Token] && i >= WHILE && i <= DO) {
                    if (current_id[Hash] == hash && current_id[Token] == Sys) {
                        //					ifOrwhile = (int)last_pos;
                        if (expression(i)) {
                            flower = mf.top() - 1;
                            destroy(mf.top());
                            return;
                        };
                        i++;
                        //					ifOrwhile = 0;
                        break;
                    }
                    i++;
                    current_id += IdSize;
                }
                //识别in、out语句
                while (current_id[Token] && i >= 7 && i <= 8) {
                    if (current_id[Hash] == hash && current_id[Token] == Sys) {
                        type = i++;
                        expression(type);
                        break;
                    }
                    i++;
                    current_id += IdSize;
                }
                //跳过main语句识别
                while (current_id[Token] && i == Main) {
                    current_id += IdSize;
                    i++;
                }
                //识别return语句
                while (current_id[Token] && i == Return) {
                    if (current_id[Hash] == hash && current_id[Token] == Sys) {
                        type = i++;
                        expression(type);
                        flower = mf.top() - 1;
                        destroy(mf.top());
                        return;
                    }
                    i++;
                    current_id += IdSize;
                }
                int flag = 0;
                int *tail = symbols_tail;
                char *old_src;
                while (current_id[Token] && i > 10) {
                    if (current_id[Hash] == hash && current_id[Token] == Id && ((current_id[Type] == INT) || (current_id[Type] == REAL) || (current_id[Type] == STRING))) {
                        expression(current_id[Type]);
                        break;
                    }
                    else if (current_id[Hash] == hash && current_id[Token] == Fun) {
                        int *current = current_id;
                        //匹配函数并解析参数列表
                        if (!flag) {
                            function_resolve();
                            current_id = current;
                            flag = 1;
                            mf.push(flower + 1);
                        }
                        old_src = src;
                        if (function_match(tail)) {
                            src = old_src;
                            current_id = current;
                            //解析函数体
                            old_src = src;
                            function_body();
                            src = old_src;
                            current_id = current;
                            mf.pop();
                            break;
                        }
                        else {
                            current_id = current;
                            src = old_src;
                        }
                    }
                    i++;
                    current_id += IdSize;
                }
            }
        }
        else if (*src >= '0'&& *src <= '9') {
            point_back();
            expression_six_enter();
            match(';');
            continue;
        }
        else if (*src == ';') {
            src++;
        }
        if (*src == '{') {
            flower++;
            src++;
        }
        else if (*src == '}') {
            src++;
            destroy(flower);
            flower--;
            if (flower < mf.top()) {
                return;
            }
        }
    }

}

//表达式计算开始的时候指针调到表达式开头
void MainWindow::point_back() {
    src--;
    while (*src != ' ' && *src != '\n' && *src != '\t'&& *src != 0 && *src != '('&&*src != ',') {
        src--;
    }
    src++;

}

//字符串处理（优先级2：赋值=   ；优先级1：连接$,删除#）
//格式   string型变量+ 符号“=” + 字符串 + 符号“$” 字符串   （字符串可以是 string型变量 或者 “abc。。。”） 如 aa="sfdvds"$bb;
/*格式   string型变量+ 符号“=” + 字符串 + 符号“#” 结果是数字的表达式，如1+2   （字符串可以是 string型变量 或者 “abc。。。”）
如  aa= "sagfgvxz" # 1+2;
*/
//使用递归下降的算法
string MainWindow::string_factor() {
    string value;
    //遇到括号就再次进行表达式运算expression_six_enter（）
    while (*src == ' ' || *src == '\n') {
        src++;
    }
    if (*src == '(') {
        match('(');
        value = string_deal();
        match(')');
    }
    else if (*src == '"') {
        src++;//2017.9.13修改
        while (*src != '"') {
            if (*src == '\\') {
                src++;
                if (*src == '0') value.append(1, '\0');
                else if (*src == '\\') value.append(1, '\\');
                else if (*src == 'n') value.append(1, '\n');
                else if (*src == '"') value.append(1, '\"');
                else if (*src == 't') value.append(1, '\t');
                src++;
            }
            else {
                value.append(1, *src);
                src++;
            }

        }
        match('"');
    }
    //变量识别和函数识别
    else if (*src >= 'a' && *src <= 'z' || *src >= 'A' && *src <= 'Z') {
        //变量名或函数名的识别
        int hash = *src;
        src++;
        while ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z') || (*src >= '0' && *src <= '9')) {
            hash = hash * 131 + *src;
            src++;
        }
        if (search(hash)) {
            jump();
            int type = current_id[Type];

            if (current_id[Token] == Fun) {
                int flag = 0;
                int *tail = symbols_tail;
                while (current_id[Token]) {
                    if (current_id[Hash] == hash && current_id[Token] == Fun) {
                        int *current = current_id;
                        //匹配函数并解析参数列表
                        if (!flag) {
                            function_resolve();
                            current_id = current;
                            flag = 1;
                            mf.push(flower + 1);
                        }
                        char *old_src = src;
                        if (function_match(tail)) {
                            src = old_src;
                            current_id = current;
                            //解析函数体
                            old_src = src;
                            function_body();
                            src = old_src;
                            current_id = current;
                            mf.pop();
                            jump();
                            return return_string;
                        }
                        else {
                            current_id = current;
                            src = old_src;
                        }
                    }
                    current_id += IdSize;
                }
            }

            else if (current_id[Token] == Id) {
                if (*src == '='&&*(src + 1) != '=') {
                    src++;
                    jump();
                    string *location = (string *)current_id[Value];
                    *location = string_deal();
                    value = *location;
                }
                else return *(string *)current_id[Value];
            }
        }
        else {
            string gg;
            return gg;
            //printf("Error:can't recognized (line %d)", line());
            //stream << "Error:can't recognized   (line:" << line() << ")";
            //system("pause");
        }
    }
    while (*src == ' ' || *src == '\n') {
        src++;
    }
    return value;
}
//识别字符串操作符$,#后进行相应操作
string MainWindow::string_deal_tail(string lvalue) {
    if (*src == '$') {
        match('$');
        string value = lvalue + string_factor();
        return string_deal_tail(value);
    }
    else if (*src == '#') {
        match('#');
        int num;
        num = (int)three();
        if (num > lvalue.length()) {
            printf("Error:# > string.length() (line %d)", line());
            stream << "Error:# > string.length()   (line:" << line() << ")";
            system("pause");
        }
        string value = lvalue.erase(num - 1, 1);
        return string_deal_tail(value);
    }
    else {
        return lvalue;
    }
}
//
string MainWindow::string_deal() {
    string lvalue = string_factor();
    return string_deal_tail(lvalue);
}
double MainWindow::string_cmp_tail(string lvalue) {
    if (*src == '>') {
        match('>');
        if (*src == '=') {
            match('=');
            double value = lvalue >= string_deal();
            return value;
        }
        else {
            double value = lvalue>string_deal();
            return value;
        }
    }
    else if (*src == '<') {
        match('<');
        if (*src == '=') {
            match('=');
            double value = lvalue <= string_deal();
            return value;
        }
        else if (*src == '>') {
            match('>');
            double value = lvalue != string_deal();
            return value;
        }
        else {
            double value = lvalue<string_deal();
            return value;
        }
    }
    else if (*src == '=') {
        match('=');
        match('=');
        double value = lvalue == string_deal();
        return value;
    }
    else{
        printf("Error:can't recognized (line %d)", line());
        stream << "Error:can't recognized   (line:" << line() << ")";
        system("pause");
    }
}
//
double MainWindow::string_cmp() {
    string lvalue = string_deal();
    return string_cmp_tail(lvalue);
}
//第六级运算，&&；||
double MainWindow::string_enter_tail(double lvalue) {
    if (*src == '&') {
        src++;
        match('&');
        double value = string_cmp() && lvalue;//five（）和lvalue的位置不能换，否则指针不往后跳，玄学。日了狗了
        return string_enter_tail(value);
    }
    else if (*src == '|') {
        src++;
        match('|');
        double value = string_cmp() || lvalue;//five（）和lvalue的位置不能换，否则指针不往后跳，玄学。
        return string_enter_tail(value);
    }
    else {
        return lvalue;
    }
}
double MainWindow::string_enter() {
    double lvalue = string_cmp();
    return string_enter_tail(lvalue);
}




/*
使用递归下降的方法
已消除左递归
如one_tail();
one();
*/
//最小单位和单目运算-，+（正负运算）；！非运算；（）括号运算
double MainWindow::factor() {
    double value = 0;
    //遇到括号就再次进行表达式运算expression_six_enter（）
    while (*src == ' ' || *src == '\n') {
        src++;
    }
    if (*src == '(') {
        match('(');
        value = expression_six_enter();
        match(')');
    }
    else if (*src == '-') {
        match('-');
        value = (-factor());
    }
    else if (*src == '+') {
        match('+');
        value = (factor());
    }
    else if (*src == '!') {
        match('!');
        value = (!factor());
    }
    else if (*src == '\'') {
        match('\'');
        value = *src;
        src++;
        jump();
        if (*src != '\'') {
            printf("Error:error! (line %d)", line());
            stream << "Error:error!  (line:" << line() << ")";
            system("pause");
        }
        else src++;
        return value;
    }
    else if(*src=='\"'){
        value=string_enter();
    }
    else if (*src >= '0' && *src <= '9') {
        value = *src - '0';
        src++;
        if (value >= 0) {
            while (*src >= '0' && *src <= '9') {
                value = value * 10 + (*src - '0');
                src++;
            }
            if (*src == '.') {
                src++;
                double i = 0.1;
                while (*src >= '0' && *src <= '9') {
                    value += (*src - '0')*i;
                    i *= 0.1;
                    src++;
                }
            }
        }
    }
    //变量识别和函数识别
    else if (*src >= 'a' && *src <= 'z' || *src >= 'A' && *src <= 'Z') {

        //变量名或函数名的识别
        int hash = *src;
        char *loc = src;
        src++;
        while ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z') || (*src >= '0' && *src <= '9')) {
            hash = hash * 131 + *src;
            src++;
        }
        //从sympols表后开始寻找变量或者是函数并解析
        if (search(hash)) {
            jump();
            int type = current_id[Type];

            if (current_id[Token] == Fun) {
                int flag = 0;
                int *tail = symbols_tail;
                while (current_id[Token]) {
                    if (current_id[Hash] == hash && current_id[Token] == Fun) {
                        int *current = current_id;
                        //匹配函数并解析参数列表
                        if (!flag) {
                            function_resolve();
                            current_id = current;
                            flag = 1;
                            mf.push(flower + 1);
                        }
                        char *old_src = src;
                        if (function_match(tail)) {
                            src = old_src;
                            current_id = current;
                            //解析函数体
                            old_src = src;
                            function_body();
                            src = old_src;
                            current_id = current;
                            mf.pop();
                            jump();
                            return return_real;
                        }
                        else {
                            current_id = current;
                            src = old_src;
                        }
                    }
                    current_id += IdSize;
                }
            }

            else if (current_id[Token] == Id) {
                switch (type)
                {
                case 1: {
                    if (*src == '='&&*(src + 1) != '=') {
                        src++;
                        jump();
                        int *location = &current_id[Value];
                        //
                        int type;
                        char *add = src;
                        next(type);
                        src = add;
                        int flag_ifelse;
                        flag_ifelse = (int)expression_six_enter();
                        *location = flag_ifelse;
                        value = *location;
                    }
                    else return current_id[Value];
                    break;
                }
                case 2: {
                    if (*src == '='&&*(src + 1) != '=') {
                        src++;
                        jump();
                        double *location = (double *)current_id[Value];
                        //
                        int type;
                        char *add = src;
                        src = add;
                        double flag_ifelse;
                        flag_ifelse = expression_six_enter();
                        *location = flag_ifelse;
                        value = *location;
                    }
                    else return *(double *)current_id[Value];
                    break;
                }
                case 3: {
                    src = loc;
                    return string_enter();
                }
                default:
                    printf("Error:variable not found! (line %d)", line());
                    stream << "Error:variable not found!  (line:" << line() << ")";
                    system("pause");
                    break;
                }
            }

        }
        //如果找不到变量
        else {
            if (*src == '='&&*(src + 1) != '=') {
                printf("Error:Variable not defined (line %d)", line());
                stream << "Error:Variable not defined  (line:" << line() << ")";
                system("pause");
            }
            else value = 0;
        }
    }
    else {
        printf("Error:can't recognized (line %d)", line());
        stream << "Error:can't recognized   (line:" << line() << ")";
        system("pause");
    }
    jump();
    return value;
}

//第一级运算，乘方^
double MainWindow::one_tail(double lvalue) {
    if (*src == '^') {
        match('^');
        double value = pow(lvalue, one_tail(factor()));//乘方的结和性为从右向左
                                                       //double value = pow(lvalue, factor());
        return value;
    }
    else {
        return lvalue;
    }
}
double MainWindow::one() {
    double lvalue = factor();
    return one_tail(lvalue);
}
//第二级运算，*；/；%
double MainWindow::two_tail(double lvalue) {

    if (*src == '*') {
        match('*');
        double value = lvalue * one();
        return two_tail(value);
    }
    else if (*src == '/') {
        match('/');
        double temp = one();
        if (temp == 0) {
            printf("Error: /!0 ( line%d)", line());
            stream << "Error: /!0  (line:" << line() << ")";
            system("pause");
        }
        double value = lvalue / temp;
        return two_tail(value);
    }
    else if (*src == '%') {
        match('%');
        if (int(lvalue) != lvalue) {
            printf("Error: int%%int ( line%d)", line());
            stream << "Error: int%%int  (line:" << line() << ")";
            system("pause");
        }
        double temp = one();
        if (int(temp) != temp) {
            printf("Error int%%int : line%d\n", line());
            stream << "Error: int%%int  (line:" << line() << ")";
            system("pause");
        }
        double value = (int)lvalue % (int)temp;
        return two_tail(value);
    }
    else {
        return lvalue;
    }
}
double MainWindow::two() {
    double lvalue = one();
    return two_tail(lvalue);
}
//第三级运算，+；-
double MainWindow::three_tail(double lvalue) {
    if (*src == '+') {
        match('+');
        double value = lvalue + two();
        return three_tail(value);
    }
    else if (*src == '-') {
        match('-');
        double value = lvalue - two();
        return three_tail(value);
    }

    else {
        return lvalue;
    }
}
double MainWindow::three() {
    double lvalue = two();
    return three_tail(lvalue);
}
//第四级运算，>；>=；<；<=
double MainWindow::four_tail(double lvalue) {
    if (*src == '>') {
        match('>');
        if (*src == '=') {
            src++;
            double value = lvalue >= three();
            return four_tail(value);
        }
        else {
            double value = lvalue > three();
            return four_tail(value);
        }
    }
    else if (*src == '<'&&*(src + 1) != '>') {
        match('<');
        if (*src == '=') {
            src++;
            double value = lvalue <= three();
            return four_tail(value);
        }
        else {
            double value = lvalue < three();
            return four_tail(value);
        }
    }
    else {
        return lvalue;
    }
}
double MainWindow::four() {
    double lvalue = three();
    return four_tail(lvalue);
}
//第五级运算，==；<>；
double MainWindow::five_tail(double lvalue) {
    if (*src == '='&&*(src + 1) == '=') {
        src++;
        match('=');
        double value = (lvalue == four());
        return five_tail(value);
    }
    if (*src == '<'&&*(src + 1) == '>') {
        src++;
        match('>');
        double value = (lvalue != four());
        return five_tail(value);
    }
    else {
        return lvalue;
    }
}
double MainWindow::five() {
    double lvalue = four();
    return five_tail(lvalue);
}
//第六级运算，&&；||
double MainWindow::expression_six_enter_tail(double lvalue) {
    if (*src == '&') {
        src++;
        match('&');
        double value = five() && lvalue;//five（）和lvalue的位置不能换，否则指针不往后跳，玄学。日了狗了
        return expression_six_enter_tail(value);
    }
    else if (*src == '|') {
        src++;
        match('|');
        double value = five() || lvalue;//five（）和lvalue的位置不能换，否则指针不往后跳，玄学。
        return expression_six_enter_tail(value);
    }
    else {
        return lvalue;
    }
}
double MainWindow::expression_six_enter() {
    double lvalue = five();
    return expression_six_enter_tail(lvalue);
}

//if语句和while语句内的函数体解析
int MainWindow::inside() {
    while (*src != 0) {
        if (*src == ' ' || *src == '\t') {
            src++;
            continue;
        }
        else if (*src == '/' || *(src + 1) == '/') {
            src += 2;
            while (*src != '\n') {
                src++;
            }
            continue;
        }
        else if (*src == '\n') {
            src++;
            continue;
        }
        else if ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z')) {
            //计算当前标识符的hash值
            int hash = *src;
            //记录关键字的地址
            char *last_pos = src - 1;
            //变量的类型
            int type = 0;
            int i = 1;
            int have_use = 0;
            //查找变量声明语句
            src++;
            while ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z') || (*src >= '0' && *src <= '9')) {
                hash = hash * 131 + *src;
                src++;
            }
            current_id = symbols;
            while (current_id[Token] && i <= STRING) {
                if (current_id[Hash] == hash && current_id[Token] == Sys) {
                    type = i++;
                    current_id += IdSize;
                    break;
                }
                i++;
                current_id += IdSize;
            }
            //如果小于等于3，则说明要声明变量或函数
            if (type > 0 && type <= 3) {
                definition(type);
            }
            else if (!flower) {
                printf("Error:error! (line %d)", line());
                stream << "Error: error!  (line:" << line() << ")";
                system("pause");
            }
            else {
                //识别if、while等语句
                while (current_id[Token] && i >= WHILE && i <= DO) {
                    if (current_id[Hash] == hash && current_id[Token] == Sys) {

                        if (expression(i)) {
                            flower = mf.top() - 1;
                            destroy(mf.top());
                            return 1;
                        }

                        i++;
                        break;
                    }
                    i++;
                    current_id += IdSize;
                }
                //识别in、out语句
                while (current_id[Token] && i >= 7 && i <= 8) {
                    if (current_id[Hash] == hash && current_id[Token] == Sys) {
                        type = i++;
                        expression(type);
                        break;
                    }
                    i++;
                    current_id += IdSize;
                }
                //跳过main语句识别
                while (current_id[Token] && i == Main) {
                    current_id += IdSize;
                    i++;
                }
                //识别return语句
                while (current_id[Token] && i == Return) {
                    if (current_id[Hash] == hash && current_id[Token] == Sys) {
                        type = i;
                        if (expression(i)) {
                            flower = mf.top() - 1;
                            destroy(mf.top());
                            return 1;
                        }
                    }
                    i++;
                    current_id += IdSize;
                }
                int flag = 0;
                int *tail = symbols_tail;
                char *old_src;
                while (current_id[Token] && i > 10) {
                    if (current_id[Hash] == hash && current_id[Token] == Id && ((current_id[Type] == INT) || (current_id[Type] == REAL) || (current_id[Type] == STRING))) {
                        expression(current_id[Type]);
                        break;
                    }
                    else if (current_id[Hash] == hash && current_id[Token] == Fun) {

                        int *current = current_id;
                        //匹配函数并解析参数列表
                        if (!flag) {
                            function_resolve();
                            current_id = current;
                            flag = 1;
                            mf.push(flower + 1);
                        }
                        old_src = src;
                        if (function_match(tail)) {
                            src = old_src;
                            current_id = current;
                            //解析函数体
                            old_src = src;
                            function_body();
                            src = old_src;
                            current_id = current;
                            mf.pop();
                            jump();
                            if (*src == ';') src++;
                            break;
                        }
                        else {
                            current_id = current;
                            src = old_src;
                        }
                    }
                    i++;
                    current_id += IdSize;
                }
            }
        }
        else if (*src >= '0'&& *src <= '9') {
            point_back();
            expression_six_enter();
            match(';');
            continue;
        }
        //else if (*src == '{') {
        //	src++;
        //	flower++;
        //}
        else if (*src == '}') {
            src++;
            destroy(flower);
            flower--;
            return 0;
        }
        else if (*src == ';') {
            src++;
            continue;
        }
    }
}




//if-else功能实现
int MainWindow::if_function() {
    int flag_ifelse = int(expression_six_enter());
    if (flag_ifelse) {
        if (match('{')) flower++;
        if (inside()) return 1;
        jump();
        if (*src == 'e'&&*(src + 1) == 'l'&&*(src + 2) == 's'&&*(src + 3) == 'e') {
            src += 4;
            match('{');
            int flower_if = 1;
            while (1) {

                if (*src == '{') flower_if++;
                else if (*src == '}') flower_if--;
                src++;
                if (flower_if == 0)break;

            }

        }
    }
    else {
        match('{');
        int flower_if = 1;
        while (1) {

            if (*src == '{') flower_if++;
            else if (*src == '}') flower_if--;
            src++;
            if (flower_if == 0)break;
        }
        jump();
        if (*src == 'e'&&*(src + 1) == 'l'&&*(src + 2) == 's'&&*(src + 3) == 'e') {
            src += 4;
            if (match('{')) flower++;
            if (inside()) return 1;
        }
    }
    return 0;
}

//while功能实现
int MainWindow::while_function() {
    jump();
    char* add;
    add = src;
    while (1) {
        src = add;
        if (int(expression_six_enter()) == 0) break;
        if (match('{')) flower++;
        //定义语句
        if (inside()) return 1;
    }
    match('{');
    int flower_if = 1;
    while (1) {

        if (*src == '{') flower_if++;
        else if (*src == '}') flower_if--;
        src++;
        if (flower_if == 0)break;
    }
    return 0;
}

int MainWindow::do_function() {
    jump();
    char* add;
    add = src;
    if (match('{')) flower++;
    if (inside()) return 1;
    jump();
    if (*src == 'u'&&*(src + 1) == 'n'&&*(src + 2) == 't'&&*(src + 3) == 'i'&&*(src + 4) == 'l') {
        src += 5;
        char* add_until = src;
        while (!expression_six_enter()) {
            match(';');
            src = add;
            if (match('{')) flower++;
            if (inside()) return 1;
            jump();
            src = add_until;
        }
        match(';');
    }
    else {
        printf("Error:Less of until (line %d)", line());
        stream << "Error:Less of until (line:" << line() << ")";
        system("pause");
    }
    return 0;
}

//先识别返回值类型，再将返回值存在全局变量return_string或者return_real
//该函数返回值是1的话：返回值是string；否则是：返回值是real；

int MainWindow::return_function() {
    jump();
    int flag;
    if (*src == '\"') {
        flag = STRING;
    }
    else next(flag);
    if (flag == STRING) {
        point_back();
        return_string = string_deal();
        if (*src == ';') {
            src++;
        }
    }
    else {
        point_back();
        return_real = expression_six_enter();
        if (*src == ';') {
            src++;
        }
    }
    return flag;
}


//查询全局变量及函数
void MainWindow::program() {
    while (*src != 0) {
        if (*src == ' ' || *src == '\t') {
            src++;
            continue;
        }
        else if (*src == '/' || *(src + 1) == '/') {
            src += 2;
            while (*src != '\n') {
                src++;
            }
            continue;
        }
        else if (*src == '\n') {
            src++;
            continue;
        }
        else if ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z')) {
            //计算当前标识符的hash值
            int hash = *src;
            //记录关键字的地址
            char *last_pos = src - 1;
            //变量的类型
            int type = 0;
            int i = 1;
            int have_use = 0;
            //查找变量声明语句
            src++;
            while ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z') || (*src >= '0' && *src <= '9')) {
                hash = hash * 131 + *src;
                src++;
            }
            current_id = symbols;
            while (current_id[Token] && i <= STRING) {
                if (current_id[Hash] == hash && current_id[Token] == Sys) {
                    type = i++;
                    current_id += IdSize;
                    break;
                }
                i++;
                current_id += IdSize;
            }
            //如果小于等于3，则说明要声明变量或函数
            if (type > 0 && type <= 3) {
                definition(type);
            }
            else if (!flower) {
                printf("Error:error! (line %d)", line());
                stream << "Error: error!  (line:" << line() << ")";
                system("pause");
            }
            else {
                //识别if、while等语句
                while (current_id[Token] && i >= WHILE && i <= DO) {
                    if (current_id[Hash] == hash && current_id[Token] == Sys) {
                        //					ifOrwhile = (int)last_pos;
                        if (expression(i)) {
                            flower = mf.top() - 1;
                            destroy(mf.top());
                            return;
                        }
                        i++;
                        //					ifOrwhile = 0;
                        break;
                    }
                    i++;
                    current_id += IdSize;
                }
                //识别in、out语句
                while (current_id[Token] && i >= 7 && i <= 8) {
                    if (current_id[Hash] == hash && current_id[Token] == Sys) {
                        type = i++;
                        expression(type);
                        break;
                    }
                    i++;
                    current_id += IdSize;
                }
                //跳过main语句识别
                while (current_id[Token] && i == Main) {
                    current_id += IdSize;
                    i++;
                }
                //识别return语句
                while (current_id[Token] && i == Return) {
                    if (current_id[Hash] == hash && current_id[Token] == Sys) {
                        type = i;
                        expression(type);
                        return;
                    }
                    i++;
                    current_id += IdSize;
                }
                int flag = 0;
                int *tail = symbols_tail;
                char *old_src;
                int *current = current_id;
                search(hash);
                current_id = current;
                while (current_id[Token] && i > 10) {
                    if (current_id[Hash] == hash && current_id[Token] == Id && ((current_id[Type] == INT) || (current_id[Type] == REAL) || (current_id[Type] == STRING))) {
                        expression(current_id[Type]);
                        break;
                    }
                    else if (current_id[Hash] == hash && current_id[Token] == Fun) {

                        int *current = current_id;
                        //匹配函数并解析参数列表
                        if (!flag) {
                            function_resolve();
                            current_id = current;
                            flag = 1;
                            mf.push(flower + 1);
                        }
                        old_src = src;
                        if (function_match(tail)) {
                            src = old_src;
                            current_id = current;
                            //解析函数体
                            old_src = src;
                            function_body();
                            src = old_src;
                            current_id = current;
                            flower = mf.top() - 1;
                            mf.pop();
                            jump();
                            if (*src == ';') src++;
                            break;
                        }
                        else {
                            current_id = current;
                            src = old_src;
                        }
                    }
                    i++;
                    current_id += IdSize;
                }
            }
        }
        else if (*src >= '0'&& *src <= '9') {
            point_back();
            expression_six_enter();
            match(';');
            continue;
        }
        else if (*src == '}') {
            src++;
            flower--;
            break;
        }
        else if (*src == ';') {
            src++;
            continue;
        }
    }
}

int MainWindow::minc_main(string stext) {
    //栈最后一个minFlower始终为0
    mf.push(1);
    //为字符表分配空间
    if (!(symbols = (int *)malloc(POOLSIZE))) {
        printf("could not malloc(%d) for symbol table\n", POOLSIZE);
        stream << "could not malloc(" << POOLSIZE << ") for symbol table\n";
        return -1;
    }
    if (!(text = (int *)malloc(POOLSIZE))) {
        printf("could not malloc(%d) for text area\n", POOLSIZE);
        stream << "could not malloc(" << POOLSIZE << ") for text table\n";
        return -1;
    }
    memset(symbols, 0, POOLSIZE);
    memset(text, 0, POOLSIZE);

    old_text = text;
    /*
    src：文本指针
    ch：文件读取操作中读取单个字符
    */
    char ch;
    //预存关键字
    src = "int real string while if do in out main return";

    //预存系统变量
    Pretreatment();

    src = (char*)stext.c_str();

    program();

    //printf("\n-------------------------------\n");
    //stream << "\n-------------------------------\n";
    //memset(src, 0, POOLSIZE);
    memset(symbols, 0, POOLSIZE);
    memset(text, 0, POOLSIZE);
    flower = 0;
    flagMain = 0;
    stream.clear();
    while(!mf.empty())mf.pop();
    while(!s.empty())s.pop();
    while(!d.empty())d.pop();
    inHintStr = "";
    outHintStr = "";
    return 0;


}





MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 初始化文件为未保存状态
    isUntitled = true;
    // 初始化文件名为"untitled.txt"
    curFile = tr("untitled.txt");
    // 初始化窗口标题为文件名
    this->setWindowTitle("Min-C Interpreter");
    this->resize( QSize( 1024, 768 ));
    codeEditor = new CodeEditor(this);

    setCentralWidget(codeEditor);
    findDlg = new QDialog(this);
    findDlg->setWindowTitle(tr("查找"));
    findLineEdit = new QLineEdit(findDlg);
    QPushButton *btn= new QPushButton(tr("查找下一个"), findDlg);
    QVBoxLayout *layout= new QVBoxLayout(findDlg);
    layout->addWidget(findLineEdit);
    layout->addWidget(btn);
    connect(btn, &QPushButton::clicked, this, &MainWindow::showFindText);
    ui->statusBar->showMessage(tr("重案六组出品"),4000);
    dlg = new QDialog(this);
    dlg->setWindowTitle("Input");
    dlg->resize( QSize( 600, 400 ));
    led = new QTextEdit(dlg);
    QPushButton *button= new QPushButton(tr("确定"), dlg);
    QVBoxLayout *lay= new QVBoxLayout(dlg);
    lay->addWidget(led);
    lay->addWidget(button);
    connect(button, &QPushButton::clicked, this, &MainWindow::setText);
    connect(button, &QPushButton::clicked, dlg, dlg->close);
    highlighter = new Highlighter(codeEditor->document());
}


void MainWindow::setText()
{
    QString hhh = led->toPlainText();
    hh = hhh.toStdString();
}


void MainWindow::newFile()
{
    if(maybeSave()){
        isUntitled = true;
        curFile = tr("untitled.txt");
        codeEditor->QPlainTextEdit::clear();
        codeEditor->QPlainTextEdit::setVisible(true);
    }
}
bool MainWindow::maybeSave()
{
    if(codeEditor->document()->isModified()){
        QMessageBox box;
        box.setWindowTitle(tr("警告"));
        box.setIcon(QMessageBox::Warning);
        box.setText(curFile + tr("尚未保存。是否保存？"));
        QPushButton *yesBtn = box.addButton(tr("是(&Y)"),QMessageBox::YesRole);
        box.addButton(tr("否(&N)"),QMessageBox::NoRole);
        QPushButton *cancelBut = box.addButton(tr("取消"),QMessageBox::RejectRole);

        box.exec();
        if(box.clickedButton() == yesBtn)
            return save();
        else if(box.clickedButton() == cancelBut)
            return false;
        }
return true;
}

bool MainWindow::save()
{
    if(isUntitled){
                return saveAs();
            }else{
                return saveFile(curFile);
           }
}
bool MainWindow::saveAs()
{
         QString fileName = QFileDialog::getSaveFileName(this,
            tr("另存为"),curFile);
            if(fileName.isEmpty())return false;
            return saveFile(fileName);
        }
bool MainWindow::saveFile(const QString &fileName)
{
            QFile file(fileName);
            if(!file.open(QFile::WriteOnly | QFile::Text)){
                QMessageBox::warning(this,tr("多文档编辑器"),
                tr("无法写入文件 %1 /n %2")
                .arg(fileName).arg(file.errorString()));
                return false;
            }
            QTextStream out(&file);
            QApplication::setOverrideCursor((Qt::WaitCursor));
            out << codeEditor->QPlainTextEdit::toPlainText();
            QApplication::restoreOverrideCursor();
            isUntitled = false;
            curFile = QFileInfo(fileName).canonicalFilePath();
            setWindowTitle(curFile);
            return true;
        }
bool MainWindow::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if(!file.open(QFile::ReadOnly | QFile::Text)){
        QMessageBox::warning(this,tr("多文档编辑器"),
                             tr("无法读取文件 %1\n%2.")
                             .arg(fileName).arg(file.errorString()));
    return false;
    }
    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    codeEditor->QPlainTextEdit::setPlainText(in.readAll());
    QApplication::restoreOverrideCursor();

    curFile = QFileInfo(fileName).canonicalFilePath();
    setWindowTitle(curFile);
    return true;
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_N_triggered()
{
    newFile();
}

void MainWindow::on_action_S_triggered()
{
    save();
}

void MainWindow::on_action_A_triggered()
{
    saveAs();
}

void MainWindow::on_action_O_triggered()
{
    if(maybeSave()){
        QString fileName = QFileDialog::getOpenFileName(this);
        if(!fileName.isEmpty()){
            loadFile(fileName);
            codeEditor->QPlainTextEdit::setVisible(true);
        }
    }
}

void MainWindow::on_action_E_triggered()
{
    if(maybeSave()){
        codeEditor->QPlainTextEdit::setVisible(false);
    }
    qApp->quit();
}

void MainWindow::on_action_Z_triggered()
{
    codeEditor->QPlainTextEdit::undo();
}

void MainWindow::on_action_X_triggered()
{
    codeEditor->QPlainTextEdit::cut();
}

void MainWindow::on_action_C_triggered()
{
    codeEditor->QPlainTextEdit::copy();
}

void MainWindow::on_action_P_triggered()
{
    codeEditor->QPlainTextEdit::paste();
}

void MainWindow::showFindText()
{
    QString str = findLineEdit->text();
    if (!codeEditor->QPlainTextEdit::find(str, QTextDocument::FindBackward))
    {
       QMessageBox::warning(this, tr("查找"),
                tr("找不到%1").arg(str));
    }
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    if(maybeSave()){
        event->accept();
    }else{
        event->ignore();
    }
}

void MainWindow::on_action_S_2_triggered()
{
    findDlg->show();
}

void MainWindow::on_action_I_triggered()
{

    QString str = "min-c interpreter";
    QMessageBox box;
    box.setWindowTitle("使用说明");
    box.setText(str);
    box.exec();
}


void MainWindow::on_action_K_triggered()
{
    stream.str("");
    QString str;
    string haha,xixi;
    str = codeEditor->QPlainTextEdit::toPlainText();
    haha = str.toStdString();
    led->clear();
    minc_main(haha);
    xixi = stream.str();
    QString qstr = QString::fromStdString(xixi);

    box = new QDialog(this);
    box->setWindowTitle("Output");
    box->resize( QSize( 600, 400 ));
    QVBoxLayout *lay= new QVBoxLayout(box);
    QTextEdit *ted = new QTextEdit(this);
    QPushButton *but= new QPushButton(tr("确定"), box);
    lay->addWidget(ted);
    lay->addWidget(but);
    connect(but, &QPushButton::clicked, box, box->close);
    QString qhintStr = QString::fromStdString(outHintStr);
    ted->setPlainText(qhintStr+qstr);
    box->show();
}




CodeEditor::CodeEditor(QWidget *a) : QPlainTextEdit(a)
{
    lineNumberArea = new LineNumberArea(this);

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}



int CodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

    return space;
}



void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}



void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}



void CodeEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}



void CodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = "#9cc9c9";

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}



void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);


    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}
