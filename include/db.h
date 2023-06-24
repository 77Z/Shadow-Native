#ifndef SHADOW_NATIVE_DB_H
#define SHADOW_NATIVE_DB_H

#include <leveldb/db.h>
#include <string>

namespace Shadow {

class Database {
public:
	Database(std::string dbName);
	~Database();

	Database(const Database&) = delete;
	Database& operator=(const Database&) = delete;

	std::string read(leveldb::Slice key);
	void write(leveldb::Slice key, std::string value);
	void remove(leveldb::Slice key);

private:
	std::string dbName;
	leveldb::DB* db;
};

}

#endif