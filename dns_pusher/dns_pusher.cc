#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <netdb.h>
#include <resolv.h>
#include <stdio.h>
#include <string.h>

#include <iostream>
#include <thread>
#include <vector>
#include <chrono>


#define NAME_SIZE 513
#define MSGSIZE 1024
#define MAX_ANSWER  (1024*8)

#define RR_OPT_EDNS                     0x0029
#define EDNS_CLIENT_SUBNET_OPTCODE      0x08
#define OLD_EDNS_CLIENT_SUBNET_OPTCODE  0x50fa


using namespace std::chrono;



class DNSClient {
 public:
  DNSClient(std::string host, std::string query, int port, int threads,
            uint32_t edns_ip, uint16_t edns_subnet)
      : host_(host),
        query_(query),
        port_(port),
        threads_(threads),
        edns_ip_(edns_ip),
        edns_subnet_(edns_subnet) {}
  void Start();
 private:
  void run();
  void eat();
  std::string host_;
  std::string query_;
  int port_;
  int threads_;
  int sockfd_;
  struct sockaddr_in servaddr_;
  size_t servlen_;
  uint32_t edns_ip_;
  u_char edns_subnet_;

};

void DNSClient::Start() {

  sockfd_ = socket(AF_INET,SOCK_DGRAM,0);
  memset(&servaddr_,0, sizeof(servaddr_));
  if (inet_pton(AF_INET,host_.c_str(), &(servaddr_.sin_addr)) == -1) {
    std::cerr << "Unable to decode ip " << host_ << " for slave";
    return;
  }
  servaddr_.sin_family = AF_INET;
  servaddr_.sin_port=htons(port_);
  servlen_ = sizeof(servaddr_);




  std::vector<std::thread> threads;
  auto eater = std::thread { [this]() {this->eat();}};
  for (int i = 0; i < threads_; i++) {
    threads.emplace_back(std::thread {[this]() {this->run();}});
  }
  eater.join();
  for(int i = 0; i < threads_; i++) {
    threads[i].join();
  }
}

void DNSClient::eat() {
  char buf[MSGSIZE];
  //seriously, we are just eating the return data, and doing nada with it!
  while(true) {
    int n = recvfrom(sockfd_, buf, MSGSIZE, 0, NULL, NULL);
  }
}


int InsertEDNS(u_char** buf, u_char* answer_head, uint32_t ip, u_char mask) {
  int size = 11;
  u_char* ptr = *buf;

  *ptr++ = 0; //domain
  ns_put16(RR_OPT_EDNS, ptr);
  ptr += 2;
  ns_put16(512, ptr);
  ptr += 2;
  ns_put32(0, ptr);
  ptr += 4;

  ns_put16(12, ptr);
  ptr+=2;
  ns_put16(EDNS_CLIENT_SUBNET_OPTCODE,ptr);
  ptr+=2;
  ns_put16(8,ptr);
  ptr+=2;
  ns_put16(1,ptr);
  ptr+=2;
  *ptr++=mask;
  *ptr++=0;
  u_char* dptr;
  dptr = (u_char*)&ip;
  for(int i = 0; i < 4; i++) {
    *ptr++ = *dptr++;
  }
  size += 12;

  ns_put16(ns_get16(answer_head + 10) + 1, answer_head + 10);

  return size;

}

void DNSClient::run() {
  struct sockaddr_in ip;
  char dname[NAME_SIZE];
  char ipbuf[256];
  u_char buf[MAX_ANSWER];


  bool add_edns = true;
  int ip_mod = 0;
  while(true) {
    int req_size = res_mkquery(QUERY, query_.c_str(), C_IN, T_A, NULL, 0, NULL,
                               buf, MAX_ANSWER);
    int new_size;
    if (add_edns) {
      auto end_ptr = buf+req_size;
      new_size = InsertEDNS(&end_ptr, buf, edns_ip_+ip_mod++, edns_subnet_);
      ip_mod = ip_mod % 10;
    }
    else {
      new_size = 0;
    }
    add_edns = !add_edns;
    sendto(sockfd_, buf, req_size+new_size, 0,
           (const struct sockaddr*)&servaddr_, servlen_);
    std::this_thread::sleep_for(milliseconds(1));
  }
}

int main(int argc, char**argv) {
  std::string host(argv[1]);
  int port = atoi(argv[2]);
  std::string query(argv[3]);
  int threads = atoi(argv[4]);
  std::string edns_subnet_str(argv[5]);
  u_char bits = atoi(argv[6]);


  struct sockaddr_in sa;
  char str[INET_ADDRSTRLEN];
  inet_pton(AF_INET, edns_subnet_str.c_str(), &(sa.sin_addr));
  uint32_t subnet = sa.sin_addr.s_addr;

  auto client = DNSClient(host, query, port, threads, subnet, bits);
  client.Start();
}






