//Author: Furkan Bingöl

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#define MAX 10000


typedef struct vertex{
    int vno;
    int indegree;
    int tmp_indegree;
    int indirect_direct_connections;
    char *fname;
    char *lname;
}VERTEX;


int isDigit(char c);
int hasEdge(int **adj, int u, int v);
void find_verticesNum_text(char *fileName, int *verticesNum, char **text);
void fill_matrix(int **adj, VERTEX *vertex, char *text, int verticesNum);
void calculate_indegree(int **adj, VERTEX *vertex, int verticesNum);
void first_eliminate(int **adj, VERTEX *vertex, int verticesNum, int M, int *is_influencer);
void find_total_connections(int **adj, int *visited, VERTEX *vertex, int verticesNum, int *is_influencer);
int has_path(int src, int dest, int **adj, int *visited, int verticesNum);  //DFS Approach
void find_and_print_influencers(VERTEX *vertex, int verticesNum, int X, int Y, int *is_influencer, int mode);


int main()
{
    int verticesNum, i;
    int M,X,Y,choose;
    int *visited, *is_influencer;
    int **adj;
    char *text;
    VERTEX *vertex;

    find_verticesNum_text("socialNetwork.txt", &verticesNum, &text);
    verticesNum++;

    is_influencer = (int *)calloc(verticesNum, sizeof(int));
    vertex = (VERTEX *)calloc(verticesNum, sizeof(VERTEX));
    adj = (int **)calloc(verticesNum, sizeof(int *));       //adjacency matrix
    for(i = 0; i < verticesNum; i++){
        adj[i] = (int *)calloc(verticesNum, sizeof(int));
    }
    visited = (int *)calloc(verticesNum, sizeof(int));      //visited array

    fill_matrix(adj, vertex, text, verticesNum);
    calculate_indegree(adj, vertex, verticesNum);

    printf("Enter M: ");
    scanf("%d", &M);
    printf("Enter X: ");
    scanf("%d", &X);
    printf("Enter Y: ");
    scanf("%d", &Y);
    if(M >= X){
        printf("M must be less than to X!\n");
        exit(0);
    }
    
    printf("-----------------------\n#### Make a choice ####\n\n");
    printf("1- Normal modda calistirma\n2- Detay modda calistirma\n");
    scanf("%d",&choose);

    if(choose == 1)
    {
        printf("\nNormal modda calistiriliyor...\n");
        first_eliminate(adj, vertex, verticesNum, M, is_influencer);
        find_total_connections(adj, visited, vertex, verticesNum, is_influencer);
        find_and_print_influencers(vertex, verticesNum, X, Y, is_influencer, 1);
    }
    else if(choose == 2)
    {
        printf("\nDetay modda calistiriliyor...\n\n");
        printf("---In-degree values in the initial state---\n");
        for(i = 0; i < verticesNum-1; i++)
            printf("VertexNo: %2d --> Indegree: %2d\n", vertex[i].vno, vertex[i].indegree);
        first_eliminate(adj, vertex, verticesNum, M, is_influencer);

        printf("\n---Nodes that are not eliminated by M---\n");
        for(i = 0; i < verticesNum-1; i++){
            if(is_influencer[i] != -1){
                printf("VertexNo: %2d , Indegree: %2d --> FName: %10s , LName: %10s\n", vertex[i].vno, vertex[i].indegree, vertex[i].fname, vertex[i].lname);
            }
        }
        find_total_connections(adj, visited, vertex, verticesNum, is_influencer);
        find_and_print_influencers(vertex, verticesNum, X, Y, is_influencer, 2);
    }
    else
        printf("Error, wrong choice!");

    //freeleme
    for(i = 0; i < verticesNum; i++)
        free(adj[i]);
    free(adj);
    free(visited);
    free(is_influencer);
    free(text);
    free(vertex);
    return 0;
}


void find_verticesNum_text(char *fileName, int *verticesNum, char **text)
{
    int fd = open(fileName, O_RDONLY);
    if(fd < 0){
        printf("Error opening file!");
        exit(0);
    }

    int backslash_n = 0, val = 0;
    char *str = (char *)malloc(MAX * sizeof(char));
    int len = read(fd, str, MAX) - 1;
    
    while(backslash_n < 2){ 
        if(str[len] == '\n')
            backslash_n++;
        len--;
    }
    len += 2;
    while(str[len] >= '0' && str[len] <= '9'){
        val = val * 10 + (str[len] - '0');
        len++;
    }
    *verticesNum = val;
    *text = str;
    close(fd);
}

