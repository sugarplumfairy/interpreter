#define _CRT_SECURE_NO_WARNINGS
#define POOLSIZE 10000
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

//记录花括号
int flower = 0;

//系统关键字
enum {
	INT = 1, REAL, STRING, WHILE, IF, ELSE, IN, OUT, Main, Return
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
//返回值的全局变量(real型或string型)
double return_real;
string return_string;
//表达式识别入口
double expression_six_enter();
//语句分析入口
void statement();
//字符串处理入口
string string_deal();
int match(char a);
void point_back();

void program();

void if_function();
void while_function();
int return_function();

int line() {
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
int search(int hash) {
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
	return 0;
}


//销毁作用域以外的变量 
void destroy(int flower) {
	current_id = symbols_tail - IdSize;
	while (current_id[Token] && current_id[Flower] == flower) {
		current_id[Token] = 0;
		current_id -= IdSize;
	}
	symbols_tail = current_id + IdSize;
}
//解析函数参数列表
int function_parameter() {
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
void Pretreatment() {
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
void myPrint() {
	while (*src != '"') {
		if (*src == 0 || *src == '\n' || *src == ';') {
			printf("Error:Less of '\"'! (line %d)", line());
			//exit(-1);
		}
		if (*src == '\\') {
			src++;
			if (*src == 'n') {
				printf("\n");
				src++;
			}
			else if (*src == 't') {
				printf("\t");
				src++;
			}
			else if (*src == '0') {
				src++;
				while (*src != '"') {
					if (*src == 0 || *src == '\n' || *src == ';') {
						printf("Error:Less of '\"'! (line %d)", line());
						//exit(-1);
					}
					src++;
				}
				return;
			}
			else if (*src == '\\') {
				printf("\\");
				src++;
			}
			else if (*src == '"') {
				printf("\"");
				src++;
			}
			else {
				printf("\\%c", *src);
				src++;
			}
		}
		else {
			printf("%c", *src);
			src++;
		}
	}
}
//空格跳过语句
void jump() {
	while (*src == ' ' || *src == '\t' || *src == '\n') {
		src++;
	}
}

//解析in并赋值给参数
void parseIn() {
	jump();
	char *last_pos = src;
	int hash = *src;
	//判断该字符是否为字母开头
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
				scanf("%d", &a);
				current_id[Value] = a;
				break;
			}
			case 2: {
				double a;
				scanf("%lf", &a);
				d.top() = a;
				current_id[Value] = (int)&d.top();
				break;
			}
			case 3: {
				string a;
				cin >> a;
				s.top() = a;
				current_id[Value] = (int)&s.top();
				/*
				cout << *(string *)(current_id + Value - 1);*/
				break;
			}
			default: {
				printf("Error:variable error! (line %d)", line());
				//exit(-1);
			}
			}
		}
	}
	else {
		printf("Error:wrong argument! (line %d)", line());
		//exit(-1);
	}
}
//解析out
void parseOut() {
	jump();
	//指针已经跳到了out以后的第一个字符
	int hash = *src;
	//如果第一个是为字母
	if ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z')) {
		src++;
		while ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z') || (*src >= '0' && *src <= '9')) {
			hash = hash * 131 + *src;
			src++;
		}
		//去symbol表中寻找变量
		if (search(hash)) {
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
									break;
								}
								case 2: {
									//输出real
									printf("%lf", *(double *)current_id[Value]);
									break;
								}
								case 3: {
									//输出字符串
									cout << *(string *)current_id[Value];
									break;
								}
								default: {
									printf("Error!");
									//exit(-1);
								}
								}
							}
						}
					}
				}
				else if (*src == ';') {
					printf("%d", current_id[Value]);
				}
				break;
			}
			case 2: {
				if (current_id[Value]) {
					printf("%lf", *(double *)current_id[Value]);
				}
				else {
					printf("0.0");
				}
				break;
			}
			case 3: {
				if (current_id[Value]) {
					cout << *(string *)current_id[Value];
				}
				else {
					printf("null");
				}
				break;
			}
			default: {
				printf("Error:variable not found! (line %d)", line());
				//exit(-1);
			}
			}
		}
		match(';');//已改动，指针在
	}
	//如果第一个是引号
	else if (*src == '"') {
		src++;
		myPrint();
		src++;
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
						break;
					}
					case 2: {
						double a = *(double *)current_id[Value];
						printf("%f", a);
						break;
					}
					case 3: {
						cout << *(string *)current_id[Value];
						break;
					}
					default: {
						printf("Error:variable error! (line %d)", line());
						//exit(-1);
					}
					}
				}
				match(';');
			}
		}
		else {
			match(';');
		}
	}
	else if (*src > '0' && *src <= '9') {
		int time = *src - '0';
		src++;
		while (*src >= '0' && *src <= '9') {
			time = time * 10 + *src - '0';
			src++;
		}
		jump();
		if (*src == ',') {
			src++;
			jump();
			//遇到‘”’
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
							break;
						}
						case 2: {
							//输出real
							printf("%lf", *(double *)current_id[Value]);
							break;
						}
						case 3: {
							//输出字符串
							cout << *(string *)current_id[Value];
							break;
						}
						default: {
							printf("Error!");
							//exit(-1);
						}
						}
					}
				}
			}
			//如果
			else {
				printf("Error:wrong argument! (line %d)", line());
				//exit(-1);
			}
		}
		//如果没有匹配到‘，’报错。
		else {
			printf("Error:wrong argument! (line %d)", line());
			//exit(-1);
		}
		match(';');
	}
}
//解析表达式
void expression(int type) {
	if (type == IF) {
		if_function();
	}
	else if (type == WHILE) {
		while_function();
	}
	else if (type == IN) {
		//去掉空格
		jump();
		//判断in关键字后是否有"
		if (*src == '"') {
			//输出引号里的内容
			src++;
			myPrint();
			src++;
			//判断引号结束以后是否有逗号
			if (*src == ',') {
				src++;
				//进入解析In的函数
				parseIn();
			}
			//没有匹配到逗号报错
			else {
				printf("Error:wrong arguments! (line %d)", line());
				//exit(-1);
			}
		}
		else if ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z')) {
			//进入解析In的函数
			parseIn();
		}
		else {
			printf("Error:wrong argument! (line %d)", line());
			//exit(-1);
		}
		jump();
		if (*src == ';') {
			src++;
			return;
		}
		else {
			printf("Error:wrong argument! (line %d)", line());
			//exit(-1);
		}
	}
	else if (type == OUT) {
		parseOut();
	}
	else if (type == Return) {
		return_function();
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
		//printf("\nshow result: %f\n", expression_six_enter());
		if (*src == ';') {
			src++;
		}
	}
	else {
		printf("ERROR: ");
		//exit(-1);
	}
}
//解析int、real、string类型后对标识符赋值
void definition(int type) {
	jump();
	int hash;
	while (*src != ';') {
		//记录标识符的地址
		char *last_pos = src;
		if (*src == '\n') {
			printf("Error:wrong arguments! (line %d)", line);
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
						printf("Error:wrong arguments!(line %d)", line);
						//exit(-1);
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
						printf("Error:Repeated function declaration (line %d)", line);
						//exit(-1);
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
						printf("Error:Repeated variable declaration (line %d)", line);
						//exit(-1);
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
					printf("Error:wrong arguments! (line %d)", line);
					//exit(-1);
				}
				continue;
			}
			//变量声明结束
			else if (*src == ';') {
				current_id = symbols;
				while (current_id[Token]) {
					if (current_id[Hash] == hash && current_id[Flower] == flower) {
						printf("Error:Repeated variable declaration (line %d)", line);
						//exit(-1);
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
						printf("Error:Repeated variable declaration (line %d)", line);
						//exit(-1);
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
				printf("Error:wrong arguments (line %d)", line);
				//exit(-1);
			}
		}
	}
}

