# dns_pusher

## what?

A really basic dns load testing tool, which is capabable of sendin EDNS Client Subnet
enabled dns requests. It has a threaded source, and a thread that just eats up responses,
not even checking them

## how?
`dns_pusher <server_ip>  <server_port> <query_cname> <client_threads> <edns_client_subnet>
 <edns_client_subnet_bits>`
example:
`dns_pusher 8.8.8.8 53 www.foo.com 4 203.116.18.0 24`

