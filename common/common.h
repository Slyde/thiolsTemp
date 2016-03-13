/** common.h
 * Contains all the common definitions
 *
 * S. Decastel
 */

#ifndef __COMMON_H
#define __COMMON_H

// Folders definitions
#define CONFIG_FOLDER "/etc/thiolsTemp/"
#define VAR_FOLDER "/var/thiolsTemp/"
#define BACKUP_FOLDER "/usr/share/thiosTemp/backup/"

// Files name definitions
#define DB_FILENAME "thiolsTemp.db"
#define CONFIG_FILENAME "thiolsTemp.config



// Files full paths
#define DB_INIT_FILENAME BACKUP_FOLDER""DB_FILENAME
#define DB_FILE VAR_FOLDER""DB_FILENAME


#endif

