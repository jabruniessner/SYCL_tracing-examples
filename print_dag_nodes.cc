#include "hipSYCL/sycl/tracer_utils.hpp"
#include <fstream>
#include <iostream>
#include <ostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

template <typename T> struct TD;

struct event_node {
  using hashtype = decltype(std::hash<void *>{}(nullptr));
  hashtype id = 0;
  hashtype node_group_id = 0;
  bool waited_for = false;
  std::vector<event_node *> dependecy_events{};

  void set_waited_for() {
    this->waited_for = true;

    for (auto &event : dependecy_events) {
      if (!(event->waited_for)) {
        event->set_waited_for();
      }
    }
  }
};

std::ostream &operator<<(std::ostream &os, event_node event) {
  os << "id: " << event.id << " node_group_id: " << event.node_group_id
     << " waited_for: " << event.waited_for;
  return os;
};

#ifdef __cplusplus
extern "C" {
#endif

struct event_node_hash {
  event_node::hashtype operator()(event_node event) { return event.id; }
};

struct dag {
  std::unordered_set<event_node::hashtype> queues;
  std::unordered_map<event_node::hashtype, event_node> events;
  std::unordered_set<event_node *> incompleted_events;
};

void wait_function_queue(void *usr_state, event_node::hashtype queue_id) {
  dag *dagraph = (dag *)usr_state;

  for (auto event_it = dagraph->incompleted_events.begin();
       event_it != dagraph->incompleted_events.end();) {
    if ((*event_it)->node_group_id == queue_id) {

      (*event_it)->set_waited_for();
      event_it = dagraph->incompleted_events.erase(event_it);

    } else {

      event_it++;
    }
  }
}

void wait_function_event(void *usr_state, event_node::hashtype event_id) {
  dag *dagraph = (dag *)usr_state;
  dagraph->events[event_id].set_waited_for();
}

void finalize(void *usr_state) {

  auto dag_ptr = (dag *)usr_state;
  std::ofstream outfile("outfiles.dag");
  for (auto &event : dag_ptr->events) {
    outfile << event.second << std::endl;
  }
}

void init_register() {
  void *usr_state = new dag;
  init_states(usr_state);
  init_finalize(finalize);
}

#ifdef __cplusplus
}
#endif