//取下一个标记，并识别当前标记是否正确
int match(char a) {
	//遇到空格 换行 tab 指针后跳
	while (*src == ' ' || *src == '\n' || *src == '\t') {
		src++;
	}
	//如果一直遇不到语法定义必须的字符，证明输入代码有错，直接报错，结束程序
	if (*src != a) {
		printf("less of %c :line %d \n", a, line());
		//exit(-1);
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
int next(int &type_value) {
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
			if (current_id[Token] == Id) {
				type_value = current_id[type];
				return 1;
			}
			else return 0;

		}
	}
	else if (*src >= '0' && *src <= '9') {
		type_value = 1;
		return 1;
	}
}


//匹配参数列表
void function_resolve() {
	jump();
	if (*src == '(') {
		int *tail = symbols_tail;
		while (*src != ')') {
			src++;
			jump();
			if ((*src > '0' && *src <= '9') || (*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src >= 'Z')) {
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
				//			//exit(-1);
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
			else if (*src == '\"') {
				string a;
				src++;
				while (*src != '\"') {
					a = a + *src;
					if (*src == '\n') {
						printf("Error:less of \" (line %d)", line());
						//exit(-1);
					}
					src++;
				}
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
				//exit(-1);
			}
		}
		src++;
		jump();
	}
}

//匹配函数
int function_match(int *tail) {
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
				//exit(-1);
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
							continue;
						}
					}
					else {
						printf("Error:wrong arguments in (line %d)", line());
						//exit(-1);
					}
				}
				else {
					return 0;
				}
			}

			jump();

			if (*src == ',') {
				src++;
				jump();
				if (*src == ')') {
					printf("Error:less of variable define (line %d)", line());
					//exit(-1);
				}
			}
		}
		else {
			printf("Error:less of variable define (line %d)", line());
			//exit(-1);
		}
	}
	if (taill[Token] != 0) {
		return 0;
	}
	return 1;
}

