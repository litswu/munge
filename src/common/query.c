/*****************************************************************************
 *  Written by Chris Dunlap <cdunlap@llnl.gov>.
 *  Copyright (C) 2007-2017 Lawrence Livermore National Security, LLC.
 *  Copyright (C) 2002-2007 The Regents of the University of California.
 *  UCRL-CODE-155910.
 *
 *  This file is part of the MUNGE Uid 'N' Gid Emporium (MUNGE).
 *  For details, see <https://dun.github.io/munge/>.
 *
 *  MUNGE is free software: you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License as published by the Free
 *  Software Foundation, either version 3 of the License, or (at your option)
 *  any later version.  Additionally for the MUNGE library (libmunge), you
 *  can redistribute it and/or modify it under the terms of the GNU Lesser
 *  General Public License as published by the Free Software Foundation,
 *  either version 3 of the License, or (at your option) any later version.
 *
 *  MUNGE is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  and GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  and GNU Lesser General Public License along with MUNGE.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *****************************************************************************/


#if HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include <errno.h>
#include <grp.h>
#include <limits.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "common.h"
#include "query.h"
#include "xgetpw.h"


int
query_uid (const char *user, uid_t *uid_ptr)
{
    xpwbuf_p       pwbufp;
    struct passwd  pw;
    uid_t          uid;
    long int       l;
    char          *end_ptr;
    int            rv;

    if (user == NULL) {
        errno = EINVAL;
        return (-1);
    }
    pwbufp = xgetpwbuf_create (0);
    if (pwbufp == NULL) {
        return (-1);
    }
    if (xgetpwnam (user, &pw, pwbufp) == 0) {
        uid = pw.pw_uid;
        rv = 0;
    }
    else {
        errno = 0;
        l = strtol (user, &end_ptr, 10);
        if ((errno == ERANGE) && ((l == LONG_MIN) || (l == LONG_MAX))) {
            rv = -1;
        }
        else if ((user == end_ptr) || (*end_ptr != '\0')) {
            rv = -1;
        }
        else if ((l < 0) || ((unsigned int) l > UID_MAXIMUM)) {
            rv = -1;
        }
        else {
            uid = (uid_t) l;
            rv = 0;
        }
    }
    if ((uid_ptr != NULL) && (rv == 0)) {
        *uid_ptr = uid;
    }
    xgetpwbuf_destroy (pwbufp);
    return (rv);
}


int
query_gid (const char *group, gid_t *gid_ptr)
{
    struct group  *gr_ptr;
    gid_t          gid;
    long int       l;
    char          *end_ptr;

    if (group == NULL) {
        errno = EINVAL;
        return (-1);
    }
    gr_ptr = getgrnam (group);
    if (gr_ptr != NULL) {
        gid = gr_ptr->gr_gid;
    }
    else {
        errno = 0;
        l = strtol (group, &end_ptr, 10);
        if ((errno == ERANGE) && ((l == LONG_MIN) || (l == LONG_MAX))) {
            return (-1);
        }
        if ((group == end_ptr) || (*end_ptr != '\0')) {
            return (-1);
        }
        if ((l < 0) || ((unsigned int) l > GID_MAXIMUM)) {
            return (-1);
        }
        gid = (gid_t) l;
    }
    if (gid_ptr != NULL) {
        *gid_ptr = gid;
    }
    return (0);
}
