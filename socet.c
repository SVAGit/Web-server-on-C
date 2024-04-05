#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>

#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_CLIENTS 5
#define PORT 8080
#define BAD_RESPONSE "\nHTTP/1.1 404 NOT FOUND\nServer: Custom HTTP server\n\n"

typedef enum methods{
	GET, POST, HEAD
}methods;

volatile sig_atomic_t flag = 1;
void response(char*, int, char*, int);
char* content_lenght(int);
char* content_type(char*);
char* date(void);
void* applicate(void*);
char* parsParam(char*);
void postFunction(char*, char*, int);
void getFunction(char*, int);

int main() {
	int errno;
	int server_socket;
	if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {//инициализируем сокет на стороне сервера
		perror("server: socket: ");
		return 1;
	}

	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(PORT);


	int opt = 1;
	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	if (bind(server_socket, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1) {// связывем сокет
		perror("server: bind: ");
		return 1;
	}

	if (listen(server_socket, MAX_CLIENTS) == -1) {// прослушиваем
		perror("server: listen: ");
		return 1;
	}
    printf("Сервер успешно запущен на порту %d\n", PORT);
	pthread_t pthread;
	int client_socket;
	while (flag) {
		ssize_t activity = 0;
		fd_set rfds;
		FD_ZERO(&rfds);
		FD_SET(server_socket, &rfds);

		activity = select(server_socket + 1, &rfds, NULL, NULL, NULL);

		if(activity < 0 && errno != EINTR){
			perror("select");
			return 1;
		}else if(FD_ISSET(server_socket, &rfds)){
			struct sockaddr_in client_addr;
			socklen_t client_addr_len = sizeof(client_addr);
			client_socket = accept(server_socket, (struct sockaddr*) &client_addr, &client_addr_len);// присоединяем клиента и получаем информацию о клиенте
			if(client_socket == -1){
				perror("server: accept: ");
				printf("errno - %d\n", errno);
				return 1;
			}
			pthread_create(&pthread, NULL, applicate, &client_socket);
		}
	}
	fprintf(stdout, "\nServer is shutting down\n");
	close(server_socket);
	return 0;
}

int methodPars(char* request){
	char* method = NULL;
	int i = 0;
	while(request[i] != ' '){
		method = realloc(method, (i+ 2)*sizeof(char));
		method[i] = request[i];
		i++;
	}
	method[i] = '\0';
	if(strcmp("GET", method) == 0){
		printf("GET запрос \n");
		free(method);
		return GET;
	}
	if(strcmp("POST", method) == 0){
		printf("POST запрос \n");
		free(method);
		return POST;
	}
	if(strcmp("HEAD", method) == 0){
		printf("HEAD запрос\n");
		free(method);
		return HEAD;
	}
}

char* pathPars(char* request){
	char* path = NULL;
	int i = 0;
	while(request[i] != '/'){
		i++;
	}
	i++;
	int j = 0;
	while(request[i] != ' '){
		path = realloc(path, (j+2)*sizeof(char));
		path[j] = request[i];
		i++;
		j++;
	}
	if(path != NULL){
		path[j] = '\0';
		printf("Путь до запрашиваемого объекта: %s\n", path);
	}else{
		printf("Путь до запрашиваемого объекта: main/index.html\n");
	}
	return path;
}

void* applicate(void* args){
    int client_socket = *(int*)args;
	fd_set rset;
	FD_ZERO(&rset);
	FD_SET(client_socket, &rset);
	if(select(client_socket + 1, &rset, NULL, NULL, NULL) == -1) {
		perror("select");
		shutdown(client_socket, SHUT_RDWR);
		pthread_exit((void*)NULL);
	}
	size_t bufsize;
	if(FD_ISSET(client_socket, &rset)) {
		if(ioctl(client_socket, FIONREAD, &bufsize) == -1) {
			perror("ioctl");
			shutdown(client_socket, SHUT_RDWR);
			pthread_exit((void*)NULL);
		}
	}

	char* request = (char*) calloc(bufsize + 1, sizeof(char));

	if(read(client_socket, request, bufsize) == -1){
		perror("read");
		shutdown(client_socket, SHUT_RDWR);
		pthread_exit((void*) 1);
	}else{
		printf("CLIENT INFO\n");
		if(write(STDOUT_FILENO, request, bufsize) == 0){
			perror("write");
			shutdown(client_socket, SHUT_RDWR);
			close(client_socket);
			client_socket = 0;
			pthread_exit((void*)NULL);
		}
		int method = methodPars(request);
		char* path = pathPars(request);
		response(path, client_socket, request, method);
		shutdown(client_socket, SHUT_RDWR);
		close(client_socket);
		client_socket = 0;
		free(path);
	}
	free(request);
	request = NULL;
	pthread_exit((void*)NULL);
}

void response(char* path, int client_socket, char* request, int method){
	switch(method){
		case GET:
			getFunction(path, client_socket);
			break;
		case POST:
			postFunction(path, request, client_socket);
			break;
		case HEAD:
			break;
	}
}

char* content_type(char* path){
	char a[32];
	char b[] = "text/html";
	char c[] = "text/css";
	char d[] = "text/javascript";
	char e[] = "text/plain";
	char f[] = "image/jpeg";
	char g[] = "image/png";
	char h[] = "image/vnd.microsoft.icon";
	char k[] = "audio/mp4";
	if(strlen(path) == 0 || strcmp(path, "login") == 0 || strcmp(path, "signin") == 0){
		for(int i = 0; i < strlen(b); i++){
			a[i] = b[i];
		}
		a[strlen(b)] = '\0';
	}else{
		int i = 0;
		while(path[i] != '.'){
			i++;
		}
		i++;//индеск следущего за точкой символа или же номер точки в нормальном счете
		int size = strlen(path) - i + 1;// +1 для тернального элемента
		char* format = (char*)malloc(size);
		for(int j= 0; j < size - 1; j++, i++){
			format[j] = path[i];
		}
		format[size - 1] = '\0';
		printf("format: .%s\n", format);
		char a[32];
		if(strcmp("html", format) == 0){
			for(int i = 0; i < strlen(b); i++){
				a[i] = b[i];
			}
			a[strlen(b)] = '\0';
		}
		if(strcmp("css", format) == 0){
			for(int i = 0; i < strlen(c); i++){
				a[i] = c[i];
			}
			a[strlen(c)] = '\0';
		}
		if(strcmp("js", format) == 0){
			for(int i = 0; i < strlen(d); i++){
				a[i] = d[i];
			}
			a[strlen(d)] = '\0';
		}
		if(strcmp("txt", format) == 0){
			for(int i = 0; i < strlen(e); i++){
				a[i] = e[i];
			}
			a[strlen(e)] = '\0';
		}
		if(strcmp("jpeg", format) == 0){
			for(int i = 0; i < strlen(f); i++){
				a[i] = f[i];
			}
			a[strlen(f)] = '\0';
		}
		if(strcmp("png", format) == 0){
			for(int i = 0; i < strlen(g); i++){
				a[i] = g[i];
			}
			a[strlen(g)] = '\0';
		}
		if(strcmp("ico", format) == 0){
			for(int i = 0; i < strlen(h); i++){
				a[i] = h[i];
			}
			a[strlen(h)] = '\0';
		}
		if(strcmp(".mp4", format) == 0){
			for(int i = 0; i < strlen(k); i++){
				a[i] = k[i];
			}
			a[strlen(k)] = '\0';
		}
	}
	char s[] = "Content-type: ";

	int lenght = strlen(a) + strlen(s) + 2;
	char* ret = (char*)calloc(1, lenght);
	strcat(ret, s);
	strcat(ret, a);
	ret[lenght - 2] = '\n';
	ret[lenght - 1] = '\0';
	return ret;
}

char* content_lenght(int lenght){
	char str[10];
	snprintf(str, sizeof(str), "%d", lenght);
	char a[] = "Content-lenght: ";
	size_t size = strlen(str) + strlen(a) + 2;
	char* ret = calloc(1, size);
	strcat(ret, a);
	strcat(ret, str);
	ret[size - 2] = '\n';
	ret[size - 1] = '\0';
	return ret;
}

char* date(void){
	char a[] = "Date: ";
	time_t curtime;
	time(&curtime);
	char* b = ctime(&curtime);
	size_t size = strlen(a) + strlen(b) + 3;
	char* ret = calloc(1, size);
	strcat(ret, a);
	strcat(ret, b);
	ret[size - 3] = '\n';
	ret[size - 2] = '\n';
	ret[size - 1] = '\0';
	return ret;
}


void getFunction(char* path, int client_socket){
	int fd;
	struct stat filestats;
	if(path == NULL){
		path = "main/index.html";
		fd = open("main/index.html", O_RDONLY);
		stat("main/index.html", &filestats);
	}else if(strcmp(path, "login") == 0){
		fd = open("login/index.html", O_RDONLY);
		stat("login/index.html", &filestats);
	}else if(strcmp(path, "signin") ==0){
		fd = open("signin/index.html", O_RDONLY);
		stat("signin/index.html", &filestats);
	}else{
		fd = open(path, O_RDONLY);
		stat(path, &filestats);
	}
	if(fd == -1){
		char response[] = BAD_RESPONSE;
		write(client_socket, response, strlen(response));
		write(STDOUT_FILENO, response, strlen(response));
	}else{
		printf("Размер файла равен: %ld\n", filestats.st_size);
		char* body = (char*) calloc(filestats.st_size, sizeof(char));
		read(fd, body, filestats.st_size);
		char response[] = "\nHTTP/1.1 200 OK\nServer: Custom HTTP server\n";
		char* type = content_type(path);
		char* lenght = content_lenght(filestats.st_size);
		char* data = date();
		size_t size = strlen(response) + strlen(type) + strlen(lenght) + strlen(data)+ strlen(body);
		char* final = (char*) calloc(size, sizeof(char));
		strcat(final, response);
		strcat(final, type);
		strcat(final, lenght);
		strcat(final, data);
		write(STDOUT_FILENO, final, size);
		strcat(final, body);
		write(client_socket, final, size);
		free(final);
	}
}


void postFunction(char* path, char* request, int client_socket){
	char* params = parsParam(request);
	int dataBase = open("database.json", O_RDWR, 0666);
	struct stat dataBaseInfo;
	stat("database.json", &dataBaseInfo);
	if(dataBaseInfo.st_size < 1){
		write(dataBase, "[", sizeof(char));
	}else{
		printf("Ofset:%ld\n",lseek(dataBase, -1, SEEK_END));
		write(dataBase, ",", sizeof(char));
		write(dataBase, "\n", sizeof(char));
		write(dataBase, "\n", sizeof(char));
	}
	write(dataBase, "{", sizeof(char));
	write(dataBase, "\n", sizeof(char));
	write(dataBase, "\t", sizeof(char));
	write(dataBase, "\"", sizeof(char));
	for(int i = 0; params[i] != '\0'; i++){
		if(params[i] == ':'){
			write(dataBase,"\"" , sizeof(char));
		}
		if(params[i] == '&'){
			write(dataBase, "\"", sizeof(char));
			write(dataBase, ",", sizeof(char));
			write(dataBase, "\n", sizeof(char));
			write(dataBase, "\t", sizeof(char));
			write(dataBase, "\"", sizeof(char));
			continue;
		}
		write(dataBase, &(params[i]), sizeof(char));
		if(params[i] == ':'){
			write(dataBase,"\"" , sizeof(char));
		}
	}
	write(dataBase, "\"", sizeof(char));
	write(dataBase, "\n", sizeof(char));
	write(dataBase, "}", sizeof(char));
	write(dataBase, "]", sizeof(char));
	close(dataBase);

	int fd;
	struct stat filestats;
	if(path == NULL){
		path = "main/index.html";
		fd = open("main/index.html", O_RDONLY);
		stat("main/index.html", &filestats);
	}else if(strcmp(path, "login") == 0){
		fd = open("login/index.html", O_RDONLY);
		stat("login/index.html", &filestats);
	}else{
		fd = open(path, O_RDONLY);
		stat(path, &filestats);
	}
	if(fd == -1){
		char response[] = BAD_RESPONSE;
		write(client_socket, response, strlen(response));
		write(STDOUT_FILENO, response, strlen(response));
	}else{
		printf("Размер файла равен: %ld\n", filestats.st_size);
		char* body = (char*) calloc(filestats.st_size, sizeof(char));
		read(fd, body, filestats.st_size);
		char response[] = "\nHTTP/1.1 200 OK\nServer: Custom HTTP server\n";
		char* type = content_type(path);
		char* lenght = content_lenght(filestats.st_size);
		char* data = date();
		size_t size = strlen(response) + strlen(type) + strlen(lenght) + strlen(data)+ strlen(body);
		char* final = (char*) calloc(size, sizeof(char));
		strcat(final, response);
		strcat(final, type);
		strcat(final, lenght);
		strcat(final, data);
		write(STDOUT_FILENO, final, size);
		strcat(final, body);
		write(client_socket, final, size);
		free(final);
	}
	free(params);
}

char* parsParam(char* request){
	char* retval = calloc(1, sizeof(char));
	int flag = 0;
	int charCount = 1;
	int j = 0;
	for(int i = 0; request[i] != '\0'; i++){
		if(flag){
			retval = realloc(retval, charCount + 1);
			if(request[i] == '='){
				retval[charCount - 1] = ':';
			}else{
				retval[charCount - 1] = request[i];
			}
			charCount++;
			continue;
		}else if(request[i] == '\n'){
			if(i == j + 2){
				flag = 1;
			}
			j = i;
			continue;
		}
		flag = 0;
	}
	retval[charCount - 1] = '\0';
	return retval;
}