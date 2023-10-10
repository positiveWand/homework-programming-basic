#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "json_c.c"

#define FUNC_MAX 128

typedef struct {
    char name[128];
    char type[32];
} ParameterInfo;

typedef struct {
    char name[128];
    char type[32];
    int params_count;
    ParameterInfo params[32];
    int if_count;
} FunctionInfo;

FunctionInfo* new_func_info()
{
    return malloc(sizeof(FunctionInfo));
}

// 함수 분석 정보 -> 문자열 변환
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

// 타입 노드 타입 추출(재귀)
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

// 함수 이름 추출
char* parse_func_name(json_value funcDef)
{
    json_value decl = json_get(funcDef, "decl");
    json_value name = json_get(decl, "name");

    return json_to_string(name);
}

// 함수 반환 타입 추출
char* parse_func_type(json_value funcDef)
{
    json_value decl = json_get(funcDef, "decl");
    json_value type = json_get(decl, "type");
    json_value type_type = json_get(type, "type");
    return parse_type(type_type);
}

// 함수 매개변수 이름 추출
char* parse_param_name(json_value param)
{
    json_value ntype = json_get(param, "_nodetype");
    if(strcmp(json_to_string(ntype), "Decl") != 0)
        return "NULL";

    json_value name = json_get(param, "name");
    return json_to_string(name);
}
// 함수 매개변수 타입 추출
char* parse_param_type(json_value param)
{
    json_value ntype = json_get(param, "_nodetype");
    if(strcmp(json_to_string(ntype), "Decl") != 0)
        return "NULL";

    json_value type = json_get(param, "type");
    return parse_type(type);
}

// 함수 매개변수 목록 추출
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

// 함수 내 if문 개수 추출(재귀)
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
    // printf("%s", json_buff);

    json_value ast = json_create(json_str);
    json_value ext = json_get(ast, "ext");

    int top_count = json_len(ext);
    // printf("top level len: %d\n", top_count);

    // json_fprint_value(stdout, ext, 0);

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
            // func_lst[func_count]->params_count = json_len(params);
    
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

    // 분석 결과 출력
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