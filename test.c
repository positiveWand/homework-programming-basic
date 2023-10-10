struct TestStruct1_s{
    char name[128];
    char type[32];
    int params_count;
};
typedef struct TestStruct2_s{
    char name[20];
    int age;
    double height;
} TestStruct2;

int someGlobalVar = 2000;

void headeronlyfunc(char[100]);

int test_func1() {
    return 1;
}

double test_func2() {
    return 1;
}

unsigned int test_func3() {
    return 1;
}

char* test_func4() {
    char* aString = "hello";

    return aString;
}

char** test_func5() {
    char** ptrptr;

    return ptrptr;
}

struct TestStruct1_s test_func6() {
    struct TestStruct1_s s = {};

    return s;
}

TestStruct2 test_func7() {
    TestStruct2 s = {};

    return s;
}

int test_func8(int a, int b, int c) {
    return 1;
}

int test_func9(unsigned int a, double b, char* c) {
    return 1;
}

int test_func10(char s1[], char s2[][10], char** kwargs, ...) {
    return 1;
}

int test_func11(struct TestStruct1_s s1, struct TestStruct1_s* s2, TestStruct2 s3, TestStruct2* s4) {
    return 1;
}

int test_func12(void) {
    return 1;
}

void test_func13() {
    return;
}

void test_func14() {
    int a = 3;
    int b = 10;
    if(a == b)
        return;

    if(a == b) {
        printf("true");
    } else {
        printf("false");
    }

    if(a == b) {
        if(a == b) {
            if(a != b) {
                printf("hello");
            }
        } else {
            printf("world.");
        }
    }

    if(a == b)
        if(b == b)
            return;

    while(1) {
        while(1) {
            if(a == b) {
                break;
            }
        }
    }

    for(int i = 0; i < 10; i++) {
        for(int j = 0; j < 10; j++) {
            printf("hello!");
            if(a == b) {
                break;
            }
        }
    }
    {
        if(a == b)
            if(b == b)
                return;
    }
}

void test_func15() {
    int a = 10;
    switch (a)
    {
        case 1:
            printf("1");
            break;
        case 2:
            printf("1");
            if(a == 2) {
                printf("hello1");
                if(a != 3) {
                    printf("world!");
                }
            }
            break;
        case 3:
            printf("1");
            break;
        case 4:
            printf("1");
            break;
        default:
            printf("default");
            break;
    }
}

typedef enum json_type_enum { JSON_UNDEFINED = 0x0, JSON_NUMBER = 0x1, JSON_STRING=0x2, JSON_BOOLEAN=0x4, JSON_ARRAY=0x8, JSON_OBJECT=0x10, JSON_NULL=0x20, JSON_INTEGER=0x40, JSON_DOUBLE=0x80 } json_type;
typedef enum json_keyorvalue_enum { JSON_KEY, JSON_VALUE } json_keyorvalue;
typedef struct json_small_stack_s{
	int top;
	int type[20];
	const void * stacktrace[20];
} json_small_stack;
typedef struct json_value_s {
    json_type type;
    void* value;
} json_value;

#define NULL (void*) 0
#define SEEK_END 2
#define SEEK_SET 0
#define EOF (-1)
#define FUNC_MAX 128

json_value json_get(json_value, void*);
json_value json_create(const char* json_message);
static const json_value undefined_json = {JSON_UNDEFINED, NULL};

typedef struct {} FILE;


typedef struct {
    char name[128];
    char type[32];
} ParameterInfo;

typedef struct {
    char name[128];
    char type[32];
    int params_count;
    ParameterInfo params[16];
    int if_count;
} FunctionInfo;

FunctionInfo* new_func_info()
{
    return malloc(sizeof(FunctionInfo));
}

