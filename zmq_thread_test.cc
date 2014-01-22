#include <sys/types.h>
#include <time.h>
#include <zmq.hpp>



#include <map>
#include <thread>
#include <mutex>
#include <iostream>


std::mutex wait_lock;
bool do_continue = true;
zmq::context_t inproc_context(1);


void eat_thread() {
  zmq::socket_t inproc_socket(inproc_context, ZMQ_PAIR);
  zmq::pollitem_t poll_items[512];

  try {
    inproc_socket.bind("inproc://a");
  } catch(zmq::error_t e) {
    std::cout << "Error on inproc bindq : " << e.what() << "\n";
    return;
  }
  int major, minor, patch;
  zmq::version (&major, &minor, &patch);
  std::cout << "Current 0MQ version is " << major << "." << minor << "."
            << patch << "\n";

  poll_items[0] = {inproc_socket, 0, ZMQ_POLLIN, 0};
  wait_lock.unlock();

  while (do_continue) {
    uint32_t current_count = 1;
    std::cout << "we are going to test out our subscriptions!"
              << " : " << current_count << "\n";
    int rc;
    try {
      if(!(rc=zmq::poll(&poll_items[0], current_count,-1)))  {
        continue;
      }
    }
    catch(zmq::error_t& e) {
      //XXX what to do here? need to be smarter
      if (e.num() == ETERM) {
        return;
      }
      std::cout << "Error received in zmq_poll: " << e.what()
                 << " (" << e.num() << ")\n";
      continue;
    }
    if (poll_items[0].revents & ZMQ_POLLIN) {
      zmq::message_t type;
      inproc_socket.recv(&type);

      std::string cmd_type(static_cast<char*>(type.data()), type.size());
      std::cout << "Received: " << type.size() << "\n";

      if (cmd_type != "add" && cmd_type != "") {
        std::cout <<  "recieved: '" << static_cast<char*>(type.data())
                  << " AKA: '" << cmd_type << "'\n";
      }
      zmq::message_t resp(2);
      memcpy(resp.data(), "OK", 2);
      inproc_socket.send(resp);
    }
  }
}


void feeder() {
  zmq::socket_t inproc_socket(inproc_context, ZMQ_PAIR);
  try {
    inproc_socket.connect("inproc://a");
  } catch(zmq::error_t e) {
    std::cout << "Error on zmq inproc connect: " << e.what() << "\n";
    return;
  }
  std::string command("add");
  std::string publisher_host("localhost:9988");
  for(int i = 0; i < 10; i++) {
    if (i%2 == 0) {
      std::string filter_name("");
      std::cout << filter_name.size() << "\n";
      zmq::message_t cmd(filter_name.size());
      memcpy(cmd.data(), filter_name.data(), filter_name.size());
      inproc_socket.send(cmd);
    } else {
      std::string filter_name("add");
      std::cout << filter_name.size() << "\n";
      zmq::message_t cmd(filter_name.size());
      memcpy(cmd.data(), filter_name.data(), filter_name.size());
      inproc_socket.send(cmd);
    }
    zmq::message_t resp;
    inproc_socket.recv(&resp);

    std::string response(static_cast<char*>(resp.data()), resp.size());
    std::cout << "Got response: " << response << "\n";
  }
}

int main(int argc, char** argv)
{
  wait_lock.lock();
  std::thread t(eat_thread);
  t.detach();
  wait_lock.lock();
  std::thread t1(feeder);
  t1.join();
  std::cout << "between two threads\n";
  std::thread t2(feeder);
  t2.join();
  do_continue = false;
  sleep(1);
}




