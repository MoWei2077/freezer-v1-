#include <fstream>
#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <unistd.h>
#include <sys/stat.h>

std::string exec(const char* cmd) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}
// 补丁初始化
void Freezer_V1_patch(){
    std::string _fv1path;
    FILE* pipe = popen("mount -t cgroup | awk '/freezer/ {print $3}'", "r");
    if (!pipe) {
        std::cerr << "无法启动" << std::endl;
    }
    char buffer[128];
    while (fgets(buffer, sizeof buffer, pipe) != NULL) {
        _fv1path = buffer;
    }
    pclose(pipe);
 // 判断路径是否存在
 if (isPathExists(_fv1path)) {
     std::cout << "Freezer-V1已挂载" << std::endl;
 } else {
     std::cout << "Freezer-V1未挂载" << std::endl;
    system("echo Freezer-V1未挂载 > /sdcard/Andoird/Freezer V1内存泄漏补丁日志.txt");
 }
 
std::string frozen_file0 = "echo \"1\" > " + _fv1path + "/frozen/freezer.killable";
if (system(frozen_file0.c_str()) == 0) {
    std::cout << "freezer V1-内存泄漏补丁成功启动" << std::endl;
    system("echo Freezer V1-内存泄漏补丁启动成功 > /sdcard/Andoird/Freezer V1内存泄漏补丁日志.txt");
} else {
    std::cerr << "Freezer V1-内存泄漏补丁启动失败" << std::endl;
    system("echo Freezer V1-内存泄漏补丁启动失败 > /sdcard/Andoird/Freezer V1内存泄漏补丁日志.txt");
}

std::string unfrozen_file0 = "echo \"1\" > " + _fv1path + "/unfrozen/freezer.killable";
if (system(unfrozen_file0.c_str()) == 0) {
    std::cout << "freezer V1-内存泄漏补丁成功启动" << std::endl;
    system("echo Freezer V1-内存泄漏补丁启动成功 > /sdcard/Andoird/Freezer V1内存泄漏补丁日志.txt");
} else {
    std::cerr << "Freezer V1-内存泄漏补丁启动失败" << std::endl;
    system("echo Freezer V1-内存泄漏补丁启动失败 > /sdcard/Andoird/Freezer V1内存泄漏补丁日志.txt");
}
}
int main(){
    pid_t pid = fork();

    if (pid == -1) {
        std::cerr << "Fork子进程失败!" << std::endl;
        return 1;
    } else if (pid == 0) {
        // 子进程
        Freezer_V1_patch();
    } else {
        // 父进程
        std::cout << "这是子进程的PID: " << pid << std::endl;
    }

    return 0;
}