char* func_info_to_string(FunctionInfo info)
{
    char* buffer1;
    char* buffer2;

    buffer1 = malloc(sizeof(char) * 256 * (info.params_count+2));
    memset(buffer1, 0, sizeof(char) * 256 * (info.params_count+2));

    buffer2 = malloc(sizeof(char) * 256 * (info.params_count+1));
    memset(buffer2, 0, sizeof(char) * 256 * (info.params_count+1));

    char* buffer3;
    buffer3 = malloc(sizeof(char) * 256);
    memset(buffer3, 0, sizeof(char) * 256);

    for(int i = 0; i < info.params_count; i++) {
        memset(buffer3, 0, sizeof(char) * 256);

        sprintf(buffer3, "\t\t{\n\t\t\t매개변수명: %s,\n\t\t\t타입: %s\n\t\t}",
            info.params[i].name,
            info.params[i].type
        );
        strcat(buffer2, buffer3);
        if(i != info.params_count - 1)
            strcat(buffer2, ",\n");
    }

    free(buffer3);

    sprintf(buffer1, "{\n\t함수 이름: %s,\n\t반환 타입: %s,\n\t매개변수: [\n%s\n\t],\n\tif문 개수: %d\n}", 
        info.name,
        info.type,
        buffer2,
        info.if_count
    );
    free(buffer2);

    return buffer1;
}

char* parse_type(json_value type)
{
    json_value ntype = json_get(type, "_nodetype");
    char* buffer;
    buffer = malloc(sizeof(char) * 128);
    memset(buffer, 0, sizeof(char) * 128);

    if(strcmp(json_to_string(ntype), "PtrDecl") == 0)
    {
        char* pre = parse_type(json_get(type, "type"));
        strcat(buffer, pre);
        strcat(buffer, "*");
        free(pre);
    }
    else if(strcmp(json_to_string(ntype), "ArrayDecl") == 0) {
        char* pre = parse_type(json_get(type, "type"));
        
        char* p1 = buffer;
        char* p2 = pre;
        char* p3 = NULL;

        for(int i = 0; i < strlen(pre); i++) {
            char c = *p2;
            if(c == '[') {
                p3 = p2;
                break;
            }
            *p1 = *p2;
            p1++;
            p2++;
        }

        json_value dim = json_get(type, "dim");

        if(json_is_null(dim)) {
            strcat(buffer, "[]");
        } else {
            json_value dim_value = json_get(dim, "value");
            strcat(buffer, "[");
            strcat(buffer, json_to_string(dim_value));
            strcat(buffer, "]");
        }

        if(p3 != NULL) {
            strcat(buffer, p3);
        }

        free(pre);
    }
    else if(strcmp(json_to_string(ntype), "TypeDecl") == 0) {
        char* pre = parse_type(json_get(type, "type"));
        strcat(buffer, pre);
        free(pre);
    }
    else if(strcmp(json_to_string(ntype), "IdentifierType") == 0) {
        json_value names = json_get(type, "names");
        for(int i = 0; i < json_len(names); i++)
        {
            json_value name = json_get(names, i);
            strcat(buffer, json_to_string(name));

            if(i != json_len(names) - 1)
                strcat(buffer, " ");
        }
    }
    else if(strcmp(json_to_string(ntype), "Struct") == 0) {
        char* name = json_to_string(json_get(type, "name"));
        strcat(buffer, name);
    }

    return buffer;
}

char* parse_func_name(json_value funcDef)
{
    json_value decl = json_get(funcDef, "decl");
    json_value name = json_get(decl, "name");

    return json_to_string(name);
}

char* parse_func_type(json_value funcDef)
{
    json_value decl = json_get(funcDef, "decl");
    json_value type = json_get(decl, "type");
    json_value type_type = json_get(type, "type");
    return parse_type(type_type);
}


char* parse_param_name(json_value param)
{
    json_value ntype = json_get(param, "_nodetype");
    if(strcmp(json_to_string(ntype), "Decl") != 0)
        return "NULL";

    json_value name = json_get(param, "name");
    return json_to_string(name);
}
char* parse_param_type(json_value param)
{
    json_value ntype = json_get(param, "_nodetype");
    if(strcmp(json_to_string(ntype), "Decl") != 0)
        return "NULL";

    json_value type = json_get(param, "type");
    return parse_type(type);
}

