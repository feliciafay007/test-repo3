#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>
#include <map>
#include <iomanip>

using namespace std;
//I add one line
// 指令数据
struct ConsumeLog {
    // 用户查询关键字
    string user_key;
    // 广告关键字
    string ad_key;
    // 广告价格
    int price;
    // 反作弊标记
    bool cheat;
};

// 指令操作类
class Consume {
public:
    Consume();
    ~Consume();
    
    // 重置日志记录
    void ResetLog();
    
    // 解析日志字符串
    // 如果日志解析失败，返回false
    // 如果日志解析成功，返回true，并设置解析成功的指令数据ConsumeLog
    bool ParseLog(const string &log, ConsumeLog *consume_log);
    
    // 记录日志
    void RecordLog(const ConsumeLog &consume_log);
    
    // 获得有效计费最高的用户查询词
    string GetMaxUserKey() { return max_user_key_; }
    // 获得有效计费最高的用户查询词对应的计费金额
    int GetMaxPrice() { return max_price_; }
    
private:
    // 记录用户查询对应的广告价格
    void RecordUserKey(const string &user_key, int price);
    
private:
    // 记录每个用户查询关键词命中的总的价格
    map<string, int> logs_price_;
    // 获得有效计费最高的用户查询词对应的计费金额
    int max_price_;
    // 有效计费最高的用户查询词
    string max_user_key_;
};

Consume::Consume() : max_price_(0)
{
}

Consume::~Consume()
{
    logs_price_.clear();
}

// 重置日志记录
void Consume::ResetLog()
{
    max_price_ = 0;
    max_user_key_ = "";
    logs_price_.clear();
}

// 解析日志字符串
// 如果日志解析失败，返回false
// 如果日志解析成功，返回true，并设置解析成功的指令数据ConsumeLog
bool Consume::ParseLog(const string &log, ConsumeLog *consume_log)
{
    ConsumeLog temp;
    istringstream iss(log);
    string none;
    bool success = iss >> temp.user_key >> temp.ad_key >> temp.price >> temp.cheat;
    if(!success)
        return false;
    // 检查是否有多余四个tab分隔符
    bool fail = iss >> none;
    if(fail)
        return false;
    if(consume_log != NULL) {
        *consume_log = temp;
    }
    return true;
}

// 记录用户查询对应的广告价格
void Consume::RecordUserKey(const string &user_key, int price)
{
    // 计算累积次数与总价格
    int current_price = logs_price_[user_key] + price;
    // 存储
    logs_price_[user_key] = current_price;
    // 重新判决有效计费最高的用户查询词
    if(max_price_ < current_price) {
        max_price_ = current_price;
        max_user_key_ = user_key;
    }
}

// 记录日志
void Consume::RecordLog(const ConsumeLog &consume_log)
{
    string user_key = consume_log.user_key;
    transform(user_key.begin(), user_key.end(), user_key.begin(), ::tolower);
    // 首尾空格的处理的处理
    user_key.erase(0,user_key.find_first_not_of(" "));
    user_key.erase(user_key.find_last_not_of(" ") + 1);
    RecordUserKey(user_key, consume_log.price);
}

int main()
{
    // 将文件consume.in重定向为标准输入
    if(freopen("consume.in", "r", stdin) == NULL) {
        cerr << "error:  consume.in" << endl;
        return 1;
    }
    // 将文件consume.out重定向为标准输出
    if(freopen("consume.out", "w", stdout) == NULL) {
        cerr << "error:  consume.out" << endl;
        return 1;
    }
    
    // 初始化
    Consume consume;
    consume.ResetLog();
    
    // 读取指令并处理
    string log;
    ConsumeLog consume_log;
    while(getline(cin, log) != NULL) {
        // 解析指令字符串
        if(!consume.ParseLog(log, &consume_log)) {
            cout << "SORRY" << endl;
            return 1;
        }
        if(consume_log.cheat)
            continue;
        consume.RecordLog(consume_log);
        
        // 获得有效计费最高的用户查询词
        string max_user_key = consume.GetMaxUserKey();
        // 获得有效计费最高的用户查询词对应的计费金额
        double max_price = consume.GetMaxPrice() / 100.0;
        cout << max_user_key << "\t";
        cout << fixed << setfill('0') << setprecision(2) << max_price << endl;
        
        return 0;
    }

}
