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

//�����ռ�
using namespace std;

//��¼minFlower���ں�����ȡ����
stack<int> mf;

//�������ô洢�ַ�����double�ͱ���
stack<string> s;
stack<double> d;

/*
symbols:��ʶ����
current_id:����ÿ�β�ѯ�Ƿ������ͬ������������µı���
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

//��¼������
int flower = 0;

//ϵͳ�ؼ���
enum {
	INT = 1, REAL, STRING, WHILE, IF, ELSE, IN, OUT, Main, Return
};

/*
��ʶ�����ö������
Hash����ʶ����Ψһ��ʶ
Value: ��Ϊ���������ֵ����Ϊ��������ַ
Type: ������������
Token: ���ֱ���������
Log: �ж�ȫ�ֻ򱾵ر���
IdSize��ÿһ����ʶ���ڱ���ռ�õ��򳤶�
Flower����¼�����Ļ����Ų����������������ж�
*/
enum {
	Hash, Value, Type, Token, Address, Flower, WwwLz, IdSize
};

//������(�����ȼ�˳������)
enum {
	Assign = 128, Add, Sub, Mul, Div, Mod, Con, Del, Or, And, Non, Mor, Les, Moe, Loe, Eq, Neq, Pos, Neg
};

/*
��������ʶ����ϵͳ����
*/

enum {
	Fun = 1, Id, Sys
};

//Ԥ����������
//����ֵ��ȫ�ֱ���(real�ͻ�string��)
double return_real;
string return_string;
//���ʽʶ�����
double expression_six_enter();
//���������
void statement();
//�ַ����������
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

//����� 
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


//��������������ı��� 
void destroy(int flower) {
	current_id = symbols_tail - IdSize;
	while (current_id[Token] && current_id[Flower] == flower) {
		current_id[Token] = 0;
		current_id -= IdSize;
	}
	symbols_tail = current_id + IdSize;
}
//�������������б�
int function_parameter() {
	int wwwlz = 0, k = 1;
	while (*src != ')') {
		token = *src++;
		//����ҵ�������continue
		if (token == ',') {
			continue;
		}
		//���Ҳ����������
		if ((token >= 'a' && token <= 'z') || (token >= 'A' && token <= 'Z')) {
			//��¼��ʶ���׵�ַ
			char *identifier = src - 1;
			//��ʶ���Ĺ�ϣֵ
			int hash = token;
			while ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z') || (*src >= '0' && *src <= '9')) {
				hash = hash * 131 + *src++;
			}
			int type = 0;
			current_id = symbols;
			int i = 0;
			//�����������͵ı�ʶ������
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
//Ԥ��ϵͳ����
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

			//������ϵͳ����
			current_id[Token] = Sys;
			current_id[Hash] = hash;
			current_id[Flower] = 0;
			current_id += IdSize;
		}
	}
}
//������
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
//�ո��������
void jump() {
	while (*src == ' ' || *src == '\t' || *src == '\n') {
		src++;
	}
}