json_value get_params(json_value funcDef)
{
    json_value decl = json_get(funcDef, "decl");
    json_value type = json_get(decl, "type");
    json_value args = json_get(type, "args");

    if(json_is_null(args))
    {
        return undefined_json;
    }

    json_value params = json_get(args, "params");

    return params;
}

int count_if_stmt(json_value stmt)
{
    if(json_is_null(stmt))
        return 0;
    
    int if_count = 0;
    json_value ntype = json_get(stmt, "_nodetype");

    if(strcmp(json_to_string(ntype), "Compound") == 0)
    {
        json_value block_items = json_get(stmt, "block_items");
        
        if(json_is_null(block_items))
            return 0;

        for(int i = 0; i < json_len(block_items); i++) {
            json_value aStmt = json_get(block_items, i);
            if_count += count_if_stmt(aStmt);
        }
    } else if(strcmp(json_to_string(ntype), "If") == 0)
    {
        if_count++;

        json_value true_stmt = json_get(stmt, "iftrue");
        json_value false_stmt = json_get(stmt, "iffalse");

        if_count += count_if_stmt(true_stmt);
        if_count += count_if_stmt(false_stmt);
    } else if(strcmp(json_to_string(ntype), "Switch") == 0)
    {
        json_value aStmt = json_get(stmt, "stmt");
        
        if_count += count_if_stmt(aStmt);
    } else if(strcmp(json_to_string(ntype), "Case") == 0)
    {
        json_value stmts = json_get(stmt, "stmts");
        
        if(json_is_null(stmts))
            return 0;

        for(int i = 0; i < json_len(stmts); i++) {
            json_value aStmt = json_get(stmts, i);
            if_count += count_if_stmt(aStmt);
        }
    } else if(strcmp(json_to_string(ntype), "While") == 0)
    {
        json_value repeat_stmt = json_get(stmt, "stmt");
        
        if_count += count_if_stmt(repeat_stmt);
    } else if(strcmp(json_to_string(ntype), "For") == 0)
    {
        json_value repeat_stmt = json_get(stmt, "stmt");
        
        if_count += count_if_stmt(repeat_stmt);
    }

    return if_count;
}

int main(void)
{
    int func_count = 0;
    FunctionInfo* func_lst[FUNC_MAX];

    FILE *fp = fopen("./ast.json", "r");
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *json_str = malloc(fsize + 1);
    fread(json_str, fsize, 1, fp);
    fclose(fp);

    if(fp == NULL)
    {
        printf("AST JSON 파일 열기 실패.\n");
        return 1;
    }
    else
    {
        printf("AST JSON 파일 열기 성공.\n");
    }

    printf("JSON 파일 크기: %d\n", strlen(json_str));

    json_value ast = json_create(json_str);
    json_value ext = json_get(ast, "ext");

    int top_count = json_len(ext);
    printf("top level len: %d\n", top_count);

    for(int i = 0; i < top_count; i++)
    {
        json_value item = json_get(ext, i);
        json_value ntype = json_get(item, "_nodetype");
        char* nodetype = json_to_string(ntype);

        if(strcmp("FuncDef", nodetype) != 0)
            continue;
        
        func_lst[func_count] = new_func_info();

        // 함수 관련 정보 추출
        // 함수 이름 추출
        strncpy(func_lst[func_count]->name, parse_func_name(item), 128);
        // 함수 반환타입 추출
        strncpy(func_lst[func_count]->type, parse_func_type(item), 32);

        // 함수 매개변수 개수 추출
        json_value params = get_params(item);
        if(params.type == JSON_UNDEFINED) {
            // 매개변수가 존재하지 않는 경우
            // 함수 매개변수 개수 추출
            func_lst[func_count]->params_count = 0;
        }
        else {
            // 매개변수가 존재하는 경우
            // 함수 매개변수 개수 추출
            int params_count = 0;
    
            // 함수 매개변수 정보(이름, 타입) 추출
            for(int j = 0; j < json_len(params); j++)
            {
                json_value param = json_get(params, j);

                // 이름이 없는 매개변수 제외
                if(strcmp("NULL", parse_param_name(param)) == 0)
                    continue;
                params_count++;

                // 매개변수 이름 추출
                strncpy((func_lst[func_count]->params)[j].name, parse_param_name(param), 128);
                // 매개변수 타입 추출
                strncpy((func_lst[func_count]->params)[j].type, parse_param_type(param), 32);
            }

            func_lst[func_count]->params_count = params_count;
        }

        // 함수 내 if문 개수 추출
        json_value body = json_get(item, "body");
        func_lst[func_count]->if_count = count_if_stmt(body);

        // 함수 개수 증가
        func_count++;
    }

    printf("함수 총 개수: %d\n", func_count);
    printf("[\n", func_count);
    for(int i = 0; i < func_count; i++) {
        printf("%s", func_info_to_string(*func_lst[i]));
        if(i != func_count - 1)
            printf(",");
        printf("\n");
    }
    printf("]\n");
}

