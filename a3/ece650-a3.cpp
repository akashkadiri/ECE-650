#include <iostream>
#include <unistd.h>
#include <vector>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <pthread.h>
#include <sys/wait.h> 
#include <string.h>


using namespace std;
struct ip_args{
    int s,n,l,c;
};

char* intToStr(int data) {
    std::string strData = std::to_string(data);

    char* temp = new char[strData.length() + 1];
    strcpy(temp, strData.c_str());

   return temp;
}

int parse_cmd_line(int argc, char** argv, ip_args *ip){
    int val;
    int i=1;
    //initialize the value
    ip->s = 10;
    ip->n = 5;
    ip->l = 5;
    ip->c = 20;
    while(i<argc){
            if(strcmp(argv[i],"-s")==0){
                i++;
                val = atoi(argv[i]); 
                if (val >= 2){
                    ip->s = val;
                }
                else{return -1;}//Invalid argv[i] for an argument
            }
            else if(strcmp(argv[i],"-n")==0){
                i++;
                val = atoi(argv[i]);
                if (val>=1){
                    ip->n = val;
                    }
                else{return -1;}
            }
            else if(strcmp(argv[i],"-l")==0){
                i++;
                val = atoi(argv[i]);
                if (val>=5){
                    ip->l = val;
                    }
                else{return -1;}
            }
            else if(strcmp(argv[i],"-c")==0){
                i++;
                val = atoi(argv[i]);
                if (val>=1){
                    ip->c = val;
                    }
                else{return -1;}
            }
            else{return -2;} //Invalid argument
        i++;
        }
    return 0;
}
//for closing processes use thread

int rgen_prog(ip_args ip){
    char *s,*n,*l,*c;
    string s1,n1,l1,c1;
    s = intToStr(ip.s);
    n = intToStr(ip.n);
    l = intToStr(ip.l);
    c = intToStr(ip.c);
    char* args[10];
    args[0] = (char *)"./rgen";
    args[1] = (char *)"-s";
    args[2] = s;
    args[3] = (char *)"-n";
    args[4] = n;
    args[5] = (char *)"-l";
    args[6] = l;
    args[7] = (char *)"-c";
    args[8] = c;
    args[9] = nullptr;
    execv("./rgen", args);
    return 0;
}

int graph_gen(){
    execlp("python3", "python3", "ece650-a1.py", (char*) NULL);
    return 0;
}

int graph_out(){
    char *args[2];
    args[0] = (char *)"./ece650-a2";
    args[1] = nullptr;
    execv("./ece650-a2",args);
    return 0;
}

int close_all(pid_t rgen_pid, vector<pid_t> child){
    int status;
    kill(rgen_pid, SIGTERM);
    waitpid(rgen_pid,&status,0);
    for (pid_t k : child) {
    int status1;
    kill(k, SIGTERM);
    waitpid(k, &status1, 0);
    }
}

void *pass_input(void *);

//0 for reading
//1 for writing
int main (int argc, char *argv[]) {

    int RG_to_A1[2],A1_to_A2[2],in_to_A2[2];
    int status, ret;
    vector<pid_t> child; 
    pid_t fork_p, rgen_pid;
    pthread_t ip_thread;
    ip_args ip;
    ret = parse_cmd_line(argc, argv, &ip);
    if (ret == -1){
        cerr<<"Error: Invalid command line value"<<endl;
        return -1;
    }
    if (ret == -2){
        cerr<<"Error: unknown command line argument"<<endl;
        return -1;
    }

    pipe(RG_to_A1);
    fork_p = fork();
    rgen_pid = fork_p;
    if(fork_p==0){
        dup2(RG_to_A1[1],STDOUT_FILENO);
        close(RG_to_A1[0]);
        close(RG_to_A1[1]);
        return rgen_prog(ip);
    }
    if(fork_p<0){
        std::cerr<<"Error: Fork Failed"<<endl;
        close_all(rgen_pid,child);
        return -1;
    }
    pipe(A1_to_A2);
    fork_p = fork();
    child.push_back(fork_p);
    if(fork_p == 0){
        dup2(RG_to_A1[0],STDIN_FILENO);
        close(RG_to_A1[0]);
        close(RG_to_A1[1]);
        dup2(A1_to_A2[1],STDOUT_FILENO);
        close(A1_to_A2[0]);
        close(A1_to_A2[1]);
        return graph_gen();
    }
    if(fork_p<0){
        std::cerr<<"Error: Fork failed"<<endl;
        close_all(rgen_pid,child);
        return -1;
    }
    pipe(in_to_A2);
    fork_p = fork();
    if(fork_p==0){
        dup2(A1_to_A2[0],STDIN_FILENO);
        close(A1_to_A2[0]);
        close(A1_to_A2[1]); 
        graph_out();
    }
    if(fork_p<0){
        std::cerr<<"Error: Fork failed"<<endl;
        close_all(rgen_pid,child);
        return -1;
    }
    dup2(A1_to_A2[1],STDOUT_FILENO);
    close(A1_to_A2[0]);
    close(A1_to_A2[1]);
    ret = pthread_create(&ip_thread,nullptr,&pass_input,&rgen_pid);
    if(ret){
        std::cerr<<"Error: Failed to create thread"<<endl;
        close_all(rgen_pid,child);
        return -1;
    }
    waitpid(rgen_pid,&status,0);
    for (pid_t k : child) {
    int status1;
    kill(k, SIGTERM);
    waitpid(k, &status1, 0);
    }
    return 0;
}

void *pass_input(void *data){
    pid_t *rgen_pid = static_cast<pid_t*>(data);
    int status;
    while (!std::cin.eof()) {
    std::string line;
    std::getline(std::cin, line);
    std::cout<<line<<"\n";
    }
    kill(*rgen_pid,SIGTERM);
    waitpid(*rgen_pid,&status,0);
    return nullptr;
} 