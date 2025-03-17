#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include "../config/config.h"
#include "link.h"
#include "../database/database.h"
#include "crawler.h"
#include <iostream>
#include <filesystem>
#include <Windows.h>
#include <boost/locale.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/ssl.hpp>
#include <thread>
#include <mutex>
#include <queue>
#include <unordered_set>
#include <string>
#include <condition_variable>
#include <chrono>


std::queue<std::pair<Link, int>> urlQueue; 
std::unordered_set<std::string> visitedUrls; 
std::mutex queueMutex; 
std::mutex dbMutex;
std::mutex logMutex;
std::condition_variable queueCV; 
bool stop = false; 
Config conf = LoadConfig("../../../../config/config.ini");  
Database db(conf);


void ProcessPage(const Link& url, int depth)
{
    if (depth <= 0)
    {
        stop = true;
        return;
    }
    std::unique_lock<std::mutex> log(logMutex);
    std::cout << "Thread with id = " << std::this_thread::get_id() << " START download page: " << url.host + url.target << std::endl;
    log.unlock();
    std::string pageContent = download_page(url);
    log.lock();
    std::cout << "Thread with id = " << std::this_thread::get_id() << " END download page: " << url.host + url.target << std::endl;
    log.unlock();
    
    if (pageContent.empty())
    {
        std::cerr << std::this_thread::get_id() << " Download error! Empty page (" + LinkBuilder(url) + ")" << std::endl;
        return;
    }

    std::chrono::duration<double> calcTimedb;
    {
        std::unique_lock<std::mutex> lock_db(dbMutex);
        log.lock();
        std::cout << " Thread with id = " << std::this_thread::get_id() << " START push to DB: " << url.host + url.target << std::endl;
        log.unlock();
        auto timeStartdb = std::chrono::steady_clock::now();
        db.InsertRow(LinkBuilder(url), index_words(CleanHTML(pageContent)));
        auto timeEnddb = std::chrono::steady_clock::now();
        calcTimedb = timeEnddb - timeStartdb;
        lock_db.unlock();
    }
    log.lock();
    std::cout << " Thread with id = " << std::this_thread::get_id() << " END (" << calcTimedb.count() << ") push to DB : " << url.host + url.target << std::endl;
    log.unlock();

    if (depth > 1)
    {
        std::vector<Link> newUrls = ExtractLinks(pageContent, url);

        std::lock_guard<std::mutex> lock(queueMutex);
        for (const auto& el : newUrls)
        {
            if (visitedUrls.find(LinkBuilder(el)) == visitedUrls.end())
            {
                visitedUrls.insert(LinkBuilder(el));
                urlQueue.push({ el, depth - 1 });
            }
        }
    }
    else stop = true;

    queueCV.notify_all();
    log.lock();
    std::cout << "Thread id = " << std::this_thread::get_id() << " DONE with: " << url.host + url.target << std::endl;
    log.unlock();
}

void worker()
{
    std::unique_lock<std::mutex> log(logMutex);
    log.unlock();
    std::unique_lock<std::mutex> lock(queueMutex);
    std::cout << "thread id: " << std::this_thread::get_id() << " start" << std::endl;
    lock.unlock();
    while (true)
    {
        std::pair<Link, int> task;

        {
            log.lock();
            std::cout << "ID = " << std::this_thread::get_id() << " prepare to wait. urlQueue.empty = " << urlQueue.empty() << " | stop = " << stop << std::endl;
            log.unlock();

            lock.lock();
            queueCV.wait(lock, [] { return !urlQueue.empty() || stop; });

            log.lock();
            std::cout << "ID = " << std::this_thread::get_id() << " after wait" << std::endl;
            log.unlock();

            if (stop && urlQueue.empty())
            {
                std::cout << "thread id: " << std::this_thread::get_id() << " end" << std::endl;
                lock.unlock();
                return;
            }
            task = urlQueue.front();
            urlQueue.pop();
            lock.unlock();
        }
        log.lock();
        std::cout << "ID = " << std::this_thread::get_id() << " run PROCESS" << std::endl;
        log.unlock();
        ProcessPage(task.first, task.second);
    }
}

void StartCrawler(const Link& start_url, int maxDepth, int numThreads)
{
    auto timeStart = std::chrono::steady_clock::now();

    visitedUrls.insert(LinkBuilder(start_url));
    urlQueue.push({ start_url, maxDepth });
    db.createTables();

    std::vector<std::thread> threadPool;
    for (int i = 0; i < numThreads; ++i)
    {
        threadPool.emplace_back(worker);
    }

    for (auto& t : threadPool)
    {
        auto tv = t.get_id();
        t.join();
        std::cout << tv << "join" << std::endl;
    }

    auto timeEnd = std::chrono::steady_clock::now();
    std::chrono::duration<double> calcTime = timeEnd - timeStart;
    std::cout << "All time is " << calcTime.count() << " sec" << std::endl;
}


int main() {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);    

    try {        
        int numThreads = std::thread::hardware_concurrency();
        int maxDepth = conf.recursion_depth;
        
        StartCrawler(SplitLink(conf.start_url), maxDepth, numThreads);
    }
    catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
    }
}