typedef struct {
    char name[20];
    int age;
    double height;
} Student;

void load_data(int size, json_value datas, Student *students)
{
    for(int i=0; i<size; i++)
    {
        json_value obj = json_get(datas, i);
        // json_print(obj);
        char *name = json_get_string(obj, "name");
        int age = json_get_int(obj, "age");
        double height = json_get_double(obj, "height");

        strcpy(students[i].name, name);
        students[i].age = age;
        students[i].height = height;
    }
}

void find_student_by_name(int size, Student *students, char *find_name)
{
    for(int i=0; i<size; i++)
    {
        Student temp_student = students[i];
        // 일치하면 strcmp는 0을 리턴함
        if(!strcmp(temp_student.name, find_name))
        {
            printf("이름: %s, 나이: %d, 키: %lf \n", temp_student.name, temp_student.age, temp_student.height);
            return;
        }
    }
    printf("정보가 존재하지 않습니다.");
}

void find_student_by_bio(int size, Student *students, int min_age, int max_age, double min_height, double max_height)
{

    for(int i=0; i<size; i++)
    {
        Student temp_student = students[i];
        if((temp_student.age >= min_age && temp_student.age <= max_age) || (temp_student.height >= min_height && temp_student.height <= max_height))
        {
            printf("이름: %s, 나이: %d, 키: %lf \n", temp_student.name, temp_student.age, temp_student.height);
        }       
    }
}

void save_all(int size, Student *students)
{
    FILE *fp = fopen("student_all.txt", "w");

    if(fp == NULL)
    {
        printf("파일열기 실패 \n");
        return;
    }

    for(int i=0; i<size; i++)
    {
        Student student = students[i];
        // fputs또는 fprintf활용 가능
        fprintf(fp, "이름: %s, 나이: %d, 키: %lf \n", student.name, student.age, student.height);
        fputs("========================================\n", fp);
    }
    fclose(fp);
}

void save_partial(int size, Student *students)
{
    FILE *fp = fopen("student_partial.txt", "w");

    if(fp == NULL)
    {
        printf("파일열기 실패 \n");
        return;
    }

    // 이름, 나이, 키 등 조건활용 가능
    // 본 예제는 이름을 조건으로하여 일부 인원만 저장
    char temp_name[20];
    printf("저장할 이름 입력 \n");
    scanf("%s", temp_name);

    for(int i=0; i<size; i++)
    {
        Student student = students[i];
        
        if(!(strcmp(student.name, temp_name)))
        {
            // fputs또는 fprintf활용 가능
            fprintf(fp, "이름: %s, 나이: %d, 키: %lf \n", student.name, student.age, student.height);
            fputs("========================================\n", fp);
        }
    }
    fclose(fp);
}


void display_students_all()
{
    FILE *fp = fopen("student_all.txt", "r");

    if(fp == NULL)
    {
        printf("파일열기 실패\n");
        return;
    }

    int c =0;
    while((c = fgetc(fp))!=EOF)
    {
        putchar(c);
    }
    fclose(fp);
}

