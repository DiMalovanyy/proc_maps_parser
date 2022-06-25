#include <proc_maps.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define FAIL() \
	printf("\t[\033[0;31mFAIL\033[0m]: %s | Line: %d\n", __func__, __LINE__); \
	return -1

#define SUCCESS() \
	printf("\t[ \033[0;32mOK\033[0m ]: %s\n", __func__); \
	return 0 

int test_basic_creation_deletion(pid_t pid) {
	proc_maps_file_iterator_t* iter;
	if((iter = create_proc_maps_file_iterator(pid)) == NULL) {
		FAIL();
	}
	if (delete_proc_maps_file_interator(iter) != 0) {
		FAIL();
	}
	SUCCESS();
}

int test_basic_iteration(pid_t pid) {
	proc_maps_file_iterator_t* iter = create_proc_maps_file_iterator(pid);
	proc_maps_ent_t* ent;
	while ((ent = next_proc_maps(iter))) {
		//Do not do nothing	
	}
	delete_proc_maps_file_interator(iter);
	SUCCESS();
}

int test_interation_values(pid_t pid) {
	proc_maps_file_iterator_t* iter = create_proc_maps_file_iterator(pid);
	proc_maps_ent_t* ent;
	while ((ent = next_proc_maps(iter))) {
		print_proc_maps_ent(ent, stdout);
	}
	delete_proc_maps_file_interator(iter);
	SUCCESS();
}

int test_for_iterator(pid_t pid) {
	proc_maps_file_iterator_t* iter = create_proc_maps_file_iterator(pid);
	
	for(proc_maps_ent_t* ent = next_proc_maps(iter); ent;
		ent = next_proc_maps(iter)) {
		print_proc_maps_ent(ent, stdout);

	}
	delete_proc_maps_file_interator(iter);
	SUCCESS();
}

int main() {
	pid_t test_pid;

	if((test_pid = fork()) == 0) {
		char *const parmList[] = {"/bin/sleep", "inf", NULL};
		execv("/bin/sleep", parmList);
	} else {
		if (test_basic_creation_deletion(test_pid) != 0) {
			goto failure;
		}

		if (test_basic_iteration(test_pid) != 0) {
			goto failure;
		}

		if (test_interation_values(test_pid) != 0) {
			goto failure;
		}

		if (test_for_iterator(test_pid) != 0) {
			goto failure;
		}
	
		kill(test_pid, SIGKILL);
		exit(EXIT_SUCCESS);
failure:
		kill(test_pid, SIGKILL);
		exit(EXIT_FAILURE);
	}



	exit(EXIT_SUCCESS);
}




