#pragma once
#include <boost/process.hpp>
#include <dluau.h>
#include <std.hpp>
#include <Lazy_type.hpp>
namespace bp = boost::process;

constexpr const char* process_namespace{"std.process"};
struct Child_type_info {
    static consteval const char* type_namespace() {return process_namespace;}
    static consteval const char* type_name() {return "child";}
};
struct Pid_type_info {
    static consteval const char* type_namespace() {return process_namespace;}
    static consteval const char* type_name() {return "pid_t";}
};

using Child = bp::child;
using T_child = Lazy_type<Child, Child_type_info>;
using Pid = bp::pid_t;
using T_pid = Lazy_type<Pid, Pid_type_info>;