//����in����ֵ������
void parseIn() {
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
//����out
void parseOut() {
	jump();
	//ָ���Ѿ�������out�Ժ�ĵ�һ���ַ�
	int hash = *src;
	//�����һ����Ϊ��ĸ
	if ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z')) {
		src++;
		while ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z') || (*src >= '0' && *src <= '9')) {
			hash = hash * 131 + *src;
			src++;
		}
		//ȥsymbol����Ѱ�ұ���
		if (search(hash)) {
			int type = current_id[Type];
			switch (type) {
				//int��
			case 1: {
				//ѭ������Ĵ���
				int time = current_id[Value];
				jump();
				if (*src == ',') {
					src++;
					jump();
					//���ʶ�𵽡�����
					if (*src == '"') {
						src++;
						char *old_src = src;
						//ѭ�����
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
									//��� int
									printf("%d", current_id[Value]);
									break;
								}
								case 2: {
									//���real
									printf("%lf", *(double *)current_id[Value]);
									break;
								}
								case 3: {
									//����ַ���
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
		match(';');//�ѸĶ���ָ����
	}
	//�����һ��������
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
			//����������
			if (*src == '"') {
				src++;
				char *old_src = src;
				//ѭ�����
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
							//��� int
							printf("%d", current_id[Value]);
							break;
						}
						case 2: {
							//���real
							printf("%lf", *(double *)current_id[Value]);
							break;
						}
						case 3: {
							//����ַ���
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
			//���
			else {
				printf("Error:wrong argument! (line %d)", line());
				//exit(-1);
			}
		}
		//���û��ƥ�䵽����������
		else {
			printf("Error:wrong argument! (line %d)", line());
			//exit(-1);
		}
		match(';');
	}
}
//�������ʽ
void expression(int type) {
	if (type == IF) {
		if_function();
	}
	else if (type == WHILE) {
		while_function();
	}
	else if (type == IN) {
		//ȥ���ո�
		jump();
		//�ж�in�ؼ��ֺ��Ƿ���"
		if (*src == '"') {
			//��������������
			src++;
			myPrint();
			src++;
			//�ж����Ž����Ժ��Ƿ��ж���
			if (*src == ',') {
				src++;
				//�������In�ĺ���
				parseIn();
			}
			//û��ƥ�䵽���ű���
			else {
				printf("Error:wrong arguments! (line %d)", line());
				//exit(-1);
			}
		}
		else if ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z')) {
			//�������In�ĺ���
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
//����int��real��string���ͺ�Ա�ʶ����ֵ
void definition(int type) {
	jump();
	int hash;
	while (*src != ';') {
		//��¼��ʶ���ĵ�ַ
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
			//�����ʱΪ(��˵��Ϊ��������
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
				//�ж���main����������Ҫ���������壻.,
				current_id = symbols;
				current_id = current_id + IdSize * 8;
				if (current_id[Hash] == hash) {
					//����main�����Ĳ���
					if (match('{')) flower++;
					//����main�����ĺ�����
					program();
					/*match('}');*/
					break;
				}
				else {
					int back = 1;
					//����������
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
			//Ϊ,��Ϊ����������������ȡ�����ı���
			else if (*src == ',') {
				current_id = symbols;
				while (current_id[Token]) {
					if (current_id[Hash] == hash && current_id[Flower] == flower) {
						printf("Error:Repeated variable declaration (line %d)", line);
						//exit(-1);
					}
					current_id += IdSize;
				}
				//�洢�±���
				current_id[Token] = Id;
				current_id[Hash] = hash;
				current_id[Address] = (int)last_pos;
				current_id[Flower] = flower;
				current_id[Type] = type;
				//Ϊdouble�ͱ�����string�ͱ�������ռ�
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
			//������������
			else if (*src == ';') {
				current_id = symbols;
				while (current_id[Token]) {
					if (current_id[Hash] == hash && current_id[Flower] == flower) {
						printf("Error:Repeated variable declaration (line %d)", line);
						//exit(-1);
					}
					current_id += IdSize;
				}
				//�洢�±���
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
			else if (*src == '=') {//�Ķ� ��ΰ�� 2017.9.10
				current_id = symbols;
				while (current_id[Token]) {
					if (current_id[Hash] == hash && current_id[Flower] == flower) {
						printf("Error:Repeated variable declaration (line %d)", line);
						//exit(-1);
					}
					current_id += IdSize;
				}
				//�洢�±���
				current_id[Token] = Id;
				current_id[Hash] = hash;
				current_id[Address] = (int)last_pos;
				current_id[Flower] = flower;
				current_id[Type] = type;
				symbols_tail = current_id + IdSize;
				//Ϊdouble�ͱ�����string�ͱ�������ռ�
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
			//��������
			else {
				printf("Error:wrong arguments (line %d)", line);
				//exit(-1);
			}
		}
	}
}

//ȡ��һ����ǣ���ʶ��ǰ����Ƿ���ȷ
int match(char a) {
	//�����ո� ���� tab ָ�����
	while (*src == ' ' || *src == '\n' || *src == '\t') {
		src++;
	}
	//���һֱ�������﷨���������ַ���֤����������д�ֱ�ӱ�����������
	if (*src != a) {
		printf("less of %c :line %d \n", a, line());
		//exit(-1);
	}
	//ָ��������Ҫƥ����ַ���ַ��
	src++;
	//������Ҫƥ���ַ�����Ŀո� ���� tab
	while (*src == ' ' || *src == '\n' || *src == '\t') {
		src++;
	}
	return 1;

}

//ʶ���һ����Ԫ�������������֣���������if-else��while�����Ͼ䣩�����������Ԫ�����͵�type_value
int next(int &type_value) {
	jump();

	if ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z')) {
		//���㵱ǰ��ʶ����hashֵ
		int hash = *src;
		//����������
		int type = 0;
		int i = 0;
		//���ұ����������
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


//ƥ������б�
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

//ƥ�亯��
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
			//���㵱ǰ��ʶ����hashֵ
			int hash = *src++;
			//����������
			int type = 0;
			int i = 0;
			//���ұ����������
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

//����������
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
			//���㵱ǰ��ʶ����hashֵ
			int hash = *src;
			//��¼�ؼ��ֵĵ�ַ
			char *last_pos = src - 1;
			//����������
			int type = 0;
			int i = 1;
			int have_use = 0;
			//���ұ����������
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
			//���С�ڵ���3����˵��Ҫ������������
			if (type > 0 && type <= 3) {
				definition(type);
			}
			else if (!flower) {
				printf("Error:error! (line %d)", line());
				//exit(-1);
			}
			else {
				//ʶ��if��while�����
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
				//ʶ��in��out���
				while (current_id[Token] && i >= 7 && i <= 8) {
					if (current_id[Hash] == hash && current_id[Token] == Sys) {
						type = i++;
						expression(type);
						break;
					}
					i++;
					current_id += IdSize;
				}
				//����main���ʶ��
				while (current_id[Token] && i == Main) {
					current_id += IdSize;
					i++;
				}
				//ʶ��return���
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
						//ƥ�亯�������������б�
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
							//����������
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

//���ʽ���㿪ʼ��ʱ��ָ��������ʽ��ͷ
void point_back() {
	src--;
	while (*src != ' ' && *src != '\n' && *src != '\t'&& *src != 0) {
		src--;
	}
	src++;

}

//�ַ����������ȼ�2����ֵ=   �����ȼ�1������$,ɾ��#��
//��ʽ   string�ͱ���+ ���š�=�� + �ַ��� + ���š�$�� �ַ���   ���ַ��������� string�ͱ��� ���� ��abc���������� �� aa="sfdvds"$bb;
/*��ʽ   string�ͱ���+ ���š�=�� + �ַ��� + ���š�#�� ��������ֵı��ʽ����1+2   ���ַ��������� string�ͱ��� ���� ��abc����������
��  aa= "sagfgvxz" # 1+2;
*/
//ʹ�õݹ��½����㷨
string string_factor() {
	string value;
	//�������ž��ٴν��б��ʽ����expression_six_enter����
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
	//����ʶ��ͺ���ʶ��
	else if (*src >= 'a' && *src <= 'z' || *src >= 'A' && *src <= 'Z') {
		//��������������ʶ��
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
					//ƥ�亯�������������б�
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
						//����������
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
//ʶ���ַ���������$,#�������Ӧ����
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
ʹ�õݹ��½��ķ���
��������ݹ�
��one_tail();
one();
*/
//��С��λ�͵�Ŀ����-��+���������㣩���������㣻������������
double factor() {
	double value = 0;
	//�������ž��ٴν��б��ʽ����expression_six_enter����
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
	//����ʶ��ͺ���ʶ��
	else if (*src >= 'a' && *src <= 'z' || *src >= 'A' && *src <= 'Z') {
		//��������������ʶ��
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
					//ƥ�亯�������������б�
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
						//����������
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
//��һ�����㣬+��-
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
//�ڶ������㣬*��/��%
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
//���������㣬+��-
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
//���ļ����㣬>��>=��<��<=
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
//���弶���㣬==��<>��
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
//���������㣬&&��||
double expression_six_enter_tail(double lvalue) {
	if (*src == '&') {
		src++;
		match('&');
		double value = five() && lvalue;//five������lvalue��λ�ò��ܻ�������ָ�벻����������ѧ�����˹���
		return expression_six_enter_tail(value);
	}
	else if (*src == '|') {
		src++;
		match('|');
		double value = five() || lvalue;//five������lvalue��λ�ò��ܻ�������ָ�벻����������ѧ��
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

//if-else����ʵ��
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

//while����ʵ��
void while_function() {
	jump();
	char* add;
	add = src;
	while (1) {
		src = add;
		if (int(expression_six_enter()) == 0) break;
		if (match('{')) flower++;
		//�������
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

//��ʶ�𷵻�ֵ���ͣ��ٽ�����ֵ����ȫ�ֱ���return_string����return_real
//�ú�������ֵ��1�Ļ�������ֵ��string�������ǣ�����ֵ��real��

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

//ʶ��ؼ��֣�IF��WHILE��RETURN��IN��OUT��STRING���������Ӧ�Ĳ���
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
	//ʶ��Ĳ��ǹؼ��֣����Ǳ��ʽ
	else {
		point_back();
		printf("%f", expression_six_enter());
		match(';');
	}
}

//��ѯȫ�ֱ���������
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
			//���㵱ǰ��ʶ����hashֵ
			int hash = *src;
			//��¼�ؼ��ֵĵ�ַ
			char *last_pos = src - 1;
			//����������
			int type = 0;
			int i = 1;
			int have_use = 0;
			//���ұ����������
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
			//���С�ڵ���3����˵��Ҫ������������
			if (type > 0 && type <= 3) {
				definition(type);
			}
			else if (!flower) {
				printf("Error:error! (line %d)", line());
				//exit(-1);
			}
			else {
				//ʶ��if��while�����
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
				//ʶ��in��out���
				while (current_id[Token] && i >= 7 && i <= 8) {
					if (current_id[Hash] == hash && current_id[Token] == Sys) {
						type = i++;
						expression(type);
						break;
					}
					i++;
					current_id += IdSize;
				}
				//����main���ʶ��
				while (current_id[Token] && i == Main) {
					current_id += IdSize;
					i++;
				}
				//ʶ��return���
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
						//ƥ�亯�������������б�
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
							//����������
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
	//ջ���һ��minFlowerʼ��Ϊ0 
	mf.push(0);
	//Ϊ�ַ������ռ�
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
	src���ı�ָ��
	ch���ļ���ȡ�����ж�ȡ�����ַ�
	*/
	char ch;
	//Ԥ��ؼ���
	src = "int real string while if else in out main return";

	//Ԥ��ϵͳ����
	Pretreatment();

	if (!(src = old_src = (char *)malloc(POOLSIZE))) {
		printf("could not malloc(%d) for source area\n", POOLSIZE);
		return -1;
	}



	int num = 0;
	//��ȡ������ļ��ı�
	while ((*src = getchar()) != EOF) {
		num++;
		src++;
	}
	*src = 0;

	//��ָ��λ���Ƶ��ı�ͷ
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