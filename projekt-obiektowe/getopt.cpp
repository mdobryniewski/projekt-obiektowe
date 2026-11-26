/*
 * getopt_long implementation for Windows
 * Compatible with MSVC
 */

#include "getopt.h"
#include <string.h>
#include <stdio.h>

char* optarg = NULL;
int optind = 1;
int opterr = 1;
int optopt = 0;

static char* nextchar = NULL;

int getopt(int argc, char* const argv[], const char* optstring)
{
    if (optind == 0) {
  optind = 1;
 nextchar = NULL;
    }

    optarg = NULL;

    if (nextchar == NULL || *nextchar == '\0') {
        if (optind >= argc || argv[optind][0] != '-' || argv[optind][1] == '\0') {
     return -1;
   }
     if (strcmp(argv[optind], "--") == 0) {
     optind++;
       return -1;
        }
    nextchar = argv[optind] + 1;
        optind++;
    }

    char c = *nextchar++;
    const char* temp = strchr(optstring, c);

    if (temp == NULL || c == ':') {
        optopt = c;
        if (opterr) {
            fprintf(stderr, "Unknown option: -%c\n", c);
        }
        return '?';
    }

    if (temp[1] == ':') {
        if (*nextchar != '\0') {
            optarg = nextchar;
       nextchar = NULL;
  }
  else if (optind < argc) {
    optarg = argv[optind];
          optind++;
   }
        else {
   optopt = c;
        if (opterr) {
   fprintf(stderr, "Option -%c requires an argument\n", c);
     }
   return optstring[0] == ':' ? ':' : '?';
        }
    }

    return c;
}

int getopt_long(int argc, char* const argv[], const char* optstring,
    const struct option* longopts, int* longindex)
{
    if (optind == 0) {
        optind = 1;
        nextchar = NULL;
    }

    optarg = NULL;

    if (nextchar != NULL && *nextchar != '\0') {
      return getopt(argc, argv, optstring);
    }

    if (optind >= argc || argv[optind][0] != '-' || argv[optind][1] == '\0') {
  return -1;
    }

    if (strcmp(argv[optind], "--") == 0) {
        optind++;
      return -1;
    }

    if (argv[optind][0] == '-' && argv[optind][1] == '-') {
        // Long option
        const char* namestart = argv[optind] + 2;
        const char* nameend = strchr(namestart, '=');
        size_t namelen = nameend ? (size_t)(nameend - namestart) : strlen(namestart);

const struct option* p;
        const struct option* pfound = NULL;
        int exact = 0;
        int ambig = 0;

        for (p = longopts; p->name; p++) {
      if (strncmp(p->name, namestart, namelen) == 0) {
                if (strlen(p->name) == namelen) {
   pfound = p;
    exact = 1;
    break;
          }
         else if (pfound == NULL) {
          pfound = p;
          }
                else {
           ambig = 1;
            }
     }
      }

  if (ambig && !exact) {
        if (opterr) {
     fprintf(stderr, "Ambiguous option: %s\n", argv[optind]);
  }
    optind++;
     return '?';
   }

        if (pfound != NULL) {
       optind++;
   if (longindex != NULL) {
    *longindex = (int)(pfound - longopts);
      }

            if (pfound->has_arg == required_argument || pfound->has_arg == optional_argument) {
      if (nameend) {
       optarg = (char*)(nameend + 1);
       }
         else if (pfound->has_arg == required_argument) {
  if (optind < argc) {
   optarg = argv[optind++];
        }
             else {
     if (opterr) {
             fprintf(stderr, "Option --%s requires an argument\n", pfound->name);
             }
   return optstring[0] == ':' ? ':' : '?';
}
       }
            }

            if (pfound->flag != NULL) {
*(pfound->flag) = pfound->val;
     return 0;
            }
    return pfound->val;
    }

        if (opterr) {
       fprintf(stderr, "Unrecognized option: %s\n", argv[optind]);
        }
  optind++;
        return '?';
    }

    // Short option
    nextchar = argv[optind] + 1;
    optind++;
 return getopt(argc, argv, optstring);
}
