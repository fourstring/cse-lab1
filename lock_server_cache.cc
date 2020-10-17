// the caching lock server implementation

#include "lock_server_cache.h"
#include <sstream>
#include <utility>
#include <cstdio>
#include <unistd.h>
#include <arpa/inet.h>
#include "lang/verify.h"
#include "handle.h"
#include "tprintf.h"


lock_server_cache::lock_server_cache() = default;


int lock_server_cache::acquire(lock_protocol::lockid_t lid, std::string &id, int &result) {
    lock_protocol::status ret = lock_protocol::OK;
    // Your lab2 part3 code goes here
    unique_t u{access_lock_mutex};
    auto istate = lock_store.find(lid);
    if (istate == lock_store.end()) {
        lock_store.insert({lid, {RLockStatus::Granted, id}});
        result = lock_protocol::OK;
    } else {
        auto &state = istate->second;
        if (state.status == RLockStatus::Free) {
            state.status = RLockStatus::Granted;
            result = lock_protocol::OK;
        } else {
            revoke_queue.enqueue({lid, id});
            result = lock_protocol::RETRY;
        }
    }
    return ret;
}

int
lock_server_cache::release(lock_protocol::lockid_t lid, std::string &id, int &result) {
    lock_protocol::status ret = lock_protocol::OK;
    // Your lab2 part3 code goes here
    return ret;
}

lock_protocol::status
lock_server_cache::stat(lock_protocol::lockid_t lid, int &r) {
    tprintf("stat request\n");
    r = nacquire;
    return lock_protocol::OK;
}

void lock_server_cache::revoker() {
    while (!_stop) {
        RevokeTask task{revoke_queue.dequeue()};
        access_lock_mutex.lock();
        auto &state = lock_store.find(task.lid)->second;
        access_lock_mutex.unlock();
        handle h{state.owned_handle};
        auto *cl = h.safebind();
        if (cl) {
            int void_r;
            cl->call(rlock_protocol::revoke, task.lid, void_r);
            handle req_h{task.requester_handle};
            auto *req_cl = req_h.safebind();
            if (req_cl) {
                req_cl->call(rlock_protocol::retry, task.lid, void_r);
                access_lock_mutex.lock();
                state.owned_handle = task.requester_handle;
                access_lock_mutex.unlock();
            }
        }
    }
}

RevokeTask::RevokeTask(lock_protocol::lockid_t lid, std::string requesterHandle) : lid(lid), requester_handle(std::move(
        requesterHandle)) {

}

RevokeTask::RevokeTask(RevokeTask &&rhs) noexcept: lid{rhs.lid},
                                                   requester_handle{std::move(rhs.requester_handle)} {

}

RevokeTask::RevokeTask(RevokeTask &rhs) = default;