void fill_matrix(int **adj, VERTEX *vertex, char *text, int verticesNum)
{
    int i = 0, j, tmp;
    int mod = 0;
    int val, tur = 0;
    char *str;

    while(text[i] != '\0')
    {
        if(text[i] == '\n' && mod == 0)
        {
            tmp = i + 1;
            while(text[tmp] && text[tmp] != '\n' && text[tmp] != ',')
                tmp++;
            if(text[tmp] == ',')
                tmp++;
            if((text[tmp] >= '0' && text[tmp] <= '9') || text[tmp] == '\n')
                mod = 1;
            else{
                mod = 0;
                tur++;
            }
        }
        else if(text[i] == '\n' && mod == 1){
            mod = 0;
            tur++;
        }
        else if(text[i] != ','){
            if(mod == 0){     //mod == 0
                if(isDigit(text[i])){
                    val = 0;
                    while(isDigit(text[i])){
                        val = val * 10 + (text[i] - '0');
                        i++;
                    }
                    vertex[tur].vno = val;
                }
                else{
                    vertex[tur].fname = (char *)malloc(100 * sizeof(char));
                    vertex[tur].lname = (char *)malloc(100 * sizeof(char));

                    j = 0;
                    str = (char *)malloc(100 * sizeof(char));
                    while(text[i] != ','){
                        str[j] = text[i];
                        i++;
                        j++;
                    }
                    str[j] = '\0';
                    strcpy(vertex[tur].fname, str);
                    free(str);

                    str = (char *)malloc(100 * sizeof(char));
                    j = 0;
                    i++;
                    while(text[i] != '\0' && text[i] != '\n'){
                        str[j] = text[i];
                        i++;
                        j++;
                    }
                    str[j] = '\0';
                    strcpy(vertex[tur].lname, str);
                    i--;
                    free(str);
                }
            }
            else{         //mod == 1
                val = 0;
                while(isDigit(text[i])){
                    val = val * 10 + (text[i] - '0');
                    i++;
                }
                adj[vertex[tur].vno][val] = 1;
                if(text[i] == '\n' || text[i] == '\0')
                    i--;
            }
        }
        i++;
    }
}

void calculate_indegree(int **adj, VERTEX *vertex, int verticesNum)
{
    int i, j;
    for(i = 1; i < verticesNum; i++){      
        for(j = 1; j < verticesNum; j++){
            if(hasEdge(adj, j, i))
                vertex[i-1].indegree++;
        }
        vertex[i-1].tmp_indegree = vertex[i-1].indegree;
    }
}

void first_eliminate(int **adj, VERTEX *vertex, int verticesNum, int M, int *is_influencer)
{
    int flag = 1;
    int i,j;
    is_influencer[verticesNum-1] = -1;

    while(flag)
    {
        for(i = 0; i < verticesNum-1; i++){
            if(vertex[i].tmp_indegree < M){
                is_influencer[i] = -1;
                vertex[i].tmp_indegree = 0;
                for(j = 0; j < verticesNum-1; j++){
                    if(i != j && hasEdge(adj, vertex[i].vno, vertex[j].vno)){
                        vertex[j].tmp_indegree--;
                        if(vertex[j].tmp_indegree < 0)
                            vertex[j].tmp_indegree = 0;
                    }
                }
            }
        }
        i = 0;
        while(i < verticesNum-1 && (vertex[i].tmp_indegree >= M || is_influencer[i] == -1))
            i++;
        if(i == verticesNum-1)
            flag = 0;        //first_eliminate function finished
    }
}

void find_total_connections(int **adj, int *visited, VERTEX *vertex, int verticesNum, int *is_influencer)
{
    int i, j;

    for(i = 1; i < verticesNum; i++){
        for(j = 1; j < verticesNum; j++){
            if(i != j && is_influencer[j-1] != -1){              //i = source, j = destination
                memset(visited, 0, verticesNum * sizeof(int))
                if(has_path(i, j, adj, visited, verticesNum))
                    vertex[j-1].indirect_direct_connections++;
            }
        }
    }
}

//DFS Approach
int has_path(int src, int dest, int **adj, int *visited, int verticesNum) {
	int i;
    visited[src] = 1;

    if (src == dest)
        return 1;

    for (i = 1; i < verticesNum; i++) {
        if (i != src && adj[src][i] && !visited[i]) {
            if (has_path(i, dest, adj, visited, verticesNum))
                return 1;
        }
    }
    return 0;
}

void find_and_print_influencers(VERTEX *vertex, int verticesNum, int X, int Y, int *is_influencer, int mode)
{
    int flag = 0;
    int i;

    printf("\n|| Influencers ||\n");
    for(i = 0; i < verticesNum-1; i++){
        if(is_influencer[i] != -1){     //if vertex is not eliminated in first_eliminate function
            if(mode == 1){
                if(vertex[i].tmp_indegree >= X && vertex[i].indirect_direct_connections >= Y){
                    printf("VNo: %2d   FName: %10s   LName: %10s\n", vertex[i].vno, vertex[i].fname, vertex[i].lname);
                    flag = 1;
                }
            }
            else if(mode == 2){
                if(vertex[i].tmp_indegree >= X && vertex[i].indirect_direct_connections >= Y){
                    printf("VNo: %2d  Indegree: %2d  AllConnections: %2d  FName: %10s   LName: %10s\n", vertex[i].vno, vertex[i].indegree, vertex[i].indirect_direct_connections, vertex[i].fname, vertex[i].lname);
                    flag = 1;
                }
            }
        }
    }
    if(!flag){
        printf("No influencers found!\n");
        exit(0);
    }
}

int isDigit(char c)
{
    return (c >= '0' && c <= '9');
}

int hasEdge(int **adj, int u, int v)
{
    return adj[u][v];
}
