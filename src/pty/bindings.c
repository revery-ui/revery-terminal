#include <stdio.h>
#ifdef __APPLE__
#include <util.h>
#include <sys/ioctl.h>
#elif WIN32
// TODO
#else
#include <pty.h>
#include <sys/ioctl.h>
#endif


#include <caml/alloc.h>
#include <caml/bigarray.h>
#include <caml/callback.h>
#include <caml/fail.h>
#include <caml/memory.h>
#include <caml/mlvalues.h>
#include <caml/threads.h>

CAMLprim value revery_terminal_openpty() {
	CAMLparam0();
	CAMLlocal1(ret);

	int master, slave;

#ifdef WIN32
	// TODO
#else
	struct winsize sz;
	sz.ws_col = 5;
	sz.ws_row = 5;

	openpty(&master, &slave, NULL, NULL, &sz);
#endif

	ret = caml_alloc(2, 0);
	Store_field(ret, 0, Val_int(master));
	Store_field(ret, 1, Val_int(slave));
	CAMLreturn(ret);
}

CAMLprim value revery_terminal_ioctl_set_size(value vFd, value vWidth, value vHeight) {
	CAMLparam3(vFd, vWidth, vHeight);

	int fd = Int_val(vFd);
	int rows = Int_val(vHeight);
	int cols = Int_val(vWidth);

	#ifdef WIN32
		// TODO
		int result = 0;
	#else
		struct winsize sz;
		sz.ws_col = cols;
		sz.ws_row = rows;

		int result = ioctl(fd, TIOCSWINSZ, &sz);
	#endif

	CAMLreturn(Val_int(result));
}
