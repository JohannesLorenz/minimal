#include <lo/lo.h>
#include "ports.h"

namespace mmms {
namespace ports {

lo_server server;

int on_receive(const char *path, const char *, lo_arg **, int, lo_message msg, void *)
{
#if 0
	static char buffer[1024*20];
	memset(buffer, 0, sizeof(buffer));
	size_t size = sizeof(buffer);
	lo_message_serialise(msg, path, buffer, &size);
	if(!strcmp("/paths", buffer)) // /paths:sbsbsbsbsb...
	update_paths(buffer, NULL);
	else if(!strcmp("/exit", buffer))
	die_nicely(buffer, NULL);
	else if(status_url == path)
	update_status_info(buffer);
	else if(!strcmp("undo_change", buffer))
	;//ignore undo messages
	else
	display(buffer, NULL);

	return 0;
#else
	(void)path;
	(void)msg;
	return 0;
#endif
}

void error_cb(int i, const char *m, const char *loc)
{
#if 0
	wprintw(log, "liblo :-( %d-%s@%s\n",i,m,loc);
#else
	(void)i;
	(void)m;
	(void)loc;
#endif
}

void init()
{
	//setup liblo - it can choose its own port
	server = lo_server_new_with_proto(NULL, LO_UDP, error_cb);
	lo_server_add_method(server, NULL, NULL, on_receive, NULL);
	//lo_addr = lo_address_new_with_proto(LO_UDP, NULL, "8080");
//	wprintw(log, "lo server running on %d\n", lo_server_get_port(server));
}

void handle_events()
{
	lo_server_recv_noblock(server, 0);
}

}}

