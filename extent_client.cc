// RPC stubs for clients to talk to extent_server

#include "extent_client.h"
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

extent_client::extent_client() {
    es = new extent_server();
}

extent_protocol::status
extent_client::create(uint32_t type, extent_protocol::extentid_t &id) {
    extent_protocol::status ret = extent_protocol::OK;
    ret = es->create(type, id);
    return ret;
}

extent_protocol::status
extent_client::get(extent_protocol::extentid_t eid, std::string &buf) {
    extent_protocol::status ret = extent_protocol::OK;
    ret = es->get(eid, buf);
    return ret;
}

extent_protocol::status
extent_client::getattr(extent_protocol::extentid_t eid,
                       extent_protocol::attr &attr) {
    extent_protocol::status ret = extent_protocol::OK;
    ret = es->getattr(eid, attr);
    return ret;
}

extent_protocol::status
extent_client::put(extent_protocol::extentid_t eid, std::string buf) {
    extent_protocol::status ret = extent_protocol::OK;
    int r;
    ret = es->put(eid, buf, r);
    return ret;
}

extent_protocol::status
extent_client::remove(extent_protocol::extentid_t eid) {
    extent_protocol::status ret = extent_protocol::OK;
    int r;
    ret = es->remove(eid, r);
    return ret;
}

Directory extent_client::get_dir(extent_protocol::extentid_t eid) {
    return es->get_dir(eid);
}

extent_protocol::status
extent_client::lookup(extent_protocol::extentid_t parent, const std::string &filename, bool &found, uint32_t &inum) {
    return es->lookup(parent, filename, found, inum);
}

extent_protocol::status
extent_client::create_file(extent_protocol::extentid_t parent, const std::string &filename, uint32_t type,
                           uint32_t &new_inum) {
    return es->create_file(parent, filename, type, new_inum);
}

extent_protocol::status extent_client::unlink(extent_protocol::extentid_t parent, const std::string &link_name) {
    return es->unlink(parent, link_name);
}

extent_protocol::status extent_client::readdir(extent_protocol::extentid_t id, std::list<extent_dirent> &entries) {
    return es->readdir(id, entries);
}


