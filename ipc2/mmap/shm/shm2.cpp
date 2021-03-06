/*************************************************************************
    > File Name: shm1.cpp
    > Author: Chen Tianzeng
    > Mail: 971859774@qq.com 
    > Created Time: 2019年04月17日 星期三 20时35分26秒
 ************************************************************************/

#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <error.h>
using namespace std;

typedef struct Shm
{
	int count;
}Shm;
string name1("/shm1");
string name2("/sem1");
#define FILE_MODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)
const int loop=10;
//sem_t *sem=nullptr;

int main()
{
	//shm_unlink(name1.c_str());
	//打开一个共享内存区对象
	int fd=shm_open(name1.c_str(),O_RDWR,FILE_MODE);
	if(fd==-1)
	{
		cerr<<strerror(errno)<<endl;
		exit(fd);
	}
	Shm *ptr=(Shm *)mmap(nullptr,sizeof(Shm),PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
	//ftruncate(fd,sizeof(Shm));
	//获取文件大小
	struct stat stats;
	fstat(fd,&stats);
	cout<<"file size: "<<stats.st_size<<endl;
	close(fd);
	
	sem_t *sem=sem_open(name2.c_str(),0);
	int value;
	int res=sem_getvalue(sem,&value);
	if(res==-1)
	{
		cerr<<strerror(errno)<<endl;
		exit(res);
	}
	cout<<"value: "<<value<<endl;
	if(sem==SEM_FAILED)
	{
		cerr<<strerror(errno)<<endl;
		exit(-1);
	}

	pid_t pid=getpid();
	for(int i=0;i<loop;++i)
	{
		sem_wait(sem);
		cout<<"pid: "<<pid<<" -> "<<ptr->count++<<endl;
		sem_post(sem);
	}
	/*
	 * 进程终止时，因为posix共享内存区至少随内核持续的特性，所以创建的该
	 * 对象继续存在，直到所有的它打开着的引用都关闭(当本进程终止时，该对
	 * 象不再有打开着的引用)并且该对象被现实的删除为止
	 */
	exit(0);
}
