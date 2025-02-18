#pragma once
#include <boost/process.hpp>
#include <dluau.h>
#include <LazyUserdataType.hpp>
namespace bp = boost::process;

constexpr const char* process_namespace{"gpm.process"};
struct ChildTypeInfo {
    static consteval const char* type_namespace() {return process_namespace;}
    static consteval const char* type_name() {return "Child";}
};
struct PidTypeInfo {
    static consteval const char* type_namespace() {return process_namespace;}
    static consteval const char* type_name() {return "Pid";}
};

using Child = bp::child;
using ChildType = LazyUserdataType<Child, ChildTypeInfo>;
using Pid = bp::pid_t;
using PidType = LazyUserdataType<Pid, PidTypeInfo>;
