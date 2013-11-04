#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include <ail.h>


static void _print_help(const char *cmd)
{
	fprintf(stderr, "Usage:\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "[Add a desktop]\n");
	fprintf(stderr, "       %s add <PACKAGE NAME>\n", cmd);
	fprintf(stderr, "\n");
	fprintf(stderr, "       Ex) %s add com.samsung.menu-screen\n", cmd);
	fprintf(stderr, "\n");
	fprintf(stderr, "[Update a desktop]\n");
	fprintf(stderr, "       %s update <PACKAGE NAME>\n", cmd);
	fprintf(stderr, "\n");
	fprintf(stderr, "       Ex) %s update com.samsung.menu-screen\n", cmd);
	fprintf(stderr, "\n");
	fprintf(stderr, "[Remove a desktop]\n");
	fprintf(stderr, "       %s remove <PACKAGE NAME>\n", cmd);
	fprintf(stderr, "\n");
	fprintf(stderr, "       Ex) %s remove com.samsung.menu-screen\n", cmd);
	fprintf(stderr, "\n");
}



static ail_error_e _add_desktop(const char *package)
{
	ail_error_e ret;

	if (!package) {
		return AIL_ERROR_FAIL;
	}

	ret = ail_desktop_add(package);
	if (ret != AIL_ERROR_OK) {
		return AIL_ERROR_FAIL;
	}

	return AIL_ERROR_OK;
}



static ail_error_e _update_desktop(const char *package)
{
	ail_error_e ret;

	if (!package) {
		return AIL_ERROR_FAIL;
	}

	ret = ail_desktop_update(package);
	if (ret != AIL_ERROR_OK) {
		return AIL_ERROR_FAIL;
	}

	return AIL_ERROR_OK;
}



static ail_error_e _remove_desktop(const char *package)
{
	ail_error_e ret;

	if (!package) {
		return AIL_ERROR_FAIL;
	}

	ret = ail_desktop_remove(package);
	if (ret != AIL_ERROR_OK) {
		return AIL_ERROR_FAIL;
	}

	return AIL_ERROR_OK;
}



int main(int argc, char** argv)
{
	ail_error_e ret = AIL_ERROR_OK;

	if (3 == argc) {
		if (!strncmp(argv[1], "add", 3)) {
			ret = _add_desktop(argv[2]);
		} else if (!strncmp(argv[1], "update", 6)) {
			ret = _update_desktop(argv[2]);
		} else if (!strncmp(argv[1], "remove", 6)) {
			ret = _remove_desktop(argv[2]);
		} else {
			fprintf(stderr, "%s is a invalid command\n", argv[1]);
		}
	}
	else {
		_print_help(argv[0]);
		return EXIT_FAILURE;
	}

	if (ret != AIL_ERROR_OK) {
		fprintf(stderr, "There are some problems\n");
	}

	return EXIT_SUCCESS;
}