//解析函数体
void function_body() {
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
				//exit(-1);
			}
			else {
				//识别if、while等语句
				while (current_id[Token] && i >= WHILE && i <= ELSE) {
					if (current_id[Hash] == hash && current_id[Token] == Sys) {
						//					ifOrwhile = (int)last_pos;
						expression(i);
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
			printf("show result: %f\n", expression_six_enter());
			match(';');
			continue;
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
void point_back() {
	src--;
	while (*src != ' ' && *src != '\n' && *src != '\t'&& *src != 0) {
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
string string_factor() {
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
		match('"');
		while (*src != '"') {
			if (*src == '\\') {
				src++;
				if (*src == '0') value.append(1, '\0');
				else if (*src == '\\') value.append(1, '\\');
				else if (*src == 'n') value.append(1, '\n');
				else if (*src == '"') value.append(1, '\"');
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
			if (current_id[Token] == Fun) {
				int flag = 0;
				int *tail = symbols_tail;
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
						if (*src == ';') src++;
					}
					else {
						current_id = current;
						src = old_src;
					}
				}
				return return_string;
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
	}
	while (*src == ' ' || *src == '\n') {
		src++;
	}
	return value;
}
//识别字符串操作符$,#后进行相应操作
string string_deal_tail(string lvalue) {
	if (*src == '$') {
		match('$');
		string value = lvalue + string_factor();
		return string_deal_tail(value);
	}
	else if (*src == '#') {
		match('#');
		int num;
		num = (int)expression_six_enter();
		string value = lvalue.erase(num - 1, 1);
		return string_deal_tail(value);
	}
	else {
		return lvalue;
	}
}
//
string string_deal() {
	string lvalue = string_factor();
	return string_deal_tail(lvalue);
}

/*
使用递归下降的方法
已消除左递归
如one_tail();
one();
*/
//最小单位和单目运算-，+（正负运算）；！非运算；（）括号运算
double factor() {
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
	else if (*src >= '0' && *src <= '9') {
		value = *src - '0';
		src++;
		if (value > 0) {
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
						if (*src == ';') src++;
					}
					else {
						current_id = current;
						src = old_src;
					}
				}
				return return_real;
			}

			else if (current_id[Token] == Id) {
				switch (type)
				{
				case 1: {
					if (*src == '='&&*(src + 1) != '=') {
						src++;
						jump();
						int *location = &current_id[Value];
						*location = (int)expression_six_enter();
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
						*location = expression_six_enter();
						value = *location;
					}
					else return *(double *)current_id[Value];
					break;
				}
				default:
					printf("Error:variable not found! (line %d)", line());
					//			exit(-1);
					break;
				}
			}

		}
	}
	jump();
	return value;
}
//第一级运算，+；-
double one_tail(double lvalue) {
	if (*src == '^') {
		match('^');
		double value = pow(lvalue, factor());
		return one_tail(value);
	}
	else {
		return lvalue;
	}
}
double one() {
	double lvalue = factor();
	return one_tail(lvalue);
}
//第二级运算，*；/；%
double two_tail(double lvalue) {

	if (*src == '*') {
		match('*');
		double value = lvalue * one();
		return two_tail(value);
	}
	else if (*src == '/') {
		match('/');
		double value = lvalue / one();
		return two_tail(value);
	}
	else if (*src == '%') {
		match('%');
		if (int(lvalue) != lvalue) {
			printf("ERROR int%%int : line%d", line());
			//exit(-1);
		}
		double temp = one();
		if (int(temp) != temp) {
			printf("ERROR int%%int : line%d\n", line());
			//exit(-1);
		}
		double value = (int)lvalue % (int)temp;
		return two_tail(value);
	}
	else {
		return lvalue;
	}
}
double two() {
	double lvalue = one();
	return two_tail(lvalue);
}
//第三级运算，+；-
double three_tail(double lvalue) {
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
double three() {
	double lvalue = two();
	return three_tail(lvalue);
}
//第四级运算，>；>=；<；<=
double four_tail(double lvalue) {
	if (*src == '>') {
		match('>');
		if (*src == '=') {
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
double four() {
	double lvalue = three();
	return four_tail(lvalue);
}
//第五级运算，==；<>；
double five_tail(double lvalue) {
	if (*src == '='&&*(src + 1) == '=') {
		src++;
		match('=');
		double value = lvalue == four();
		return five_tail(value);
	}
	if (*src == '<') {
		src++;
		match('>');
		double value = (lvalue != four());
		return five_tail(value);
	}
	else {
		return lvalue;
	}
}
double five() {
	double lvalue = four();
	return five_tail(lvalue);
}
//第六级运算，&&；||
double expression_six_enter_tail(double lvalue) {
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
double expression_six_enter() {
	double lvalue = five();
	return expression_six_enter_tail(lvalue);
}

//if-else功能实现
void if_function() {
	int flag_ifelse = int(expression_six_enter());
	if (flag_ifelse) {
		if (match('{')) flower++;
		program();
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
			program();
		}
	}
}

//while功能实现
void while_function() {
	jump();
	char* add;
	add = src;
	while (1) {
		src = add;
		if (int(expression_six_enter()) == 0) break;
		if (match('{')) flower++;
		//定义语句
		program();
	}
	match('{');
	int flower_if = 1;
	while (1) {

		if (*src == '{') flower_if++;
		else if (*src == '}') flower_if--;
		src++;
		if (flower_if == 0)break;
	}
}

//先识别返回值类型，再将返回值存在全局变量return_string或者return_real
//该函数返回值是1的话：返回值是string；否则是：返回值是real；

int return_function() {
	jump();
	int flag;
	next(flag);
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

//识别关键字（IF，WHILE，RETURN，IN，OUT，STRING）后进行相应的操作
void statement() {
	int flag;
	next(flag);
	if (flag == IF) {
		if_function();
	}
	else if (flag == WHILE) {
		while_function();
	}
	else if (flag == Return) {
		return_function();
		match(';');
	}
	else if (flag == IN || flag == OUT) {
		expression(flag);
	}
	else if (flag == STRING) {
		point_back();
		string_deal();
		match(';');
	}
	//识别的不是关键字，则是表达式
	else {
		point_back();
		printf("%f", expression_six_enter());
		match(';');
	}
}

//查询全局变量及函数
void program() {
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
				//exit(-1);
			}
			else {
				//识别if、while等语句
				while (current_id[Token] && i >= WHILE && i <= ELSE) {
					if (current_id[Hash] == hash && current_id[Token] == Sys) {
						//					ifOrwhile = (int)last_pos;
						expression(i);
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
						break;
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
			printf("show result: %f\n", expression_six_enter());
			match(';');
			continue;
		}
		else if (*src == '}') {
			src++;
			destroy(flower);
			flower--;
			break;
		}
	}
}

int main() {
	//栈最后一个minFlower始终为0 
	mf.push(0);
	//为字符表分配空间
	if (!(symbols = (int *)malloc(POOLSIZE))) {
		printf("could not malloc(%d) for symbol table\n", POOLSIZE);
		return -1;
	}
	if (!(text = (int *)malloc(POOLSIZE))) {
		printf("could not malloc(%d) for text area\n", POOLSIZE);
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
	src = "int real string while if else in out main return";

	//预存系统变量
	Pretreatment();

	if (!(src = old_src = (char *)malloc(POOLSIZE))) {
		printf("could not malloc(%d) for source area\n", POOLSIZE);
		return -1;
	}



	int num = 0;
	//读取输入的文件文本
	while ((*src = getchar()) != EOF) {
		num++;
		src++;
	}
	*src = 0;

	//将指针位置移到文本头
	head = src -= num;

	program();

	printf("\n-------------------------------\n");

	//current_id = symbols;
	//while (current_id[Token]) {
	//	printf("Token:%d\n", current_id[Token]);
	//	printf("Type:%d\n", current_id[Type]);
	//	printf("Value:%d\n", current_id[Value]);
	//	current_id += IdSize;
	//}
	system("pause");
}