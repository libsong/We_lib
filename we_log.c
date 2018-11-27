/*日志记录*/  
#include <stdio.h>  
#include <stdlib.h>  
#include <time.h>  
#include <unistd.h>  
#include <assert.h>  
#include <string.h>  
#include <fcntl.h>  
#include <stdarg.h>  

//int main()  
//{  
//	log_file();  
//	write_cmd("the address for cmd:%d\n", 100);  
//	write_log("the address for log:%d\n", 200);  
//	close_file();  
//	return 0;  
//}  
  
enum switch_mode  
{  
	mode_minute,  
	mode_hour,  
	mode_day,  
	mode_month  
};  

int we_logfd = -1;  
#define WELOG_NAME "/tmp/kl_simulik-log.txt"

//int log_file(switch_mode mode = mode_day)  
int WeLog_init(void)  
{  
	char file_path[512] = { 0 };  
	char filetime[32] = { 0 };  
	struct tm tm_time;  
	time_t t_log;  
	char log_time[512] = "";  
  
//	assert(getcwd(file_path, 512) != NULL);      //当前目录  
//	if(file_path[strlen(file_path) - 1] != '/') {  
//		file_path[strlen(file_path)] = '/';  
//	}  
//	if (access(file_path, F_OK) != 0) {
//		//目录不存在  
//   std::string build_path = "mkdir -p ";  
//		build_path += file_path;  
//		assert(system(build_path.c_str()) != 0);  
//	}  
  
//	t_log = time(NULL);  
//	localtime_r(&t_log, &tm_time);  
//	strftime(filetime, sizeof(filetime), "%Y%m%d%H%M%S", &tm_time);   //日志的时间  
//	switch(mode) {
//		//日志存储模式  
//  case mode_minute :  
//      log_time.assign(filetime, 0, 12);  
//		break;  
//	case mode_hour:  
//		log_time.assign(filetime, 0, 10);  
//		break;  
//	case mode_day:  
//		log_time.assign(filetime, 0, 8);  
//		break;  
//	case mode_month:  
//		log_time.assign(filetime, 0, 6);  
//		break;  
//	default:  
//		log_time.assign(filetime, 0, 8);  
//	}  
//	strcat(file_path, "log_");  
//	strcat(file_path, log_time.c_str());  
//	strcat(file_path, ".log");  
  
	we_logfd = open(WELOG_NAME, O_RDWR | O_CREAT | O_TRUNC, 0777);  
	assert(we_logfd != -1);  
	return 0;  
} 

void WeLog_2cmd(const char *fmt, ...)  
{  
	va_list ap;  
	va_start(ap, fmt);  
	vprintf(fmt, ap);  
	va_end(ap);  
}  

int WeLog_2txt(const char *msg, ...)  
{  
	char final[2048] = { 0 };    //当前时间记录  
	va_list vl_list;  
	char content[1024] = { 0 };  
	char file[128] = { 0 };
	time_t  time_write;  
	struct tm tm_Log;  	

#ifdef DEBUG
	if (we_logfd == -1)
	{
		WeLog_init();
	}
	
	va_start(vl_list, msg);  	
	vsprintf(content, msg, vl_list);     //格式化处理msg到字符串  
	va_end(vl_list);  
	
	time_write = time(NULL);         //日志存储时间  
	localtime_r(&time_write, &tm_Log);  
	strftime(final, sizeof(final), "[%Y-%m-%d %H:%M:%S] ", &tm_Log);  
  
	sprintf(file, "[file : %s line : % d] ", __FILE__, __LINE__);
	strncat(final, file, strlen(file));
	strncat(final, content, strlen(content));  
	assert(msg != NULL && we_logfd != -1);  
	assert(write(we_logfd, final, strlen(final)) == strlen(final));  
#endif // DEBUG
	return 0;  
}  

void WeLog_Deinit()  
{  
	if (we_logfd != -1)
	{
		we_logfd = -1;
		close(we_logfd); 
	}	 
}  

//int read_file(char *file_name)  
//{  
//	char *buff;  
//	FILE *fp = fopen(file_name, "r+");  
//	assert(fp);  
//	int flag = fseek(fp, 0, SEEK_END);  
//	assert(flag == 0);  
//	int len = ftell(fp);  
//	buff = (char *)malloc(sizeof(char) * (len + 1));  
//	flag = fseek(fp, 0, SEEK_SET);  
//	assert(flag == 0);  
//  
//	int num = fread(buff, 1, len + 1, fp);  
//	assert(num == len);  
//  
//	printf("len:%d, num:%d, buff:%s", len, num, buff);  
//	free(buff);  
//	buff = NULL;  
//	fclose(fp);  
//	return 0;  
//}  
//int read_file_p(char *file_name)  
//{  
//	char buff[1024] = { 0 };  
//	FILE *fp = fopen(file_name, "r+");  
//	assert(fp);  
//	int ch;  
//	int i = 0;  
//	do {  
//		ch = fgetc(fp);  
//		buff[i++] = ch;  
//	} while (ch != EOF);  
//	buff[i - 2] = '\0';  
//	fclose(fp);  
//  
//	printf("buff:%s\n", buff);  
//	return 0;  
//}  
//int main(int argc, char *argv[])  
//{  
//	assert(argc == 2);  
//	read_file(argv[1]);  
//	read_file_p(argv[1]);  
//	return 0;  
//} 

