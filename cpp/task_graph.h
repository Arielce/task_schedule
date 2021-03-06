#ifndef TASK_SCHEDULE_TASK_GRAPH_H
#define TASK_SCHEDULE_TASK_GRAPH_H

#include <map>
#include <set>
#include <string>
#include <vector>
#include <utility>

// 给调度系统用的任务信息
struct TaskInfo {
    TaskInfo(const std::string& taskName, const std::string& shellCmd, int maxRetry) :
            taskName(taskName), shellCmd(shellCmd), maxRetry(maxRetry) {
    }
    std::string taskName; // 任务名称
    std::string shellCmd; // Shell命令
    int maxRetry;   // 最大重试次数
};

class TaskGraph {
public:
    ~TaskGraph();

    // 添加任务, 声明依赖, 重试次数(默认不重试)
    bool addTask(const std::string& taskName, const std::vector<std::string> &deps, const std::string& shellCmd, int maxRetry = 0);

    // 初始化拓扑
    bool initGraph();

    // 获取所有待执行任务
    void getTodoTasks(std::vector<TaskInfo*> *todo);

    // 标识任务完成
    bool markTaskDone(const std::string& taskName);

    // 获取任务信息
    TaskInfo* getTaskInfo(const std::string& taskName);

    // 打印所有任务信息
    void printGraph(std::ostream& ostream);

private:
    // 任务节点, 随着依赖的任务完成, 剩余依赖任务逐渐减少为0; 此时依赖已全部完成, 当前任务进入待执行状态.
    struct TaskNode {
        TaskNode(const std::string& taskName, const std::vector<std::string>& deps, const std::string& shellCmd, int maxRetry) :
                info(taskName, shellCmd, maxRetry), outCounter(0), inCounter(0), done(false) {
            for (std::vector<std::string>::const_iterator iter = deps.begin(); iter != deps.end(); ++iter) {
                std::pair<std::map<std::string, bool>::iterator, bool> ret = outEdge.insert(make_pair(*iter, false));
                if (ret.second) {
                    ++outCounter;
                }
            }
        }

        TaskInfo info; // 调度系统需要的任务信息

        std::map<std::string, bool> outEdge; // 本任务依赖的其他任务、完成状况
        std::map<std::string, bool> inEdge; // 依赖了本任务的其他任务、完成状况
        int outCounter; // 剩余依赖任务
        int inCounter; // 剩余被依赖任务
        bool done; // 任务是否完成
    };

    // 在图数据结构中维护所有任务
    typedef std::map<std::string, TaskNode*> Graph;
    Graph graph_;

    // 依赖完全, 可以立即执行的任务
    std::set<std::string> todo_;
};


#endif //TASK_SCHEDULE_TASK_GRAPH_H
