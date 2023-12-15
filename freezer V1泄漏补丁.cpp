#include <fstream>
#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <unistd.h>
#include <sys/stat.h>
// 调用shell命令
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

// freezerV1 泄漏补丁
void freezeit(){
    std::string _fv1path = exec("mount -t cgroup | awk '/freezer/ {print $3}'");
    _fv1path.pop_back();  

    std::string mempressure = exec("awk '/some/ {print $2}' /proc/pressure/memory");
    mempressure = mempressure.substr(6); 
    mempressure = mempressure.substr(0, mempressure.size()-4);  
    int _memp = std::stoi(mempressure);

    if (_memp > 18) {
        std::ofstream frozen_file(_fv1path + "/frozen/freezer.state");
        if (frozen_file.is_open()) {
            frozen_file << "THAWED";
            frozen_file.close();
        } else {
            std::cerr << "无法打开文件" << _fv1path + "/frozen/freezer.state" << std::endl;
        }

        sleep(10);

        std::ofstream frozen_file2(_fv1path + "/frozen/freezer.state");
        if (frozen_file2.is_open()) {
            frozen_file2 << "FROZEN";
            frozen_file2.close();
        } else {
            std::cerr << "无法打开文件" << _fv1path + "/frozen/freezer.state" << std::endl;
        }
    }
}
void freezer(){
while(true){
// 3秒调用一次freezer补丁
freezeit();
sleep(3);
}
}
// 错误判断
void error_log(){ 
    std::ofstream outfile; 
    outfile.open("/sdcard/Android/freezer_error_log.txt"); 
 if (outfile.is_open()) {
        outfile << "freezer V1 内存泄漏补丁启动失败";  
        outfile.close();  
    } else {
        std::cout << "无法打开文件";
    }
}
// 补丁初始化
void initialization(){
    std::string _fv1path;
    FILE* pipe = popen("mount -t cgroup | awk '/freezer/ {print $3}'", "r");
    if (!pipe) {
        std::cerr << "无法启动" << std::endl;
        error_log();
    }
    char buffer[128];
    while (fgets(buffer, sizeof buffer, pipe) != NULL) {
        _fv1path = buffer;
    }
    pclose(pipe);

    std::ofstream frozen_file(_fv1path + "/frozen/freezer.killable");
    if (frozen_file.is_open()) {
        frozen_file << "1";
        frozen_file.close();
    } else {
        std::cerr << "无法打开文件: " << _fv1path + "/frozen/freezer.killable" << std::endl;
    }

    std::ofstream unfrozen_file(_fv1path + "/unfrozen/freezer.killable");
    if (unfrozen_file.is_open()) {
        unfrozen_file << "1";
        unfrozen_file.close();
    } else {
        std::cerr << "无法打开文件: " << _fv1path + "/unfrozen/freezer.killable" << std::endl;
    }

}
// 模块自检
void files_path(){
            struct stat info;

    if( stat( "/data/MoWei", &info ) != 0 ) {
        std::cout << "无法访问文件夹" <<std::endl;
    } else if( info.st_mode & S_IFDIR ) {
        std::cout << "正常:开始下一步操作" <<std::endl;
        initialization();
        freezer();
    } else {
        std::cout << "警告:非法使用此模块" <<std::endl;
        std::cout << "开始休眠所有关于A0的进程" <<std::endl;
        sleep(60 * 60 * 24 * 365);
    }
}

int main(){
    // 模块自检
    files_path();
    // 休眠五秒
    sleep(5);
}