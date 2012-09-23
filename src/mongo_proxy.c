/*
 * file   : test_ptrace.c
 * author : ning
 * date   : 2012-01-13 16:10:27
 */

#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "log.h"
#include "cfg.h"

#include "mongo_server.h"

#define __VERSION__ "0.0.1"

//globals
//
mongo_replica_set_t * replica_set;
mongo_conn_t * free_conn = NULL;


int onexit(){

}


void sig_handler(int signum)
{
    fprintf(stderr, "is going to exit!");
    onexit();
    exit(0);
}

void init_sig_handler()
{
    if (signal(SIGINT, sig_handler) == SIG_IGN)
        signal(SIGINT, SIG_IGN);
    if (signal(SIGHUP, sig_handler) == SIG_IGN)
        signal(SIGHUP, SIG_IGN);
    if (signal(SIGTERM, sig_handler) == SIG_IGN)
        signal(SIGTERM, SIG_IGN);
}


int usage()
{
    printf("mongoproxy -c path_to_config, default: conf/mongoproxy.cfg ");
    printf("mongoproxy -v : show version");
}

int init(int argc, char **argv)
{
    int ch;
    int rundaemon = 1; 
    int logundefined = 1;
    char cfgfile[999] = "conf/mongoproxy.cfg"; //default cfg file

    init_sig_handler();

    while ((ch = getopt(argc, argv, "vduc:h?")) != -1) {
        switch (ch) {
        case 'd':                   //run in frontend, default
            rundaemon = 0;
            break;
        case 'c':
            strncpy(cfgfile, optarg, sizeof(cfgfile));
            break;
        case 'u':
            logundefined = 1;
            break;
        case 'v':
            printf("version: %s\n", __VERSION__);
            exit(0);
        default:
            usage();
            exit(0);
        }
    }
    fprintf(stderr, "use %s as config file: ", cfgfile);

    if (cfg_load(cfgfile, logundefined) == 0) {
        fprintf(stderr, "can't load config file: %s - using defaults\n",
                cfgfile);
    }
    cfg_add("CFG_FILE", cfgfile);

    char * logfile = cfg_getstr("MONGOPROXY_LOG_FILE", "log/mongoproxy.log");
    log_init(logfile);

    int max_files = cfg_getint32("MONGOPROXY_MAX_FILES", 65535);
    util_set_max_files(max_files);

    argc -= optind;
    argv += optind;

    return 0;
}

int main(int argc, char **argv){
    init(argc, argv);

    return 0;
}
