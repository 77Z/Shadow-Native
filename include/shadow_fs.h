//
// Created by Vince on 4/3/22.
//

#ifndef SHADOW_FS_H
#define SHADOW_FS_H

#include <leveldb/db.h>
#include <string>

namespace Shadow {
	namespace fs {
		leveldb::DB* openDB(std::string dbname);
		void closeDB(leveldb::DB* db);
		void writeDB(leveldb::DB* db, leveldb::Slice key, std::string value);
	}
}

#endif // SHADOW_FS_H
