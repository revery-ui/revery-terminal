#include <stdio.h>
#include <util.h>
#include <sys/ioctl.h>

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

	struct winsize sz;
	sz.ws_col = 5;
	sz.ws_row = 5;

	int master, slave;
	openpty(&master, &slave, NULL, NULL, &sz);

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

	struct winsize sz;
	sz.ws_col = cols;
	sz.ws_row = rows;

	int result = ioctl(fd, TIOCSWINSZ, &sz);

	CAMLreturn(Val_int(result));
}