void display_students_partial()
{
    // 이름, 나이, 키 등 조건활용 가능
    // 본 예제는 이름을 조건으로하여 일부 인원만 출력
    char temp_name[20];
    printf("출력할 이름 입력 \n");
    scanf("%s", temp_name);
    FILE *fp = fopen("student_all.txt", "r");

    if(fp == NULL)
    {
        printf("파일열기 실패\n");
        return;
    }

    char buf[100];
    char copy_buf[100];
    while (fgets(buf, sizeof(buf), fp) != NULL)
    {
        strcpy(copy_buf, buf);
        // 이름을 찾기 위해 buf에서 이름 필드를 추출
        // 이름: ~~~
        char *name = strtok(buf, ",");
        // temp_name과 비교하여 일치하는 경우에만 출력
        if (name != NULL && strstr(name, temp_name) != NULL) 
        {
            printf("%s\n", copy_buf);
            fclose(fp);
            break;
        }
    }
}

void update_student(char *update_name)
{
    FILE *open_file = fopen("student_all.txt", "r");
    FILE *write_file = fopen("student_all_temp.txt", "w");


    if(open_file == NULL || write_file == NULL)
    {
        printf("파일열기 실패 \n");
        return;
    }

    char buf[100];
    char copy_buf[100];
    while (fgets(buf, sizeof(buf), open_file) != NULL) 
    {
        strcpy(copy_buf, buf);

        // 이름을 찾기 위해 buf에서 이름 필드를 추출
        char *name = strtok(buf, ",");
        // temp_name과 비교하여 일치하는 경우에만 수정
        // 나이를 5살로 강제로 수정
        if (name != NULL && strstr(name, update_name) != NULL) 
        {      
            // 나이: 시작하는 위치 찾기     

            char *age_position = strstr(copy_buf, "나이: ");
            if(age_position != NULL)
            {
                strncpy(age_position, "나이: 15", strlen("나이: 15"));
                fprintf(write_file, "%s", copy_buf);
            }
        }
        else 
        {   
            fprintf(write_file,"%s", copy_buf);
        }
    }
    fclose(open_file);
    fclose(write_file);

    int remove_original_file = remove("student_all.txt");
    if(remove_original_file == 0)
	{
		if(rename("student_all_temp.txt", "student_all.txt") != 0)
        {
            printf("파일 재생성 에러발생\n");
        }
	}
	else if(remove_original_file == -1)
	{
		printf( "파일 삭제 실패" );
	}
}


void remove_student(char *remove_name)
{
    FILE *open_file = fopen("student_all.txt", "r");
    FILE *write_file = fopen("student_all_temp.txt", "w");


    if(open_file == NULL || write_file == NULL)
    {
        printf("파일열기 실패 \n");
        return;
    }

    char buf[100];
    char copy_buf[100];

    // 이전에 지워졌는지 확인하기 위한 flag
    // -1: 이전에 안지워짐
    // 1: 이전에 지워짐
    int before_delete = -1;

    while (fgets(buf, sizeof(buf), open_file) != NULL) 
    {
        strcpy(copy_buf, buf);

        // 이름을 찾기 위해 buf에서 이름 필드를 추출
        char *name = strtok(buf, ",");
        // temp_name과 비교하여 일치하는 경우에만 삭제
        if ((name != NULL && strstr(name, remove_name) != NULL) || before_delete == 1) 
        {     
            before_delete = before_delete * (-1);
        }
        else 
        {   
            fprintf(write_file,"%s", copy_buf);
        }
    }
    fclose(open_file);
    fclose(write_file);

    int remove_original_file = remove("student_all.txt");
    if(remove_original_file == 0)
	{
		if(rename("student_all_temp.txt", "student_all.txt") != 0)
        {
            printf("파일 재생성 에러발생\n");
        }
	}
	else if(remove_original_file == -1)
	{
		printf( "파일 삭제 실패" );
	}
}
